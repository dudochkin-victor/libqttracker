/*
 * This file is part of LibQtTracker project
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
 * rdfservice_p.cpp
 *
 *  Created on: Mar 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "rdfservice_p.h"
#include "rowstoremodel_p.h"
#include "include/sopranolive/liveautogenerationbase.h"
#include "util_p.h"

namespace SopranoLive
{

	RDFServicePtr RDFGraphBasic::service() const
	{
            if( external_parent_ref_ )
                return external_parent_ref_;

            //return parent_;
            // in this case parent is self so toStrongRef will always succee
            return parent_.toStrongRef();
	}

	QUrl RDFGraphBasic::graphIri() const
	{
		return graph_iri;
	}

	QVector<QStringList> RDFGraphBasic::rawExecuteQuery(QString const &query, int type)
	{
		return parent_.toStrongRef()->rawExecuteQueryOnGraph(query, type, *this);
	}
	void RDFGraphBasic::rawInsertTriple
			( QString const &subject, QString const &predicate, QString const &object)
	{
		return parent_.toStrongRef()->rawInsertTripleToGraph(subject, predicate, object, *this);
	}
	void RDFGraphBasic::rawDeleteTriple
			( QString const &subject, QString const &predicate, QString const &object)
	{
		return parent_.toStrongRef()->rawDeleteTripleFromGraph(subject, predicate, object, *this);
	}
	void RDFGraphBasic::rawLoad(QUrl const &file_uri)
	{
		return parent_.toStrongRef()->rawLoadOnGraph(file_uri, *this);
	}

	QUrl RDFGraphBasic::createUniqueIri(QString stem_text)
	{
		return parent_.toStrongRef()->createUniqueIri(stem_text);
	}

	LiveNode RDFGraphBasic::createLiveNode(CppType type)
	{
		return liveNode
			( type.metaTypeId() == qMetaTypeId<QUrl>() || type.metaTypeId() >= QMetaType::User
				? Node(createUniqueIri("generated_unique_id"))
				: rawNodeFromString(QString(), type)
			);
	}

	void RDFGraphBasic::addPrefix(QString prefix, QUrl value)
	{
		QString ns = value.toString();
		prefixes[prefix] = ns;
		reverse_prefixes[ns] = prefix;
	}

	QUrl RDFGraphBasic::prefixSuffix(QString prefix, QString suffix)
	{
		Prefixes::iterator pi = prefixes.find(prefix);
		if(pi != prefixes.end())
			return *pi + suffix;
		if(parent_)
                        parent_.toStrongRef()->prefixSuffix(prefix, suffix);
		warning() << "SopranoLive: unrecognized iri prefix \"" + prefix + "\"";
		return QUrl();
	}

	QPair<QString, QString> RDFGraphBasic::splitToPrefixSuffix(QString const &iri)
	{
		Prefixes::iterator pi = reverse_prefixes.upperBound(iri);
		if(pi != reverse_prefixes.begin())
		{
			--pi;
			if(iri.midRef(0, pi.key().size()) == pi.key())
				return qMakePair(pi.value(), iri.mid(pi.key().size()));
		}
		return qMakePair(QString(), iri);
	}

	LiveNode RDFGraphBasic::implLiveNode
			( LiveNode const &node, LiveNode const &type, bool strict
			, QList<QUrl> const &resource_contexts)
	{
		if(!node.isValid())
			return node;

		// if this graph implements the any of the supplied resource_contexts,
		// it is given that resource exists and has requested types, thus skip checks
		if(!node.isLiteral()
				&& (!resource_contexts.size() || !isAnyContextImplemented(resource_contexts)))
		{
			RDFVariable type_var(type.isValid()
					? type.variable()
					: RDFVariable(rdfs::Resource::iri()));
			if(strict)
			{
				RDFVariable checker = RDFVariable::fromType(type_var);
				checker == node;
				if(!modelVariable(checker)->rowCount())
					return LiveNode();
			} else
				// derived type implies all base types, suffices to add only it
				executeQuery(service_context_data_->update()
						.addInsertion(node, rdf::type::iri(), type_var));
		}

		if(node.isLive() && node->graph() == this)
			// checks have been made and node already is on this graph, return it directly
			return node;

		// otherwise, create a new node
		return newLiveResource(node)->initialSharedFromThis<LiveResource>();
	}

	LiveNodes RDFGraphBasic::implLiveNodes
	( LiveNodeList const &urls, RDFVariable const &object_info,
	  RDFStrategyFlags model_strategy, bool strict,
	  QSharedPointer<QObject> const &parent)
	{
		// TODO: something maybe
		Q_UNUSED(urls); Q_UNUSED(object_info); Q_UNUSED(model_strategy); Q_UNUSED(strict); Q_UNUSED(parent);
		critical() << "Here is no code; it's in RDFGraphCache";
		return LiveNodes();
	}

	ResourceOfService *RDFGraphBasic::newLiveResource(Node const &node)
	{
		return new ResourceOfService(sharedFromThis<RDFGraphBasic>(), node);
	}


	Node RDFGraphBasic::rawNodeFromString(QString string, CppType type)
	{
		return parent_.toStrongRef()->rawNodeFromString(string, type);
	}

	QString RDFGraphBasic::rawStringFromNode(Node const &node)
	{
		return parent_.toStrongRef()->rawStringFromNode(node);
	}

	void RDFGraphBasic::insertTriple
			( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object)
	{
		return parent_.toStrongRef()->insertTripleToGraph(subject, predicate, object, *this);
	}
	void RDFGraphBasic::deleteTriple
			( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object)
	{
		return parent_.toStrongRef()->deleteTripleFromGraph(subject, predicate, object, *this);
	}

	LiveNodes RDFGraphBasic::modelQuery
			( QString const &select, QStringList const &columns, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
	{
		return parent_.toStrongRef()->modelQuery
				(select, columns, model_strategy, parent, triple_module);
	}
	LiveNodeModelBasePtr RDFGraphBasic::baseImplModelQuery
			( RDFDerivedSelect *select, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
	{
		return parent_.toStrongRef()->baseImplModelQuery
				(select, model_strategy, parent, triple_module);
	}
	QList<LiveNodes> RDFGraphBasic::executeQuery
			( RDFQuery const &query, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return parent_.toStrongRef()->executeQuery(query, model_strategy, parent);
	}

	LiveNodes RDFGraphBasic::implModelResources(RDFProperty &resource_info)
	{
		LiveNode type = liveNode(resource_info.predicate().metaValue());
		resource_info.setPredicate(rdf::type::iri());
		resource_info.setStrategy(resource_info.strategy() | RDFStrategy::Subject);
		return liveNode(type)->implGetProperties(resource_info);
	}

	void RDFGraphBasic::addRemoveResourceClause(RDFStatementList *removed_statements, RDFVariable const &res)
	{
		RDFStatement sts = RDFStatement(RDFVariable(), RDFVariable(), RDFVariableLink(res));
		sts.subject().property(sts.predicate()) = sts.object();
		RDFStatement sto = RDFStatement(RDFVariableLink(res));
		sto.subject().property(sto.predicate()) = sto.object();
		*removed_statements << sts << sto;
	}

	RDFStatementList *RDFGraphBasic::recurseRemoveResources
			(RDFStatementList *removed_statements, RDFVariable const &res
			, QLinkedList<RDFVariable> *post_removes)
	{
		QVector<RDFProperty> const dp = res.derivedProperties();
		for(QVector<RDFProperty>::const_iterator cdpi = dp.begin(), cdpiend = dp.end()
				; cdpi != cdpiend; ++cdpi)
		{
			RDFStrategyFlags str = cdpi->strategy();
			if(!(str & (RDFStrategy::Owns | RDFStrategy::Shares)))
				continue;

			RDFVariable derived =
					( (str & RDFStrategy::NonOptionalValued) || !(str & RDFStrategy::Owns)
							? RDFVariableLink(res) : RDFVariableLink(res).optional())
					.property(cdpi->deepCopy());
			if(str & RDFStrategy::Owns)
			{
				if(str & RDFStrategy::NonMultipleValued)
					recurseRemoveResources(removed_statements, derived, post_removes);
				else
				{
					RDFStatementList derived_removes;
					executeQuery(RDFUpdate(service_context_data_->update())
							.addDeletion(*recurseRemoveResources
									(&derived_removes, derived, post_removes)));
					;
				}
			} else
			if(str & RDFStrategy::Shares)
			{
				// Expensive, but can't really be avoided unless backend supports sophisticated
				// conditional cascading delete.
				// This is because either we need to create a horribly convoluted query for
				// checking whether a shared object being deleted is shared by a resource that is
				// _not_ getting deleted, or then we get the list of shared resources for which
				// a sharer was destroyed, and then requery if all they no longer have sharers
				// after that.
				LiveNodes actual_deriveds = modelVariables(RDFVariableList() << derived);
				if(actual_deriveds->rowCount())
				{
					RDFVariable var = RDFVariable::fromContainer(actual_deriveds);
					var.setDerivedProperties(derived.derivedProperties());
					// TODO: huh.. we should have a reverseProperty here
					var.optional().subject(cdpi->deepCopy()).doesntExist();
					post_removes->push_back(var);
				}
			}
		}
		addRemoveResourceClause(removed_statements, res);
		return removed_statements;
	}

	void RDFGraphBasic::removeResources(RDFVariable const &variable)
	{
		QLinkedList<RDFVariable> removes;
		removes << variable;
		RDFTransactionPtr tx = createTransaction();
		while(1)
		{
			RDFStatementList removed_statements;
			executeQuery
					(RDFUpdate(service_context_data_->update())
					.addDeletion(*recurseRemoveResources
							(&removed_statements, removes.front(), &removes)));
			removes.pop_front();
			tx->commit();
			if(!removes.size())
				return;
			if(!(tx = createTransaction(RDFTransaction::Exclusive)))
			{
				warning() << "using recursive sharing removes requires that "
						"no transaction is active on the service";
				return;
			}
		}
	}

	TripleUpdateContext::Mode RDFGraphBasic::registerForTripleUpdates
			(TripleUpdateContextPtr const &context)
	{
		// TODO: context unused
		Q_UNUSED(context);
		return TripleUpdateContext::Inactive;
	}

	bool RDFGraphBasic::isAnyContextImplemented(QList<QUrl> const &contexts)
	{
		for(QList<QUrl>::const_iterator cci = contexts.begin(), cciend = contexts.end()
				; cci != cciend; ++cci)
			if(implemented_contexts_.find(cci->toString()) != implemented_contexts_.end())
				return true;
		return false;
	}

	void RDFGraphBasic::addImplementedContext(QUrl const &context)
	{
		++implemented_contexts_[context.toString()];
	}

	RDFServiceBasic::RDFServiceBasic
			( QString name, QList<QUrl> const &implemented_contexts
			, QSharedPointer<RDFServiceBasic> const &parent
			, ServiceContextDataPtr const &ctxt)
		: RDFGraphBasic(parent, ctxt)
		, name_(name)
		, transaction_()
	{
            /*if(!this->parent_)
                        this->parent_ = initialSharedFromThis<RDFServiceBasic>();
                        */
		addImplementedContext(Contexts::NoChecks::iri());
		for(QList<QUrl>::const_iterator cici = implemented_contexts.begin()
				, ciciend = implemented_contexts.end(); cici != ciciend; ++cici)
			addImplementedContext(*cici);
		loadPrefixes();
		attributes_["debug_message_count"] = QVariant(0);
	}

	RDFServiceBasic::~RDFServiceBasic()
	{
		debug(3) << "RDFServiceBasic(" << name_ << ") destroyed";
	}


	void RDFServiceBasic::loadPrefixes()
	{
		// TODO: Think, is this sane? Are more needed? Less?
		// answer to above: prefix system might be unnecessary,
		// conveniece namespaces should be used instead
		// answer to above: prefix system is internally required in
		// converting full uris back to prefixes
		// conclusion: more is better, cannot hurt and the ontologies are stable
		addPrefix("rdf", QUrl("http://www.w3.org/1999/02/22-rdf-syntax-ns#"));
		addPrefix("rdfs", QUrl("http://www.w3.org/2000/01/rdf-schema#"));

		addPrefix("a", QUrl("http://www.w3.org/2000/10/annotation-ns#"));

		addPrefix("xsd", QUrl("http://www.w3.org/2001/XMLSchema#"));

		addPrefix("owl", QUrl("http://www.w3.org/2002/07/owl#"));

		addPrefix("skos", QUrl("http://www.w3.org/2004/02/skos/core#"));

		addPrefix("geo", QUrl("http://www.w3.org/2003/01/geo/wgs84_pos#"));

		addPrefix("dc", QUrl("http://purl.org/dc/elements/1.1/"));
		addPrefix("dcterms", QUrl("http://purl.org/dc/terms/"));
		addPrefix("dctype", QUrl("http://purl.org/dc/dcmitype/"));
		addPrefix("dcam", QUrl("http://purl.org/dc/dcam/"));

		addPrefix("foaf", QUrl("xmlns.com/foaf/0.1/"));
	}

	void RDFServiceBasic::insertTripleToGraph
			( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
			, RDFGraphImplBase const &/*graph*/)
	{
		if(!transaction_)
		{
			RDFUpdate insertion = service_context_data_->update();
			insertion.addInsertion(subject, predicate, object);
			executeQuery(insertion);
		} else
			transaction_->update_.addInsertion(subject, predicate, object);
		// TODO: implement graphs
		//rawInsertTripleToGraph
		//		( stringFromNode(subject), stringFromNode(predicate), stringFromNode(object)
		//		, graph);
	}

	void RDFServiceBasic::deleteTripleFromGraph
			( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
			, RDFGraphImplBase const &/*graph*/)
	{
		if(!transaction_)
		{
			RDFUpdate deletion = service_context_data_->update();
			deletion.addDeletion(subject, predicate, object);
			executeQuery(deletion);
		} else
			transaction_->update_.addDeletion(subject, predicate, object);
		// TODO: implement graphs
		//rawDeleteTripleFromGraph
		//		( stringFromNode(subject), stringFromNode(predicate), stringFromNode(object)
		//		, graph);
	}

	RDFTransactionPtr RDFServiceBasic::createTransaction(RDFTransaction::Mode mode)
	{
		QSharedPointer<Transaction> mid;
		bool initiate = !(mode & RDFTransaction::Inactive);
		mode |= RDFTransaction::Inactive;

		if(transaction_ && transaction_->canAdopt(mode))
			mid = transaction_->sharedFromThis<Transaction>();
		else if(!initiate || !transaction_)
			mid = QSharedCreator<Transaction>::create(sharedFromThis<RDFServiceBasic>(), mode);
		else
			return RDFTransactionPtr();

		RDFTransactionPtr ret = mode & RDFTransaction::Exclusive
			? RDFTransactionPtr(mid)
			: RDFTransactionPtr(QSharedCreator<InPlaceSubTransaction>::create
					(mid, mode));

		if(initiate)
			ret->reinitiate();
		return ret;
	}

	RDFTransactionPtr RDFServiceBasic::pendingTransaction() const
	{
		if(transaction_)
			return transaction_->sharedFromThis<RDFTransaction>();
		return RDFTransactionPtr();
	}

	RDFServicePtr RDFServiceBasic::createChildService()
	{
		// TODO: implement subservices
		return RDFServicePtr();
	}

	QDebug RDFServiceBasic::logMessage(int level, char const *func, char const *loc) const
	{
		int dmsgc = attributes_["debug_message_count"].toInt();
		attributes_["debug_message_count"] = QVariant(dmsgc + 1);
		QDebug log = parentContext()->logMessage(level, func, loc);
		log.nospace() << "[RDFService(" << name_ << ' ' << (void *)this << ") "
					  << qSetFieldWidth(4) << dmsgc << qSetFieldWidth(0) << "]";
		return log.space();
	}

	QVariant RDFServiceBasic::serviceAttribute(QString const &attribute_name) const
	{
		AttributeMap::const_iterator ai = attributes_.find(attribute_name);
		if(ai != attributes_.end())
			return *ai;
		return QVariant();
	}

	QVariantMap RDFServiceBasic::serviceAttributes() const
	{
		return attributes_;
	}

	//! Accepts all attributes which make it here.
	bool RDFServiceBasic::setServiceAttribute
			( QString const &service_attribute, QVariant const &value)
	{
		if(!value.isValid())
			attributes_.remove(service_attribute);
		attributes_[service_attribute] = value;
		return true;
	}

	// TODO: implement shared parent aggregation
	RowStoreModelPtr RDFServiceBasic::newRowStoreModel
			( RDFStrategyFlags strategy_flags, ContentModulePtr const &content_module
			, QSharedPointer<QObject> const &parent)
	{
		Q_UNUSED(parent);
		RowStoreModelPtr ret = QSharedCreator<RowStoreModel>::create
				(sharedFromThis<RDFServiceBasic>(), strategy_flags);
		ret->changeContentModule(content_module);
		return ret;
	}

	// TODO: implement shared parent aggregation
	TripleModelPtr RDFServiceBasic::newTripleModel
			( RDFStrategyFlags strategy_flags, ContentModulePtr const &content_module
			, TripleModulePtr const &cache_module, QSharedPointer<QObject> const &parent)
	{
		Q_UNUSED(parent);
		TripleModelPtr ret = QSharedCreator<TripleModel>::create
				(sharedFromThis<RDFServiceBasic>(), strategy_flags);
		ret->changeContentModule(content_module);
		ret->changeTripleModule(cache_module);
		return ret;
	}

}
