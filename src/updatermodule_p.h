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
 * updatermodule_p.h
 *
 *  Created on: Apr 7, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_UPDATERMODULE_P_H_
#define SOPRANOLIVE_UPDATERMODULE_P_H_

#include "rdfcache_p.h"
#include "lnm_transitionstrategy_p.h"

namespace SopranoLive
{
	class UpdaterModule
		: public QObject
		, public ModuleOfDerivedModel<TripleModel>
		, public ModuleInterface<ModelCommons, TripleModel, UpdaterModule>
		, public ModuleInterface<TripleUpdateReceiver, TripleModel, UpdaterModule>
	{
		Q_OBJECT

		/* VZR_CLASS(UpdaterModule
				, (QObject)
				, (ModuleOfDerivedModel<TripleModel>)
				, (ModuleInterface<ModelCommons, TripleModel, UpdaterModule>)
				, (ModuleInterface<TripleUpdateReceiver, TripleModel, UpdaterModule>));*/

	public:
		UpdaterModule(RDFStrategyFlags responsibility_modes);

		enum
		{
			OmnipresentResponsibilityModes = RDFStrategy::Running
		,	EndOfStreamSeen = RDFStrategyFlags(1) << RDFStrategy::offset_ContextSpecificFlags
		};

		RDFStrategyFlags responsibility_modes_; // VZR_MEMBER

		bool seenEndOfStream() const { return responsibility_modes_ & EndOfStreamSeen; }
		void setSeenEndOfStream(bool seen = true)
		{
			if(seen)
				responsibility_modes_ |= RDFStrategyFlags(EndOfStreamSeen);
			else
				responsibility_modes_ &= ~RDFStrategyFlags(EndOfStreamSeen);
		}

		QList<RDFVariableLink> column_filters_; // VZR_MEMBER

		typedef QList<QSharedPointer<TripleUpdateContext> > ActiveUpdateContexts;
		ActiveUpdateContexts active_update_contexts_; // VZR_MEMBER

		// contains a single entry for each pending update model.
		typedef QMap<LiveNodeModel const *, UpdateContextPtr> PendingUpdates;
		PendingUpdates pending_updates_; // VZR_MEMBER

		RDFSelect::OrderByInfo model_ordering_; // VZR_MEMBER

		RDFDerivedSelect old_columns_; // VZR_MEMBER

		// overrides of Module

		void attached(TripleModel &model);
		void detached(TripleModel &model);

		// overrides of ModelCommons

		bool alterStrategy_chain
				( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
				, InterfaceChain<ModelCommons> chain);

		ReadyFlags refreshModel_chain
				( LiveNodeModel::RefreshMode, InterfaceChain<ModelCommons> chain);

		// overrides of TripleUpdateReceiver

		bool tripleUpdates_chain
				( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
				, InterfaceChain<TripleUpdateReceiver> chain);

		// local additions

		void changeState(RDFStrategyFlags flags, RDFStrategyFlags mask);

		RDFStrategyFlags constrainUpdateQuery
				( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
				, RDFDerivedColumn *col, RDFDerivedSelect *update_select);

	public Q_SLOTS:
		void updateModelDerivedsUpdated();

	public:
		struct LiveUpdate
			: UpdateContext
		{
			/* VZR_CLASS(LiveUpdate, (UpdateContext));*/

			LiveUpdate
					( RowStoreModelPtr const &model, RawTriplesData const &triples
					, TripleUpdateContextConstPtr const &update_context
					, RDFSelect::OrderByInfo const &model_ordering
					, SmoothTransitionModule::UpdateKeys const &update_keys);

			RowStoreModelPtr source_model_; // VZR_MEMBER
			RawTriplesData triples_; // VZR_MEMBER

			TripleUpdateContextConstPtr update_context_; // VZR_MEMBER
			RDFSelect::OrderByInfo model_ordering_; // VZR_MEMBER

			SmoothTransitionModule::UpdateKeys update_keys_; // VZR_MEMBER

			bool updateModel(LiveNodeModelBase *target) const;
		};

		struct ChainContext
			: TripleUpdateContext
		{
			/* VZR_CLASS(ChainContext, (TripleUpdateContext));*/

			ChainContext
					( QWeakPointer<TripleUpdateReceiver> const &receiver
					, RDFStatement const &triple_mask
					, QModelIndex const &index, RDFProperty const &link);

			RDFProperty link_; // VZR_MEMBER

			//! Contains information on aggregate chained property update dependency chains
		};
		typedef QSharedPointer<ChainContext> ChainContextPtr;

		TripleUpdateContextPtr createAndRegisterContext(RDFStatement const &statement, int col);
		RDFProperty recurseAndRegisterChainUpdates
				( RDFVariable const &source, RDFProperty const &link, int col
				, bool register_outermost = true);

		struct UpdateTransitionModule
			: ModuleOfDerivedModel<TripleModel>
			, ModuleInterface<ModelCommons, TripleModel, UpdaterModule>
		{
			/* VZR_CLASS(UpdateTransitionModule
					, (ModuleOfDerivedModel<TripleModel>)
					, (ModuleInterface<ModelCommons, TripleModel, UpdaterModule>));*/

			bool updateTargetModule_chain
				( ModelModule *target_module, UpdateContext const &update_context
				, InterfaceChain<ModelCommons const> chain) const;
		};

		// streaming strategy handling functions

		void enableStreaming();
		void disableStreaming();

		bool initiateStreamPiece(bool first_in_stream);
		void receivedStreamPiece(int size);
		void cancelStreamPiece();
		void endOfStream(RDFStrategyFlags additional_resets = 0);

		struct StreamingUpdate
			: UpdateContext
		{
			/* VZR_CLASS(StreamingUpdate, (UpdateContext));*/

			RowStoreModel * source_; // VZR_MEMBER
			UpdaterModule * updater_; // VZR_MEMBER

			int last_requested_block_size_; // VZR_MEMBER

			StreamingUpdate(RowStoreModelPtr &source, UpdaterModule *updater, int requested_size)
				: UpdateContext(source), source_(source.data()), updater_(updater)
				, last_requested_block_size_(requested_size)
			{}
			bool updateModel(LiveNodeModelBase *target) const;
		};

		QSharedPointer<StreamingUpdate> streaming_update_; // VZR_MEMBER

		int streaming_block_size_; // VZR_MEMBER
		int streaming_first_block_size_; // VZR_MEMBER

		int streaming_remaining_limit_; // VZR_MEMBER
	};

}
#endif /* SOPRANOLIVE_UPDATERMODULE_P_H_ */
