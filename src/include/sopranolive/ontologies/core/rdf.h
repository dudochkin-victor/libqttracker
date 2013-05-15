/*
 * rdf.h, manually edited
 */

#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_FORWARD_DECLARATIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_FORWARD_DECLARATIONS

// Namespace objects and forward declarations section

#include "../../QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			/*   \defgroup onto_rdf_classes namespace rdf class definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_rdf_properties namespace rdf property definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_rdf_resources namespace rdf resource definitions
			 *   {
			 *   }
			 */
			static const char rdf_namespace_prefix[] = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
			struct Ontology_rdf
			{
				//   The defining context of this namespace
				typedef ::SopranoLive::Contexts::Core ResourceContext;
				static QUrl iri(QString suffix = QString())
				{
					return QLatin1String(rdf_namespace_prefix) + suffix;
				}
			};
			namespace rdf
			{
				inline QUrl iri(QString suffix) { return Ontology_rdf::iri(suffix); }

				/*
				 * \section classes Classes
				 */
				// {
				class Property;
				// }
			}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_FORWARD_DECLARATIONS */


// Property class definitions section

#if (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_PROPERTY_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_PROPERTY_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "nrl.h"
#include "xsd.h"
#include "rdfs.h"
#elif defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "nrl.h"
#include "xsd.h"
#include "rdfs.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#else
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "nrl.h"
#include "xsd.h"
#include "rdfs.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			namespace rdf
			{
				/*
				 * \section properties Property description classes
				 */
				// {
				/*   Property generated from rdf:type
				 * \anchor rdf_type
				 * \n rdfs:range=rdfs:Class
				 * \n tracker:modified=2249
				 * \n rdfs:domain=rdfs:Resource
				 * \n tracker:added=2010-04-08T11:21:26Z
				 */
				/// \ingroup onto_rdf_properties
				class type
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdf Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = 0 };
					/// The domain/owning class of this property
					typedef rdfs::Resource Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef rdfs::Resource RDFDomain;
					/// The localized c++ range type of this property
					typedef rdfs::Class Range;
					/// The full, non-localized rdf range type of this property
					typedef rdfs::Class RDFRange;
				};
				// }
				/*
				 * \section resources Resource description classes
				 */
				// {
				// }
			}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_PROPERTY_DEFINITIONS */
#endif /* (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */


#if ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) \
      && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \
      ) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_CLASS_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_CLASS_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "nrl.h"
