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
 * dev_doc_global.h
 *
 *  Created on: Mar 8, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_DEV_DOC_GLOBAL_H_
#define SOPRANOLIVE_DEV_DOC_GLOBAL_H_

namespace SopranoLive
{
/*! \defgroup soprano Soprano
 *
 * \page dev_manual Developer manual
 *
 *
 *	\n \n \n
 *	\section dev_manual_intro 1. Introduction
 *
 *	  SopranoLIve is a C++/Qt object - RDF mapper client library.
 *	Understanding, maintaining and developing SopranoLive and its extensions can be a daunting
 *	task. This document exists to help and collect information that does not natively fit inside
 *	the low-level class and member function documentation.
 *
 *	\n \n \n
 *	\section dev_manual_backend 2. Backend services
 *
 *	  Backend service is a way to connect SopranoLive to an external data source. In the basic
 *	form it consists of implementing RDFService using generic implementer classes RDFServiceBasic
 *	and RDFGraphCache, and connecting implementation query functions to the backend specific
 *	SPARQL start points. This form requires that the backend supports SPARQL and SPARQL update
 *	extensions, and will not offer live functionality. From this base platform, the backend can
 *	be extended to offer live functionality, to overcome any missing SPARQL implementation
 *	support limitations, to add domain specific performance optimizations and more.
 *
 *	\section dev_manual_backend_basic 2.1. Basic implementation
 *	\section dev_manual_backend_servicebasic 2.1.1. RDFServiceBasic
 *	\section dev_manual_backend_graphcache 2.1.2. RDFGraphCache
 *	\section dev_manual_backend_live 2.2. Live functionality
 *	\section dev_manual_backend_serviceimplbase 2.3. RDFServiceImplBase
 *	\section dev_manual_backend_modelimplbase 2.4. LiveNodeModelBase
 *	\section dev_manual_backend_model_commons 2.4.1. ModelCommons
 *
 *		The \ref ModelCommons chain interface contains basic implementation functions shared by
 *	all model classes. Functions \ref ModelCommons::isAdoptable_chain and \ref ModelCommons::adopted_chain
 *	control adoption process. This is where a LiveNodeModel parent graph association is changed,
 *	and happens when the same backend model is moved to frontend graph until passed to user.
 *	Functions \ref ModelCommons::strategy and \ref ModelCommons::alterStrategy control the state
 *	and state changes of the model, such as \ref RDFStrategy::Running, \ref RDFStrategy::Live,
 *	and so on. Functions \ref ModelCommons::updateFrom and \ref ModelCommons::updateTargetModule
 *	control the updating/merging of compatible models. This is used on streaming and live modes
 *	bring in new updates to the model. As all these functions are chained functions, there are
 *	guidelines on how they should be implemented. See the documentation of each one for specifics.
 *
 *
 *	\section dev_manual_
 */

}
#endif /* SOPRANOLIVE_DEV_DOC_GLOBAL_H_ */
