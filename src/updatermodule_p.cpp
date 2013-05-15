/*
 * This file is part of libqttracker project
 *
 * Copyright (C) 2009, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * updatermodule_p.cpp
 *
 *  Created on: Apr 7, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "updatermodule_p.h"
#include "rdfderivedselect_p.h"
#include "util_p.h"

namespace SopranoLive
{
	UpdaterModule::UpdaterModule(RDFStrategyFlags responsibility_modes)
		: QObject()
		, responsibility_modes_(responsibility_modes | OmnipresentResponsibilityModes)
		, streaming_remaining_limit_(-1)
	{
		if(!(responsibility_modes_ & (RDFStrategy::Streaming | RDFStrategy::Windowed)))
			responsibility_modes_ &= ~RDFStrategy::Running;
	}

	void UpdaterModule::attached(TripleModel &triplemodel)
	{
		ModuleOfDerivedModel<TripleModel>::attached(triplemodel);
		// we're going to cache the old module, so that we can return it on detach.
		// if you're attaching and detaching modules employing this same tactic, make
		// sure they're detached in reverse order they were attached.

		old_columns_ = model()->columns_;

		// add additional orderings on the model columns, guaranteeing sorted, uniquely
		// identified rows which are required by both streaming and live
		if(responsibility_modes_ & (RDFStrategy::Streaming | RDFStrategy::Live))
		{
			for(int i = 0, iend = model()->columns_.baseColumnCount(); i != iend; ++i)
				if(model()->columns_[i].strategy() & RDFStrategy::IdentityColumn)
					model()->columns_.orderBy(i, true);
			model_ordering_ = model()->columns_.getOrderBys();
		}
		if(responsibility_modes_ & RDFStrategy::Streaming)
		{
			RDFServicePtr svc = triplemodel.graph()->service();

			int sbs = model()->modelAttribute("streaming_block_size").toInt();
			if(sbs <= 0)
				sbs = 200;
			model()->setModelAttribute("streaming_block_size", QVariant(sbs));

			int sfbs = model()->modelAttribute("streaming_first_block_size").toInt();
			if(sfbs <= 0)
				sfbs = sbs;
			model()->setModelAttribute("streaming_first_block_size", QVariant(sfbs));

			int limit = triplemodel.columns_.getLimit();
			if(limit == RDFSelect::NoLimit)
				limit = -1;
			model()->setModelAttribute("streaming_limit", QVariant(limit));
		}
		RDFStrategyFlags our_states_on_model = responsibility_modes_ & triplemodel.strategy();
		changeState(our_states_on_model, our_states_on_model);
	}

	void UpdaterModule::detached(TripleModel &triplemodel)
	{
		Q_UNUSED(triplemodel);
		active_update_contexts_.clear();
		model()->columns_ = old_columns_;
		ModuleOfDerivedModel<TripleModel>::detached(triplemodel);
	}

	bool UpdaterModule::alterStrategy_chain
			( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
			, InterfaceChain<ModelCommons> chain)
	{
		// diagnostics
		RDFStrategyFlags our_responsibilities = responsibility_modes_ & mask;

		// diagnostics: check for requests which are our responsibility but we must reject
		if(our_responsibilities & flags & (RDFStrategy::Streaming | RDFStrategy::Running)
				&& seenEndOfStream())
		{
			// requesting streaming enable, but end of stream seen
			warning(model(), "LiveNodeModel::alterStrategy") << "end of stream seen";
		}
		// delegation: done checking locally, ask remainder of the module chain
		else if(ModelCommons::alterStrategy_chain(flags, mask & ~our_responsibilities, index, chain))
		{
			// implementation: chain accepts state changes as well, go ahead and perform ours
			changeState(flags, our_responsibilities);
			return true;
		}

		return false;
	}

	ModelCommons::ReadyFlags UpdaterModule::refreshModel_chain
			( LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
	{
		if(responsibility_modes_ & RDFStrategy::Streaming)
		{
			if(mode & LiveNodeModel::Flush)
				warning(model()) << "LiveNodeModel::refreshModel(Flush): "
					"not implemented yet in streaming mode";
			else
			if(mode & LiveNodeModel::Block && streaming_update_)
				streaming_update_->source_->refreshModel(mode);
			// TODO: handle derived properties too.
			return seenEndOfStream() ? ReadyFlags(AllReady) : ReadyFlags();
		}
		return ModelCommons::refreshModel_chain(mode, chain);
	}

	//! Checks if the updater module needs to perform update queries when given set of \a
	//! triples is received in given \a context, affecting given \a col.
	//! \return RDFStrategy::Object if it was an object property of col that changed,
	//! and/or RDFStrategy::Subject if it was a subject property.
	RDFStrategyFlags UpdaterModule::constrainUpdateQuery
			( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
			, RDFDerivedColumn *col, RDFDerivedSelect *update_select)
	{
		Q_UNUSED(context);

		/*
				<< "UpdaterModule::constrainUpdateQuery: triples"
				<< "(" << triples.subjects() << triples.predicates() << triples.objects() << "/&"
				<< triples.mask() << "), "
				<< "col.index(" << col.index() << ")";
		*/

		RDFStrategyFlags ret = 0;
		ChainContext const *chain_context = 0;

		RDFVariableLink col_var_link(col->variable());

		RDFPattern first_alternate = col_var_link.pattern().child();

		col->setVariable(col_var_link);

		RDFVariableList objects, subjects;
		RDFService *svc = model()->graph()->service().data();
		for(QStringList::const_iterator coi = triples.objects().constBegin()
				, coiend = triples.objects().constEnd(); coi != coiend; ++coi)
			objects.push_back(svc->rawNodeFromString(*coi, CppType::of<QUrl>()).uri());
		for(QStringList::const_iterator csi = triples.subjects().constBegin()
				, csiend = triples.subjects().constEnd(); csi != csiend; ++csi)
			subjects.push_back(svc->rawNodeFromString(*csi, CppType::of<QUrl>()).uri());

		if(!triples.predicates().size())
		{
			// << "\tNeeded, empty predicates given";
			ret = RDFStrategy::SubjectObject;
		} else
		if(triples.predicates() == rdf_type_as_list_g)
		{
			// "\tNeeded, predicate rdf:type denotes additions or removals";
			ret = RDFStrategy::Object;
		} else if(!(chain_context = dynamic_cast<ChainContext const *>(context.data())))
		{
			// TODO: remove dependency from RDFDerivedColumnPrivate
			RDFDerivedColumnPrivate::Deriveds const &cdeps
					= RDFDerivedColumnPrivate::d_func(*col)->refDeriveds();
			QStringList const preds = triples.predicates();

			// << "\tHas deriveds(" << cdeps << ")";

			for(QStringList::const_iterator cpi = preds.constBegin()
					, cpiend = preds.constEnd(); cpi != cpiend; ++cpi)
			{
				RDFDerivedColumnPrivate::Deriveds::const_iterator cdci;
				if((cdci = cdeps.find(RawPropertyData::fromObject(*cpi))) != cdeps.end())
				{
					if(subjects.size())
						ret |= RDFStrategy::Object;
					if(objects.size())
					{
						first_alternate
								.union_()
								.variable(RDFVariableLink
										((*update_select)[cdci.value()].variable()))
								.isMemberOf(objects);
						ret |= RDFStrategy::UpdateProperty;
					}
				}
				if((cdci = cdeps.find(RawPropertyData::fromSubject(*cpi))) != cdeps.end())
				{
					if(objects.size())
						ret |= RDFStrategy::Subject;
					if(subjects.size())
					{
						first_alternate
								.union_()
								.variable(RDFVariableLink
										((*update_select)[cdci.value()].variable()))
								.isMemberOf(subjects);
						ret |= RDFStrategy::UpdateProperty;
					}
				}
			}
		} else // chain_context
		{
			RawPropertyData prop(chain_context->link_);

			QStringList const preds = triples.predicates();

			for(QStringList::const_iterator cpi = preds.constBegin()
					, cpiend = preds.constEnd(); cpi != cpiend; ++cpi)
				if(prop.predicate() == *cpi)
				{
					RDFVariable target = first_alternate
							.variable(RDFVariableLink(chain_context->link_.target()));

					if(prop.strategy() & RDFStrategy::Object && subjects.size())
						target.subject(chain_context->link_.predicate().metaValue().uri())
								.isMemberOf(subjects);
					else
					if(prop.strategy() & RDFStrategy::Subject && objects.size())
						target.object(chain_context->link_.predicate().metaValue().uri())
								.isMemberOf(objects);

					if((prop.strategy() & RDFStrategy::Subject && subjects.size())
							|| (prop.strategy() & RDFStrategy::Object && objects.size()))
						target.isMemberOf(prop.strategy() & RDFStrategy::Object
										? objects
										: subjects);
					ret |= RDFStrategy::ChainedProperty;
				}

		}

		RDFVariableList col_ups;
		if(ret & RDFStrategy::Object)
			col_ups = subjects;
		if(ret & RDFStrategy::Subject)
			col_ups += objects;

		if(col_ups.size() && !chain_context)
		{
			first_alternate
					.variable(chain_context
							? chain_context->link_.source()
							: RDFPattern().variable(RDFVariableLink(col->variable())))
					.isMemberOf(objects + subjects);
		}

		// << "\tReturning" << ret;
		return ret;
	}

	bool UpdaterModule::tripleUpdates_chain
			( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
			, InterfaceChain<TripleUpdateReceiver> chain)
	{
		RDFDerivedSelect update_sel = model()->columns_;

		RDFDerivedColumn &col = update_sel[context->index_.column()];

		// << __func__ << triples << "\n" << update_sel.getQuery();
		// << "\ttripleUpdates deriveds(" << col.refDeriveds()
		//		<< ", " << col.refDeriveds().size() << ")";

		if(RDFStrategyFlags updates = constrainUpdateQuery(triples, context, &col, &update_sel))
		{
			/*
					<< "UpdaterModule::tripleUpdates: opts(" << opts << "), "
					<< "mode(" << updates << "), "
					<< "col.index(" << col.index() << ")"
					<< "col.parent(" << col.parent() << ")"
					;
			 */

			LiveNodeModelBasePtr update_model = model()->graph()->baseImplModelQuery
					(&update_sel, RDFStrategy::Running);

			update_model->attachFrontModule(QSharedCreator<UpdateTransitionModule>::create());

			SmoothTransitionModule::UpdateKeys updated_keys;
			if(updates && !(updates & RDFStrategy::ChainedProperty))
			{
				QSet<QString> updated_column_values;
				if((updates & RDFStrategy::SubjectObject) == RDFStrategy::Object)
					updated_column_values += QSet<QString>::fromList(triples.subjects());
				if((updates & RDFStrategy::SubjectObject) == RDFStrategy::Subject)
					updated_column_values += QSet<QString>::fromList(triples.objects());
				updated_keys << qMakePair(updated_column_values, col.index());
			}

			pending_updates_[update_model.data()] =
					QSharedCreator<LiveUpdate>::create
							( update_model.staticCast<RowStoreModel>()
							, triples, context, model_ordering_, updated_keys);

			QObject::connect
					( update_model.data(), SIGNAL(derivedsUpdated())
					, this, SLOT(updateModelDerivedsUpdated()));
		}

		TripleUpdateReceiver::tripleUpdates_chain(triples, context, chain);
		return true;
	}

	TripleUpdateContextPtr UpdaterModule::createAndRegisterContext
			( RDFStatement const &statement, int col)
	{
		TripleUpdateContextPtr ret(QSharedCreator<TripleUpdateContext>::create
				( model()->sharedFromThis<TripleModel>()
				, statement
				, model()->areaIndex(-1, col)
				));
		if(model()->graph()->registerForTripleUpdates(ret))
			active_update_contexts_.push_back(ret);
		return ret;
	}

	RDFProperty UpdaterModule::recurseAndRegisterChainUpdates
			( RDFVariable const &source, RDFProperty const &link, int col, bool reg_outermost)
	{
		RDFProperty ret;
		RDFVariable target = source;
		if(link.strategy() & RDFStrategy::SubjectObject)
		{
			ret = link.deepCopy();
			// clear the target so that it doesn't pull in the rest of the property chain
			// we build the update chain separately with incremental inward flowing links
			// retain all derived property information however
			ret.setTarget(RDFVariable());
			ret.target().setDerivedProperties(link.target().derivedProperties());

			target.metaAssign(ret.bind(RDFVariableLink(source)));

			// tracker specific: tracker backend doesn't know how to handle subject properties
			// properly, thus if a property is a Subject property, we're creating a regular
			// chain property for it
			if(reg_outermost || (link.strategy() & RDFStrategy::Subject))
			{
				ChainContextPtr ctxt(QSharedCreator<ChainContext>::create
						( model()->sharedFromThis<TripleModel>()
						, ret.asStatement()
						, model()->areaIndex(-1, col)
						, ret
						));
				if(model()->graph()->registerForTripleUpdates(ctxt))
					active_update_contexts_.push_back(ctxt);
			}
		}

		QVector<RDFProperty> derived_props = link.target().derivedProperties();
		for(QVector<RDFProperty>::const_iterator cdi = derived_props.begin(),
					cdiend = derived_props.end(); cdi != cdiend; ++cdi)
			if(cdi->strategy() & RDFStrategy::UpdateProperty)
			{
				if(link.strategy() & RDFStrategy::SubjectObject)
					recurseAndRegisterChainUpdates(target, *cdi, col);
				else
					ret = recurseAndRegisterChainUpdates(target, *cdi, col);
			}

		return ret;
	}

	void UpdaterModule::changeState(RDFStrategyFlags flags, RDFStrategyFlags mask)
	{
		RDFStrategyFlags
			  enables = mask & flags
			, disables = mask & ~flags;

		RDFDerivedSelect &cols = model()->columns_;
		if(enables & RDFStrategy::Live)
		{
			int c = 0, bases_end = cols.baseColumnCount();
			for(int cend = cols.realColumnCount(); c < cend; ++c)
			{
				RDFDerivedColumn &col = cols[c];

				// chained property columns are handled as part of their parent column
				if(col.strategy() & RDFStrategy::ChainedProperty)
					continue;

				RDFVariable col_var = col.variable();
				// << "Column parent(" << cols[c].parent() << ")";


				// TODO TODO: this is now tracker specific behaviour, generalize if needed
				// tracker specifity: a wildcard triple update is requested for each parent
				// column and the specific predicate information is discarded.
				// Predicate information is checked on the update method itself only.

				RDFStrategyFlags get_mask = 0;

				// for base columns, register for all
				if(c < bases_end)
					get_mask = RDFStrategy::SubjectObject;

				RDFDerivedColumnPrivate::Deriveds const &deriveds =
						RDFDerivedColumnPrivate::cd_func(col)->deriveds();

				// iterate the derived columns of this column
				for(RDFDerivedColumnPrivate::Deriveds::const_iterator
							di = deriveds.constBegin(), diend = deriveds.constEnd()
						; di != diend; ++di)
				{
					RDFStrategyFlags derived_flags = di.key().strategy();
					if(derived_flags & RDFStrategy::ChainedProperty)
					{
						// get the derived properties of the source of the chained property
						QVector<RDFProperty> const chain_deriveds
								= constify(cols[di.value()])
										.propertyData().source().derivedProperties();
						// iterate to find the update chains
						for(QVector<RDFProperty>::const_iterator cpi = chain_deriveds.begin()
								, cpiend = chain_deriveds.end(); cpi != cpiend; ++cpi)
						{
							RDFStrategyFlags chain_source_flags = cpi->strategy();
							if(chain_source_flags & RDFStrategy::UpdateProperty)
							{
								// first links of the chain are handled as part of normal
								// column updates
								get_mask |= chain_source_flags;

								// further links have to be recursed separately.
								RDFPattern exists_pattern_parent = col.variable().pattern();
								RDFPattern exists_pattern = exists_pattern_parent.child();

								exists_pattern.exists();

								RDFVariable var = exists_pattern.variable
										(exists_pattern.child().variable(col_var));

								RDFProperty first_triple_link
										= recurseAndRegisterChainUpdates( var, *cpi, c, false);
								// add the first triple link of the chain as hidden column
								// so that updates can find a match
								cols.addColumn
										( first_triple_link
										, RDFDerivedColumn::HiddenColumn
												| RDFDerivedColumn::MuteColumn
										, c);

							}
						}
					}
					else
						get_mask |= derived_flags;
				}
				if(get_mask & RDFStrategy::Object)
					createAndRegisterContext(RDFStatement(col_var), c);
				if(get_mask & RDFStrategy::Subject)
					createAndRegisterContext(RDFStatement
							(RDFVariable(), RDFVariable(), col_var), c);
			}
			/*
			for(int deriveds_end = ; c < deriveds_end; ++c)
			{
				RDFStatement statement = cols[c].propertyData().asStatementWith(cols[cols[c].parent()].variable());

				// TODO: these might fail and return false/partial. What then?
				TripleUpdateContextPtr ctxt(QSharedCreator<TripleUpdateContext>::create
								( model()->sharedFromThis<TripleModel>()
								, statement
								, model()->areaIndex(-1, c)
								);
				if(model()->graph()->registerForTripleUpdates(ctxt))
					active_update_contexts_.push_back(ctxt);
			}
			*/
		}

		if(responsibility_modes_ & RDFStrategy::Streaming)
		{
			if(enables & RDFStrategy::Streaming)
				enableStreaming();
			else if(disables & RDFStrategy::Streaming)
				disableStreaming();

			if(disables & RDFStrategy::Running)
				cancelStreamPiece();
			else if(enables & (RDFStrategy::Streaming | RDFStrategy::Running))
				if(!initiateStreamPiece(true))
					endOfStream();
		}
	}

	void UpdaterModule::updateModelDerivedsUpdated()
	{
		PendingUpdates::iterator ui = pending_updates_.find
				(static_cast<LiveNodeModel *>(sender()));
		if(ui == pending_updates_.end())
			warning(model()) << "Internal error: no pending update entry found for signal sender" << sender();
		else
		{
			model()->updateFrom(**ui);
			pending_updates_.erase(ui);
		}
	}

	UpdaterModule::LiveUpdate::LiveUpdate
			( RowStoreModelPtr const &model, RawTriplesData const &triples
			, TripleUpdateContextConstPtr const &update_context
			, RDFSelect::OrderByInfo const &model_ordering
			, SmoothTransitionModule::UpdateKeys const &update_keys)
		: UpdateContext(model), source_model_(model)
		, triples_(triples), update_context_(update_context)
		, model_ordering_(model_ordering)
		, update_keys_(update_keys)
	{}

	bool UpdaterModule::LiveUpdate::updateModel(LiveNodeModelBase *target) const
	{
		RowStoreModel *target_model = static_cast<RowStoreModel *>(target);
		SmoothTransitionModule(target_model, ContentModulePtr(), model_ordering_)
				.initialStackShare<SmoothTransitionModule>()->update
				// TODO: perf improvements: now the whole model is traversed at every update
				// find out the minimum necessary range, and replace {0, size()} with it
						( 0, target_model->row_store.size(), source_model_->row_store
						, update_keys_);
		return false;
	}

	UpdaterModule::ChainContext::ChainContext
			( QWeakPointer<TripleUpdateReceiver> const &receiver, RDFStatement const &triple_mask
			, QModelIndex const &index, RDFProperty const &link)
		: TripleUpdateContext(receiver, triple_mask, index), link_(link)
	{}

	bool UpdaterModule::UpdateTransitionModule::updateTargetModule_chain
		( ModelModule *target_module, UpdateContext const &update_context
		, InterfaceChain<ModelCommons const> chain) const
	{
		// TODO: implement
		Q_UNUSED(target_module); Q_UNUSED(update_context); Q_UNUSED(chain);
		warning(model()) << "multiple valued derived property updates not implemented";
		return true;
	}

	void UpdaterModule::enableStreaming()
	{
		streaming_remaining_limit_ = model()->modelAttribute("streaming_limit").toInt();
		model()->doEnableStrategyFlags(RDFStrategy::Streaming);
	}

	void UpdaterModule::disableStreaming()
	{
		streaming_remaining_limit_ = -1;
		model()->doDisableStrategyFlags(RDFStrategy::Streaming);
	}

	bool UpdaterModule::initiateStreamPiece(bool first_in_stream)
	{
		if(seenEndOfStream())
			return false;
		if(streaming_update_)
			return true;

		// copy the model derived columns so we can modify and rerun it
		RDFDerivedSelect streampiece_sel(model()->columns_);

		int block_limit = 0;

		if(int rows = model()->rowCount())
		{
			--rows; // last row index

			RDFSelect::OrderByInfo const obs = streampiece_sel.getOrderBys();

			RDFFilter filter(LiteralValue(false));

			for(RDFSelect::OrderByInfo::const_iterator cobi = obs.end(), cobiend = obs.begin()
					; cobi != cobiend; )
			{
				--cobi;
				if(cobi->first < 0)
					// cannot perform streaming ordering by non-column variable
					continue;

				RDFDerivedColumn &col = streampiece_sel.refColumns()[cobi->first];
				RDFVariableLink col_var(col.variable().inParent(RDFVariable::TopmostParent));
				col.setVariable(col_var);

				Node col_last_item = model()->liveNode(rows, cobi->first);

				bool filter_is_empty = filter.metaIsExplicit();
				bool last_item_unbound = col_last_item.isEmpty();

				if(!filter_is_empty)
				{
					filter.metaAssign((last_item_unbound
							? !col_var.isBound()
							: col_var == col_last_item)
						&& filter);
				}

				RDFFilter comp(LiteralValue(false));
				if(last_item_unbound)
				{
					if(cobi->second == RDFSelect::Ascending)
						comp.metaAssign(col_var.isBound());
					else
						// if descending, the unbound values are the last, so the
						// "?col > last_current_item" can never be true, so we omit it.
						continue;
				} else
				if(col_var.metaIsResource())
				{
					comp.metaAssign(cobi->second == RDFSelect::Ascending
							? col_var.filter("STR") > LiteralValue(col_last_item.toString())
							: col_var.filter("STR") < LiteralValue(col_last_item.toString()));
				} else
				{
					comp.metaAssign(cobi->second == RDFSelect::Ascending
							? col_var > col_last_item
							: col_var < col_last_item || !col_var.isBound());
				}

				filter.metaAssign(filter_is_empty ? comp : comp || filter);
			}
			if(filter.metaIsExplicit())
				// means filter is the literal false, there can be no results. Abort here.
				return false;

		}

		block_limit = model()->modelAttribute(first_in_stream
				? "streaming_first_block_size"
				: "streaming_block_size").toInt();

		if(unsigned(streaming_remaining_limit_) < unsigned(block_limit))
			block_limit = streaming_remaining_limit_;

		streampiece_sel.limit(block_limit);

		RowStoreModelPtr streampiece = model()->graph()->baseImplModelQuery
				(&streampiece_sel, RDFStrategy::Running).dynamicCast<RowStoreModel>();

		if(!streampiece)
		{
			warning(model()) << "internal error: null model from backend";
			return false;
		}

		streaming_update_ = QSharedCreator<StreamingUpdate>::create
				(streampiece, this, block_limit);

		model()->doEnableStrategyFlags(RDFStrategy::Running);

		if(streampiece->strategy() & RDFStrategy::Running)
		{
			pending_updates_[streampiece.data()] = streaming_update_;
			QObject::connect
					( streampiece.data(), SIGNAL(derivedsUpdated())
					, this, SLOT(updateModelDerivedsUpdated()));
		} else
			model()->updateFrom(*streaming_update_);

		return true;
	}

	void UpdaterModule::receivedStreamPiece(int size)
	{
		if(!(streaming_remaining_limit_ -= size))
			// autostop streaming at user supplied RDFSelect::limit.
			disableStreaming();

		if(model()->strategy() & RDFStrategy::Streaming)
		{
			if(!initiateStreamPiece(false))
				endOfStream();
		} else
			model()->doDisableStrategyFlags(RDFStrategy::Running);
	}

	void UpdaterModule::cancelStreamPiece()
	{
		if(!streaming_update_)
			return;
		QObject::disconnect
				( streaming_update_->source_, SIGNAL(derivedsUpdated())
				, this, SLOT(updateModelDerivedsUpdated()));
		pending_updates_.remove(streaming_update_->source_);
		streaming_update_.clear();
		model()->doDisableStrategyFlags(RDFStrategy::Running);
	}

	void UpdaterModule::endOfStream(RDFStrategyFlags additional_resets)
	{
		setSeenEndOfStream();
		// TODO: disable RDFStrategy::Partial here.
		model()->doDisableStrategyFlags
				(RDFStrategy::Streaming | RDFStrategy::Running | additional_resets);
		// emit modelUpdated signal. TODO: refactor refreshModel handling into UpdaterModule
		model()->refreshModel(LiveNodeModel::Poll);
	}

	bool UpdaterModule::StreamingUpdate::updateModel(LiveNodeModelBase *target) const
	{
		RowStoreModel *utm = updater_->model();
		if(target != utm)
			warning(utm) << "internal error: requested target model(" << target << ")"
							"is different from the updater model(" << utm << ")";

		updater_->streaming_update_.clear();

		int rows = source_->rowCount();

		if(rows)
			utm->appendRowsImpl(source_->row_store);

		if(rows < last_requested_block_size_)
			updater_->endOfStream();
		else
			updater_->receivedStreamPiece(rows);
		return true;
	}

}