#include "xsd.h"
#include "rdfs.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "rdfs.h"
#else // !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "nrl.h"
#include "xsd.h"
#include "rdfs.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "rdfs.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			namespace rdf
			{

				using namespace xsd;
				using namespace rdfs;

				/*   Class generated from rdf:Property
				 * \ingroup onto_rdf_classes
				 *
				 * rdfs:label = Property
				 *
				 * tracker:modified = 2060
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 *
				 * 	\b Properties \b having \b this \b class \b as \b Range \n
				 * 		\ref rdfs_subPropertyOf "rdfs:subPropertyOf"
				 */
				class Property
					: public virtual rdfs::Resource

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/1999/02/22-rdf-syntax-ns#Property"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<rdfs::Resource, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_rdf Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<Property> Live;
					Property() { this->attachInterfaceHelper< Property >(); }
					~Property() { this->detachInterfaceHelper< Property >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {

					/* Property generated from rdfs:subPropertyOf
					 * \n rdfs:range=rdf:Property
					 * \n tracker:modified=2251
					 * \n rdfs:domain=rdf:Property
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveRdfsSubPropertyOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveRdfsSubPropertyOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdfs::subPropertyOf >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getRdfsSubPropertyOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getRdfsSubPropertyOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdfs::subPropertyOf >(object_info, strategy, parent); }
					/*
						\fn void setRdfsSubPropertyOfs(LiveNodes const &rdfsSubPropertyOf_)


						Sets the property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set to the given \a rdfsSubPropertyOf_ set.
						\sa LiveResource::setObjects
					 */
					void setRdfsSubPropertyOfs(LiveNodes const &rdfsSubPropertyOf_) { setObjects(rdfs::subPropertyOf::iri(), rdfsSubPropertyOf_); }
					/*
						\fn void removeRdfsSubPropertyOfs(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeRdfsSubPropertyOfs(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdfs::subPropertyOf::iri(), object_info); }
					/*
						\fn ::SopranoLive::Live< Property > firstRdfsSubPropertyOf()


						Returns the first property \ref rdfs_subPropertyOf "rdfs:subPropertyOf". If the property didn't exist, a null LiveNode is returned.
						\sa LiveResource::firstObject
					 */
					::SopranoLive::Live< Property > firstRdfsSubPropertyOf() { return firstObject< rdfs::subPropertyOf >(); }
					/*
						\fn ::SopranoLive::Live< Property > addRdfsSubPropertyOf()


						Adds a new uniquely named resource to the property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Property > addRdfsSubPropertyOf() { return addObject< rdfs::subPropertyOf >(); }
					/*
						\fn ::SopranoLive::Live< Property > addRdfsSubPropertyOf(::SopranoLive::Live< Property > const &rdfsSubPropertyOf_)


						Adds the given \a rdfsSubPropertyOf_ to the property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Property > addRdfsSubPropertyOf(::SopranoLive::Live< Property > const &rdfsSubPropertyOf_) { return addObject(rdfs::subPropertyOf::iri(), rdfsSubPropertyOf_); }
					/*
						\fn void removeRdfsSubPropertyOf(::SopranoLive::Live< Property > const &rdfsSubPropertyOf_)


						Removes the given \a rdfsSubPropertyOf_ from the property \ref rdfs_subPropertyOf "rdfs:subPropertyOf" set.
						\sa LiveResource::removeObject
					 */
					void removeRdfsSubPropertyOf(::SopranoLive::Live< Property > const &rdfsSubPropertyOf_) { removeObject(rdfs::subPropertyOf::iri(), rdfsSubPropertyOf_); }

					/* Property generated from nrl:maxCardinality
					 * \n rdfs:range=xsd:integer
					 * \n tracker:modified=2265
					 * \n rdfs:domain=rdf:Property
					 * \n tracker:added=2010-04-08T11:21:26Z
					 * \n nrl:maxCardinality=1
					 */
					/*
						\fn LiveNodes liveNrlMaxCardinalitys(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref nrl_maxCardinality "nrl:maxCardinality" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveNrlMaxCardinalitys(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< nrl::maxCardinality >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getNrlMaxCardinalitys(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref nrl_maxCardinality "nrl:maxCardinality" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getNrlMaxCardinalitys(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< nrl::maxCardinality >(object_info, strategy, parent); }
					/*
						\fn void setNrlMaxCardinalitys(LiveNodes const &nrlMaxCardinality_)


						Sets the property \ref nrl_maxCardinality "nrl:maxCardinality" set to the given \a nrlMaxCardinality_ set.
						\sa LiveResource::setObjects
					 */
					void setNrlMaxCardinalitys(LiveNodes const &nrlMaxCardinality_) { setObjects(nrl::maxCardinality::iri(), nrlMaxCardinality_); }
					/*
						\fn void removeNrlMaxCardinalitys(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref nrl_maxCardinality "nrl:maxCardinality" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeNrlMaxCardinalitys(RDFVariable const &object_info = RDFVariable()) { removeObjects(nrl::maxCardinality::iri(), object_info); }
					/*
						\fn int getNrlMaxCardinality()


						Returns the property \ref nrl_maxCardinality "nrl:maxCardinality". If the property didn't exist, it is set to a default initialized value of type int and returned.
						\sa LiveResource::getObject
					 */
					int getNrlMaxCardinality() { return AutoGen::Value< int >::fromNode(getObject< nrl::maxCardinality >()); }
					/*
						\fn void setNrlMaxCardinality(int const &nrlMaxCardinality_)


						Sets the property \ref nrl_maxCardinality "nrl:maxCardinality" to the given \a nrlMaxCardinality.
						\sa LiveResource::setObject
					 */
					void setNrlMaxCardinality(int const &nrlMaxCardinality_) { setObject(nrl::maxCardinality::iri(), AutoGen::Value< int >::toNode(nrlMaxCardinality_ )); }
					/*
						\fn int firstNrlMaxCardinality()


						Returns the first property \ref nrl_maxCardinality "nrl:maxCardinality". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					int firstNrlMaxCardinality() { return AutoGen::Value< int >::fromNode(firstObject< nrl::maxCardinality >()); }
					/*
						\fn void addNrlMaxCardinality(int const &nrlMaxCardinality_)


						Adds the given \a nrlMaxCardinality_ to the property \ref nrl_maxCardinality "nrl:maxCardinality" set.
						\sa LiveResource::addObject
					 */
					void addNrlMaxCardinality(int const &nrlMaxCardinality_) { addObject(nrl::maxCardinality::iri(), AutoGen::Value< int >::toNode(nrlMaxCardinality_)); }
					/*
						\fn void removeNrlMaxCardinality(int const &nrlMaxCardinality_)


						Removes the given \a nrlMaxCardinality_ from the property \ref nrl_maxCardinality "nrl:maxCardinality" set.
						\sa LiveResource::removeObject
					 */
					void removeNrlMaxCardinality(int const &nrlMaxCardinality_) { removeObject(nrl::maxCardinality::iri(), AutoGen::Value< int >::toNode(nrlMaxCardinality_)); }

					/* Property generated from rdfs:range
					 * \n rdfs:range=rdfs:Class
					 * \n tracker:modified=2255
					 * \n rdfs:domain=rdf:Property
					 * \n tracker:added=2010-04-08T11:21:26Z
					 * \n nrl:maxCardinality=1
					 */
	// property api changing 			typedef rdfs::Class Class;
					/*
						\fn LiveNodes liveRdfsRanges(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdfs_range "rdfs:range" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveRdfsRanges(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdfs::range >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getRdfsRanges(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdfs_range "rdfs:range" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getRdfsRanges(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdfs::range >(object_info, strategy, parent); }
					/*
						\fn void setRdfsRanges(LiveNodes const &rdfsRange_)


						Sets the property \ref rdfs_range "rdfs:range" set to the given \a rdfsRange_ set.
						\sa LiveResource::setObjects
					 */
					void setRdfsRanges(LiveNodes const &rdfsRange_) { setObjects(rdfs::range::iri(), rdfsRange_); }
					/*
						\fn void removeRdfsRanges(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdfs_range "rdfs:range" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeRdfsRanges(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdfs::range::iri(), object_info); }
					/*
						\fn ::SopranoLive::Live< rdfs::Class > getRdfsRange()


						Returns the property \ref rdfs_range "rdfs:range". If the property didn't exist, it is set to a new unique resource and returned.
						\sa LiveResource::getObject
					 */
					::SopranoLive::Live< rdfs::Class > getRdfsRange() { return getObject< rdfs::range >(); }
					/*
						\fn void setRdfsRange(::SopranoLive::Live< rdfs::Class > const &rdfsRange_)


						Sets the property \ref rdfs_range "rdfs:range" to the given \a rdfsRange.
						\sa LiveResource::setObject
					 */
					void setRdfsRange(::SopranoLive::Live< rdfs::Class > const &rdfsRange_) { setObject(rdfs::range::iri(), rdfsRange_ ); }
					/*
						\fn ::SopranoLive::Live< rdfs::Class > firstRdfsRange()


						Returns the first property \ref rdfs_range "rdfs:range". If the property didn't exist, a null LiveNode is returned.
						\sa LiveResource::firstObject
					 */
					::SopranoLive::Live< rdfs::Class > firstRdfsRange() { return firstObject< rdfs::range >(); }
					/*
						\fn ::SopranoLive::Live< rdfs::Class > addRdfsRange()


						Adds a new uniquely named resource to the property \ref rdfs_range "rdfs:range" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< rdfs::Class > addRdfsRange() { return addObject< rdfs::range >(); }
					/*
						\fn ::SopranoLive::Live< rdfs::Class > addRdfsRange(::SopranoLive::Live< rdfs::Class > const &rdfsRange_)


						Adds the given \a rdfsRange_ to the property \ref rdfs_range "rdfs:range" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< rdfs::Class > addRdfsRange(::SopranoLive::Live< rdfs::Class > const &rdfsRange_) { return addObject(rdfs::range::iri(), rdfsRange_); }
					/*
						\fn void removeRdfsRange(::SopranoLive::Live< rdfs::Class > const &rdfsRange_)


						Removes the given \a rdfsRange_ from the property \ref rdfs_range "rdfs:range" set.
						\sa LiveResource::removeObject
					 */
					void removeRdfsRange(::SopranoLive::Live< rdfs::Class > const &rdfsRange_) { removeObject(rdfs::range::iri(), rdfsRange_); }

					/* Property generated from rdfs:domain
					 * \n rdfs:range=rdfs:Resource
					 * \n tracker:modified=2254
					 * \n rdfs:domain=rdf:Property
					 * \n tracker:added=2010-04-08T11:21:26Z
					 * \n nrl:maxCardinality=1
					 */
	// property api changing 			typedef rdfs::Resource Resource;
					/*
						\fn LiveNodes liveRdfsDomains(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdfs_domain "rdfs:domain" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveRdfsDomains(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdfs::domain >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getRdfsDomains(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdfs_domain "rdfs:domain" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getRdfsDomains(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdfs::domain >(object_info, strategy, parent); }
					/*
						\fn void setRdfsDomains(LiveNodes const &rdfsDomain_)


						Sets the property \ref rdfs_domain "rdfs:domain" set to the given \a rdfsDomain_ set.
						\sa LiveResource::setObjects
					 */
					void setRdfsDomains(LiveNodes const &rdfsDomain_) { setObjects(rdfs::domain::iri(), rdfsDomain_); }
					/*
						\fn void removeRdfsDomains(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdfs_domain "rdfs:domain" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeRdfsDomains(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdfs::domain::iri(), object_info); }
					/*
						\fn ::SopranoLive::Live< rdfs::Resource > getRdfsDomain()


						Returns the property \ref rdfs_domain "rdfs:domain". If the property didn't exist, it is set to a new unique resource and returned.
						\sa LiveResource::getObject
					 */
					::SopranoLive::Live< rdfs::Resource > getRdfsDomain() { return getObject< rdfs::domain >(); }
					/*
						\fn void setRdfsDomain(::SopranoLive::Live< rdfs::Resource > const &rdfsDomain_)


						Sets the property \ref rdfs_domain "rdfs:domain" to the given \a rdfsDomain.
						\sa LiveResource::setObject
					 */
					void setRdfsDomain(::SopranoLive::Live< rdfs::Resource > const &rdfsDomain_) { setObject(rdfs::domain::iri(), rdfsDomain_ ); }
					/*
						\fn ::SopranoLive::Live< rdfs::Resource > firstRdfsDomain()


						Returns the first property \ref rdfs_domain "rdfs:domain". If the property didn't exist, a null LiveNode is returned.
						\sa LiveResource::firstObject
					 */
					::SopranoLive::Live< rdfs::Resource > firstRdfsDomain() { return firstObject< rdfs::domain >(); }
					/*
						\fn ::SopranoLive::Live< rdfs::Resource > addRdfsDomain()


						Adds a new uniquely named resource to the property \ref rdfs_domain "rdfs:domain" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< rdfs::Resource > addRdfsDomain() { return addObject< rdfs::domain >(); }
					/*
						\fn ::SopranoLive::Live< rdfs::Resource > addRdfsDomain(::SopranoLive::Live< rdfs::Resource > const &rdfsDomain_)


						Adds the given \a rdfsDomain_ to the property \ref rdfs_domain "rdfs:domain" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< rdfs::Resource > addRdfsDomain(::SopranoLive::Live< rdfs::Resource > const &rdfsDomain_) { return addObject(rdfs::domain::iri(), rdfsDomain_); }
					/*
						\fn void removeRdfsDomain(::SopranoLive::Live< rdfs::Resource > const &rdfsDomain_)


						Removes the given \a rdfsDomain_ from the property \ref rdfs_domain "rdfs:domain" set.
						\sa LiveResource::removeObject
					 */
					void removeRdfsDomain(::SopranoLive::Live< rdfs::Resource > const &rdfsDomain_) { removeObject(rdfs::domain::iri(), rdfsDomain_); }
				};
			}
		}
	}
}
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::rdf::Property::Live)
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_CLASS_DEFINITIONS */
#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */


#if !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_POST_INCLUDES)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDF_H_POST_INCLUDES
#include "nrl.h"
#include "xsd.h"
#include "rdfs.h"
#endif
