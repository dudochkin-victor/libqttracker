/*
 * rdfs.h, manually edited
 */

#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_FORWARD_DECLARATIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_FORWARD_DECLARATIONS

// Namespace objects and forward declarations section

#include "../../QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			/*   \defgroup onto_rdfs_classes namespace rdfs class definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_rdfs_properties namespace rdfs property definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_rdfs_resources namespace rdfs resource definitions
			 *   {
			 *   }
			 */
			static const char rdfs_namespace_prefix[] = "http://www.w3.org/2000/01/rdf-schema#";
			struct Ontology_rdfs
			{
				//   The defining context of this namespace
				typedef ::SopranoLive::Contexts::Core ResourceContext;
				static QUrl iri(QString suffix = QString())
				{
					return QLatin1String(rdfs_namespace_prefix) + suffix;
				}
			};
			namespace rdfs
			{
				inline QUrl iri(QString suffix) { return Ontology_rdfs::iri(suffix); }

				/*
				 * \section classes Classes
				 */
				// {
				class Class;
				class Literal;
				class Resource;
				// }
			}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_FORWARD_DECLARATIONS */


// Property class definitions section

#if (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_PROPERTY_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_PROPERTY_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "dc.h"
#include "rdf.h"
#include "xsd.h"
#elif defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "dc.h"
#include "rdf.h"
#include "xsd.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#else
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "dc.h"
#include "rdf.h"
#include "xsd.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			namespace rdfs
			{
				/*
				 * \section properties Property description classes
				 */
				// {
				/*   Property generated from rdfs:subClassOf
				 * \anchor rdfs_subClassOf
				 * \n rdfs:range=rdfs:Class
				 * \n tracker:modified=2250
				 * \n rdfs:domain=rdfs:Class
				 * \n tracker:added=2010-04-08T11:21:26Z
				 */
				/// \ingroup onto_rdfs_properties
				class subClassOf
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#subClassOf"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdfs Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = 0 };
					/// The domain/owning class of this property
					typedef Class Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef Class RDFDomain;
					/// The localized c++ range type of this property
					typedef Class Range;
					/// The full, non-localized rdf range type of this property
					typedef Class RDFRange;
				};
				/*   Property generated from rdfs:comment
				 * \anchor rdfs_comment
				 * \n rdfs:range=xsd:string
				 * \n tracker:modified=2252
				 * \n rdfs:domain=rdfs:Resource
				 * \n tracker:added=2010-04-08T11:21:26Z
				 * \n nrl:maxCardinality=1
				 */
				/// \ingroup onto_rdfs_properties
				class comment
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#comment"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdfs Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
					/// The domain/owning class of this property
					typedef Resource Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef Resource RDFDomain;
					/// The localized c++ range type of this property
					typedef QString Range;
					/// The full, non-localized rdf range type of this property
					typedef ::SopranoLive::Ontologies::Core::xsd::string RDFRange;
				};
				/*   Property generated from rdfs:label
				 * \anchor rdfs_label
				 * \n rdfs:range=xsd:string
				 * \n tracker:modified=2253
				 * \n rdfs:domain=rdfs:Resource
				 * \n tracker:added=2010-04-08T11:21:26Z
				 * \n nrl:maxCardinality=1
				 */
				/// \ingroup onto_rdfs_properties
				class label
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#label"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdfs Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
					/// The domain/owning class of this property
					typedef Resource Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef Resource RDFDomain;
					/// The localized c++ range type of this property
					typedef QString Range;
					/// The full, non-localized rdf range type of this property
					typedef ::SopranoLive::Ontologies::Core::xsd::string RDFRange;
				};
				/*   Property generated from rdfs:domain
				 * \anchor rdfs_domain
				 * \n rdfs:range=rdfs:Resource
				 * \n tracker:modified=2254
				 * \n rdfs:domain=rdf:Property
				 * \n tracker:added=2010-04-08T11:21:26Z
				 * \n nrl:maxCardinality=1
				 */
				/// \ingroup onto_rdfs_properties
				class domain
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#domain"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdfs Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued };
					/// The domain/owning class of this property
					typedef rdf::Property Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef rdf::Property RDFDomain;
					/// The localized c++ range type of this property
					typedef Resource Range;
					/// The full, non-localized rdf range type of this property
					typedef Resource RDFRange;
				};
				/*   Property generated from rdfs:range
				 * \anchor rdfs_range
				 * \n rdfs:range=rdfs:Class
				 * \n tracker:modified=2255
				 * \n rdfs:domain=rdf:Property
				 * \n tracker:added=2010-04-08T11:21:26Z
				 * \n nrl:maxCardinality=1
				 */
				/// \ingroup onto_rdfs_properties
				class range
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#range"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdfs Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued };
					/// The domain/owning class of this property
					typedef rdf::Property Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef rdf::Property RDFDomain;
					/// The localized c++ range type of this property
					typedef Class Range;
					/// The full, non-localized rdf range type of this property
					typedef Class RDFRange;
				};
				/*   Property generated from rdfs:subPropertyOf
				 * \anchor rdfs_subPropertyOf
				 * \n rdfs:range=rdf:Property
				 * \n tracker:modified=2251
				 * \n rdfs:domain=rdf:Property
				 * \n tracker:added=2010-04-08T11:21:26Z
				 */
				/// \ingroup onto_rdfs_properties
				class subPropertyOf
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#subPropertyOf"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_rdfs Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = 0 };
					/// The domain/owning class of this property
					typedef rdf::Property Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef rdf::Property RDFDomain;
					/// The localized c++ range type of this property
					typedef rdf::Property Range;
					/// The full, non-localized rdf range type of this property
					typedef rdf::Property RDFRange;
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
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_PROPERTY_DEFINITIONS */
#endif /* (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */


#if ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) \
      && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \
      ) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_CLASS_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_CLASS_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "dc.h"
#include "rdf.h"
#include "xsd.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#else // !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "dc.h"
#include "rdf.h"
#include "xsd.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			namespace rdfs
			{

				using namespace rdf;
				using namespace xsd;

				/*   Class generated from rdfs:Resource
				 * \ingroup onto_rdfs_classes
				 * All resources
				 *
				 * rdfs:label = All Resources
				 *
				 * tracker:modified = 2058
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 *
				 * 	\b Properties \b having \b this \b class \b as \b Range \n
				 * 		\ref dc_source "dc:source"
				 * 		\ref rdfs_domain "rdfs:domain"
				 * 		\ref nao_isRelated "nao:isRelated"
				 * 		\ref nao_hasTag "nao:hasTag"
				 */
				class Resource
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_rdfs Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<Resource> Live;
					Resource() { this->attachInterfaceHelper< Resource >(); }
					~Resource() { this->detachInterfaceHelper< Resource >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {

					/* Property generated from dc:rights
					 * \n Information about rights held in and over the resource.
					 * \n rdfs:label=Rights
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:modified=2280
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcRightss(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_rights "dc:rights" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcRightss(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::rights >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcRightss(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_rights "dc:rights" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcRightss(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::rights >(object_info, strategy, parent); }
					/*
						\fn void setDcRightss(LiveNodes const &dcRights_)


						Sets the property \ref dc_rights "dc:rights" set to the given \a dcRights_ set.
						\sa LiveResource::setObjects
					 */
					void setDcRightss(LiveNodes const &dcRights_) { setObjects(dc::rights::iri(), dcRights_); }
					/*
						\fn void removeDcRightss(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_rights "dc:rights" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcRightss(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::rights::iri(), object_info); }
					/*
						\fn QString getDcRights()


						Returns the property \ref dc_rights "dc:rights". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcRights() { return AutoGen::Value< QString >::fromNode(getObject< dc::rights >()); }
					/*
						\fn void setDcRights(QString const &dcRights_)


						Sets the property \ref dc_rights "dc:rights" to the given \a dcRights.
						\sa LiveResource::setObject
					 */
					void setDcRights(QString const &dcRights_) { setObject(dc::rights::iri(), AutoGen::Value< QString >::toNode(dcRights_ )); }
					/*
						\fn QString firstDcRights()


						Returns the first property \ref dc_rights "dc:rights". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcRights() { return AutoGen::Value< QString >::fromNode(firstObject< dc::rights >()); }
					/*
						\fn void addDcRights(QString const &dcRights_)


						Adds the given \a dcRights_ to the property \ref dc_rights "dc:rights" set.
						\sa LiveResource::addObject
					 */
					void addDcRights(QString const &dcRights_) { addObject(dc::rights::iri(), AutoGen::Value< QString >::toNode(dcRights_)); }
					/*
						\fn void removeDcRights(QString const &dcRights_)


						Removes the given \a dcRights_ from the property \ref dc_rights "dc:rights" set.
						\sa LiveResource::removeObject
					 */
					void removeDcRights(QString const &dcRights_) { removeObject(dc::rights::iri(), AutoGen::Value< QString >::toNode(dcRights_)); }

					/* Property generated from dc:contributor
					 * \n An entity responsible for making contributions to the resource.
					 * \n rdfs:label=Contributor
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:weight=4
					 * \n tracker:modified=2271
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcContributors(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_contributor "dc:contributor" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcContributors(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::contributor >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcContributors(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_contributor "dc:contributor" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcContributors(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::contributor >(object_info, strategy, parent); }
					/*
						\fn void setDcContributors(LiveNodes const &dcContributor_)


						Sets the property \ref dc_contributor "dc:contributor" set to the given \a dcContributor_ set.
						\sa LiveResource::setObjects
					 */
					void setDcContributors(LiveNodes const &dcContributor_) { setObjects(dc::contributor::iri(), dcContributor_); }
					/*
						\fn void removeDcContributors(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_contributor "dc:contributor" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcContributors(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::contributor::iri(), object_info); }
					/*
						\fn QString firstDcContributor()


						Returns the first property \ref dc_contributor "dc:contributor". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcContributor() { return AutoGen::Value< QString >::fromNode(firstObject< dc::contributor >()); }
					/*
						\fn void addDcContributor(QString const &dcContributor_)


						Adds the given \a dcContributor_ to the property \ref dc_contributor "dc:contributor" set.
						\sa LiveResource::addObject
					 */
					void addDcContributor(QString const &dcContributor_) { addObject(dc::contributor::iri(), AutoGen::Value< QString >::toNode(dcContributor_)); }
					/*
						\fn void removeDcContributor(QString const &dcContributor_)


						Removes the given \a dcContributor_ from the property \ref dc_contributor "dc:contributor" set.
						\sa LiveResource::removeObject
					 */
					void removeDcContributor(QString const &dcContributor_) { removeObject(dc::contributor::iri(), AutoGen::Value< QString >::toNode(dcContributor_)); }

					/* Property generated from dc:format
					 * \n The file format, physical medium, or dimensions of the resource.
					 * \n rdfs:label=Format
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:modified=2274
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcFormats(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_format "dc:format" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcFormats(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::format >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcFormats(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_format "dc:format" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcFormats(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::format >(object_info, strategy, parent); }
					/*
						\fn void setDcFormats(LiveNodes const &dcFormat_)


						Sets the property \ref dc_format "dc:format" set to the given \a dcFormat_ set.
						\sa LiveResource::setObjects
					 */
					void setDcFormats(LiveNodes const &dcFormat_) { setObjects(dc::format::iri(), dcFormat_); }
					/*
						\fn void removeDcFormats(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_format "dc:format" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcFormats(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::format::iri(), object_info); }
					/*
						\fn QString getDcFormat()


						Returns the property \ref dc_format "dc:format". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcFormat() { return AutoGen::Value< QString >::fromNode(getObject< dc::format >()); }
					/*
						\fn void setDcFormat(QString const &dcFormat_)


						Sets the property \ref dc_format "dc:format" to the given \a dcFormat.
						\sa LiveResource::setObject
					 */
					void setDcFormat(QString const &dcFormat_) { setObject(dc::format::iri(), AutoGen::Value< QString >::toNode(dcFormat_ )); }
					/*
						\fn QString firstDcFormat()


						Returns the first property \ref dc_format "dc:format". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcFormat() { return AutoGen::Value< QString >::fromNode(firstObject< dc::format >()); }
					/*
						\fn void addDcFormat(QString const &dcFormat_)


						Adds the given \a dcFormat_ to the property \ref dc_format "dc:format" set.
						\sa LiveResource::addObject
					 */
					void addDcFormat(QString const &dcFormat_) { addObject(dc::format::iri(), AutoGen::Value< QString >::toNode(dcFormat_)); }
					/*
						\fn void removeDcFormat(QString const &dcFormat_)


						Removes the given \a dcFormat_ from the property \ref dc_format "dc:format" set.
						\sa LiveResource::removeObject
					 */
					void removeDcFormat(QString const &dcFormat_) { removeObject(dc::format::iri(), AutoGen::Value< QString >::toNode(dcFormat_)); }

					/* Property generated from dc:publisher
					 * \n An entity responsible for making the resource available.
					 * \n rdfs:label=Publisher
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:weight=4
					 * \n tracker:modified=2270
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcPublishers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_publisher "dc:publisher" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcPublishers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::publisher >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcPublishers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_publisher "dc:publisher" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcPublishers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::publisher >(object_info, strategy, parent); }
					/*
						\fn void setDcPublishers(LiveNodes const &dcPublisher_)


						Sets the property \ref dc_publisher "dc:publisher" set to the given \a dcPublisher_ set.
						\sa LiveResource::setObjects
					 */
					void setDcPublishers(LiveNodes const &dcPublisher_) { setObjects(dc::publisher::iri(), dcPublisher_); }
					/*
						\fn void removeDcPublishers(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_publisher "dc:publisher" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcPublishers(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::publisher::iri(), object_info); }
					/*
						\fn QString getDcPublisher()


						Returns the property \ref dc_publisher "dc:publisher". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcPublisher() { return AutoGen::Value< QString >::fromNode(getObject< dc::publisher >()); }
					/*
						\fn void setDcPublisher(QString const &dcPublisher_)


						Sets the property \ref dc_publisher "dc:publisher" to the given \a dcPublisher.
						\sa LiveResource::setObject
					 */
					void setDcPublisher(QString const &dcPublisher_) { setObject(dc::publisher::iri(), AutoGen::Value< QString >::toNode(dcPublisher_ )); }
					/*
						\fn QString firstDcPublisher()


						Returns the first property \ref dc_publisher "dc:publisher". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcPublisher() { return AutoGen::Value< QString >::fromNode(firstObject< dc::publisher >()); }
					/*
						\fn void addDcPublisher(QString const &dcPublisher_)


						Adds the given \a dcPublisher_ to the property \ref dc_publisher "dc:publisher" set.
						\sa LiveResource::addObject
					 */
					void addDcPublisher(QString const &dcPublisher_) { addObject(dc::publisher::iri(), AutoGen::Value< QString >::toNode(dcPublisher_)); }
					/*
						\fn void removeDcPublisher(QString const &dcPublisher_)


						Removes the given \a dcPublisher_ from the property \ref dc_publisher "dc:publisher" set.
						\sa LiveResource::removeObject
					 */
					void removeDcPublisher(QString const &dcPublisher_) { removeObject(dc::publisher::iri(), AutoGen::Value< QString >::toNode(dcPublisher_)); }

					/* Property generated from rdf:type
					 * \n rdfs:range=rdfs:Class
					 * \n tracker:modified=2249
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveRdfTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdf_type "rdf:type" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveRdfTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdf::type >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getRdfTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdf_type "rdf:type" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getRdfTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdf::type >(object_info, strategy, parent); }
					/*
						\fn void setRdfTypes(LiveNodes const &rdfType_)


						Sets the property \ref rdf_type "rdf:type" set to the given \a rdfType_ set.
						\sa LiveResource::setObjects
					 */
					void setRdfTypes(LiveNodes const &rdfType_) { setObjects(rdf::type::iri(), rdfType_); }
					/*
						\fn void removeRdfTypes(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdf_type "rdf:type" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeRdfTypes(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdf::type::iri(), object_info); }
					/*
						\fn ::SopranoLive::Live< Class > firstRdfType()


						Returns the first property \ref rdf_type "rdf:type". If the property didn't exist, a null LiveNode is returned.
						\sa LiveResource::firstObject
					 */
					::SopranoLive::Live< Class > firstRdfType() { return firstObject< rdf::type >(); }
					/*
						\fn ::SopranoLive::Live< Class > addRdfType()


						Adds a new uniquely named resource to the property \ref rdf_type "rdf:type" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Class > addRdfType() { return addObject< rdf::type >(); }
					/*
						\fn ::SopranoLive::Live< Class > addRdfType(::SopranoLive::Live< Class > const &rdfType_)


						Adds the given \a rdfType_ to the property \ref rdf_type "rdf:type" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Class > addRdfType(::SopranoLive::Live< Class > const &rdfType_) { return addObject(rdf::type::iri(), rdfType_); }
					/*
						\fn void removeRdfType(::SopranoLive::Live< Class > const &rdfType_)


						Removes the given \a rdfType_ from the property \ref rdf_type "rdf:type" set.
						\sa LiveResource::removeObject
					 */
					void removeRdfType(::SopranoLive::Live< Class > const &rdfType_) { removeObject(rdf::type::iri(), rdfType_); }

					/* Property generated from dc:title
					 * \n A name given to the resource.
					 * \n rdfs:label=Title
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:weight=10
					 * \n tracker:modified=2266
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcTitles(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_title "dc:title" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcTitles(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::title >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcTitles(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_title "dc:title" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcTitles(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::title >(object_info, strategy, parent); }
					/*
						\fn void setDcTitles(LiveNodes const &dcTitle_)


						Sets the property \ref dc_title "dc:title" set to the given \a dcTitle_ set.
						\sa LiveResource::setObjects
					 */
					void setDcTitles(LiveNodes const &dcTitle_) { setObjects(dc::title::iri(), dcTitle_); }
					/*
						\fn void removeDcTitles(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_title "dc:title" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcTitles(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::title::iri(), object_info); }
					/*
						\fn QString getDcTitle()


						Returns the property \ref dc_title "dc:title". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcTitle() { return AutoGen::Value< QString >::fromNode(getObject< dc::title >()); }
					/*
						\fn void setDcTitle(QString const &dcTitle_)


						Sets the property \ref dc_title "dc:title" to the given \a dcTitle.
						\sa LiveResource::setObject
					 */
					void setDcTitle(QString const &dcTitle_) { setObject(dc::title::iri(), AutoGen::Value< QString >::toNode(dcTitle_ )); }
					/*
						\fn QString firstDcTitle()


						Returns the first property \ref dc_title "dc:title". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcTitle() { return AutoGen::Value< QString >::fromNode(firstObject< dc::title >()); }
					/*
						\fn void addDcTitle(QString const &dcTitle_)


						Adds the given \a dcTitle_ to the property \ref dc_title "dc:title" set.
						\sa LiveResource::addObject
					 */
					void addDcTitle(QString const &dcTitle_) { addObject(dc::title::iri(), AutoGen::Value< QString >::toNode(dcTitle_)); }
					/*
						\fn void removeDcTitle(QString const &dcTitle_)


						Removes the given \a dcTitle_ from the property \ref dc_title "dc:title" set.
						\sa LiveResource::removeObject
					 */
					void removeDcTitle(QString const &dcTitle_) { removeObject(dc::title::iri(), AutoGen::Value< QString >::toNode(dcTitle_)); }

					/* Property generated from dc:source
					 * \n A related resource from which the described resource is derived.
					 * \n rdfs:range=rdfs:Resource
					 * \n rdfs:label=Source
					 * \n tracker:modified=2276
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcSources(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_source "dc:source" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcSources(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::source >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcSources(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_source "dc:source" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcSources(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::source >(object_info, strategy, parent); }
					/*
						\fn void setDcSources(LiveNodes const &dcSource_)


						Sets the property \ref dc_source "dc:source" set to the given \a dcSource_ set.
						\sa LiveResource::setObjects
					 */
					void setDcSources(LiveNodes const &dcSource_) { setObjects(dc::source::iri(), dcSource_); }
					/*
						\fn void removeDcSources(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_source "dc:source" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcSources(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::source::iri(), object_info); }
					/*
						\fn ::SopranoLive::Live< Resource > firstDcSource()


						Returns the first property \ref dc_source "dc:source". If the property didn't exist, a null LiveNode is returned.
						\sa LiveResource::firstObject
					 */
					::SopranoLive::Live< Resource > firstDcSource() { return firstObject< dc::source >(); }
					/*
						\fn ::SopranoLive::Live< Resource > addDcSource()


						Adds a new uniquely named resource to the property \ref dc_source "dc:source" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Resource > addDcSource() { return addObject< dc::source >(); }
					/*
						\fn ::SopranoLive::Live< Resource > addDcSource(::SopranoLive::Live< Resource > const &dcSource_)


						Adds the given \a dcSource_ to the property \ref dc_source "dc:source" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Resource > addDcSource(::SopranoLive::Live< Resource > const &dcSource_) { return addObject(dc::source::iri(), dcSource_); }
					/*
						\fn void removeDcSource(::SopranoLive::Live< Resource > const &dcSource_)


						Removes the given \a dcSource_ from the property \ref dc_source "dc:source" set.
						\sa LiveResource::removeObject
					 */
					void removeDcSource(::SopranoLive::Live< Resource > const &dcSource_) { removeObject(dc::source::iri(), dcSource_); }

					/* Property generated from dc:date
					 * \n A point or period of time associated with an event in the lifecycle of the resource.
					 * \n rdfs:range=xsd:dateTime
					 * \n rdfs:label=Date
					 * \n tracker:modified=2272
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcDates(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_date "dc:date" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcDates(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::date >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcDates(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_date "dc:date" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcDates(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::date >(object_info, strategy, parent); }
					/*
						\fn void setDcDates(LiveNodes const &dcDate_)


						Sets the property \ref dc_date "dc:date" set to the given \a dcDate_ set.
						\sa LiveResource::setObjects
					 */
					void setDcDates(LiveNodes const &dcDate_) { setObjects(dc::date::iri(), dcDate_); }
					/*
						\fn void removeDcDates(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_date "dc:date" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcDates(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::date::iri(), object_info); }
					/*
						\fn QDateTime firstDcDate()


						Returns the first property \ref dc_date "dc:date". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QDateTime firstDcDate() { return AutoGen::Value< QDateTime >::fromNode(firstObject< dc::date >()); }
					/*
						\fn void addDcDate(QDateTime const &dcDate_)


						Adds the given \a dcDate_ to the property \ref dc_date "dc:date" set.
						\sa LiveResource::addObject
					 */
					void addDcDate(QDateTime const &dcDate_) { addObject(dc::date::iri(), AutoGen::Value< QDateTime >::toNode(dcDate_)); }
					/*
						\fn void removeDcDate(QDateTime const &dcDate_)


						Removes the given \a dcDate_ from the property \ref dc_date "dc:date" set.
						\sa LiveResource::removeObject
					 */
					void removeDcDate(QDateTime const &dcDate_) { removeObject(dc::date::iri(), AutoGen::Value< QDateTime >::toNode(dcDate_)); }

					/* Property generated from dc:identifier
					 * \n An unambiguous reference to the resource within a given context.
					 * \n rdfs:label=Identifier
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:modified=2275
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcIdentifiers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_identifier "dc:identifier" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcIdentifiers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::identifier >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcIdentifiers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_identifier "dc:identifier" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcIdentifiers(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::identifier >(object_info, strategy, parent); }
					/*
						\fn void setDcIdentifiers(LiveNodes const &dcIdentifier_)


						Sets the property \ref dc_identifier "dc:identifier" set to the given \a dcIdentifier_ set.
						\sa LiveResource::setObjects
					 */
					void setDcIdentifiers(LiveNodes const &dcIdentifier_) { setObjects(dc::identifier::iri(), dcIdentifier_); }
					/*
						\fn void removeDcIdentifiers(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_identifier "dc:identifier" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcIdentifiers(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::identifier::iri(), object_info); }
					/*
						\fn QString getDcIdentifier()


						Returns the property \ref dc_identifier "dc:identifier". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcIdentifier() { return AutoGen::Value< QString >::fromNode(getObject< dc::identifier >()); }
					/*
						\fn void setDcIdentifier(QString const &dcIdentifier_)


						Sets the property \ref dc_identifier "dc:identifier" to the given \a dcIdentifier.
						\sa LiveResource::setObject
					 */
					void setDcIdentifier(QString const &dcIdentifier_) { setObject(dc::identifier::iri(), AutoGen::Value< QString >::toNode(dcIdentifier_ )); }
					/*
						\fn QString firstDcIdentifier()


						Returns the first property \ref dc_identifier "dc:identifier". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcIdentifier() { return AutoGen::Value< QString >::fromNode(firstObject< dc::identifier >()); }
					/*
						\fn void addDcIdentifier(QString const &dcIdentifier_)


						Adds the given \a dcIdentifier_ to the property \ref dc_identifier "dc:identifier" set.
						\sa LiveResource::addObject
					 */
					void addDcIdentifier(QString const &dcIdentifier_) { addObject(dc::identifier::iri(), AutoGen::Value< QString >::toNode(dcIdentifier_)); }
					/*
						\fn void removeDcIdentifier(QString const &dcIdentifier_)


						Removes the given \a dcIdentifier_ from the property \ref dc_identifier "dc:identifier" set.
						\sa LiveResource::removeObject
					 */
					void removeDcIdentifier(QString const &dcIdentifier_) { removeObject(dc::identifier::iri(), AutoGen::Value< QString >::toNode(dcIdentifier_)); }

					/* Property generated from dc:description
					 * \n An account of the resource.
					 * \n rdfs:label=Description
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:weight=3
					 * \n tracker:modified=2269
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcDescriptions(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_description "dc:description" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcDescriptions(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::description >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcDescriptions(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_description "dc:description" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcDescriptions(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::description >(object_info, strategy, parent); }
					/*
						\fn void setDcDescriptions(LiveNodes const &dcDescription_)


						Sets the property \ref dc_description "dc:description" set to the given \a dcDescription_ set.
						\sa LiveResource::setObjects
					 */
					void setDcDescriptions(LiveNodes const &dcDescription_) { setObjects(dc::description::iri(), dcDescription_); }
					/*
						\fn void removeDcDescriptions(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_description "dc:description" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcDescriptions(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::description::iri(), object_info); }
					/*
						\fn QString getDcDescription()


						Returns the property \ref dc_description "dc:description". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcDescription() { return AutoGen::Value< QString >::fromNode(getObject< dc::description >()); }
					/*
						\fn void setDcDescription(QString const &dcDescription_)


						Sets the property \ref dc_description "dc:description" to the given \a dcDescription.
						\sa LiveResource::setObject
					 */
					void setDcDescription(QString const &dcDescription_) { setObject(dc::description::iri(), AutoGen::Value< QString >::toNode(dcDescription_ )); }
					/*
						\fn QString firstDcDescription()


						Returns the first property \ref dc_description "dc:description". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcDescription() { return AutoGen::Value< QString >::fromNode(firstObject< dc::description >()); }
					/*
						\fn void addDcDescription(QString const &dcDescription_)


						Adds the given \a dcDescription_ to the property \ref dc_description "dc:description" set.
						\sa LiveResource::addObject
					 */
					void addDcDescription(QString const &dcDescription_) { addObject(dc::description::iri(), AutoGen::Value< QString >::toNode(dcDescription_)); }
					/*
						\fn void removeDcDescription(QString const &dcDescription_)


						Removes the given \a dcDescription_ from the property \ref dc_description "dc:description" set.
						\sa LiveResource::removeObject
					 */
					void removeDcDescription(QString const &dcDescription_) { removeObject(dc::description::iri(), AutoGen::Value< QString >::toNode(dcDescription_)); }

					/* Property generated from dc:relation
					 * \n A related resource.
					 * \n rdfs:range=xsd:string
					 * \n rdfs:label=Relation
					 * \n tracker:modified=2278
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcRelations(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_relation "dc:relation" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcRelations(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::relation >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcRelations(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_relation "dc:relation" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcRelations(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::relation >(object_info, strategy, parent); }
					/*
						\fn void setDcRelations(LiveNodes const &dcRelation_)


						Sets the property \ref dc_relation "dc:relation" set to the given \a dcRelation_ set.
						\sa LiveResource::setObjects
					 */
					void setDcRelations(LiveNodes const &dcRelation_) { setObjects(dc::relation::iri(), dcRelation_); }
					/*
						\fn void removeDcRelations(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_relation "dc:relation" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcRelations(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::relation::iri(), object_info); }
					/*
						\fn QString firstDcRelation()


						Returns the first property \ref dc_relation "dc:relation". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcRelation() { return AutoGen::Value< QString >::fromNode(firstObject< dc::relation >()); }
					/*
						\fn void addDcRelation(QString const &dcRelation_)


						Adds the given \a dcRelation_ to the property \ref dc_relation "dc:relation" set.
						\sa LiveResource::addObject
					 */
					void addDcRelation(QString const &dcRelation_) { addObject(dc::relation::iri(), AutoGen::Value< QString >::toNode(dcRelation_)); }
					/*
						\fn void removeDcRelation(QString const &dcRelation_)


						Removes the given \a dcRelation_ from the property \ref dc_relation "dc:relation" set.
						\sa LiveResource::removeObject
					 */
					void removeDcRelation(QString const &dcRelation_) { removeObject(dc::relation::iri(), AutoGen::Value< QString >::toNode(dcRelation_)); }

					/* Property generated from rdfs:label
					 * \n rdfs:range=xsd:string
					 * \n tracker:modified=2253
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:added=2010-04-08T11:21:26Z
					 * \n nrl:maxCardinality=1
					 */
					/*
						\fn LiveNodes liveLabels(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdfs_label "rdfs:label" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveLabels(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdfs::label >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getLabels(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdfs_label "rdfs:label" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getLabels(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdfs::label >(object_info, strategy, parent); }
					/*
						\fn void setLabels(LiveNodes const &label_)


						Sets the property \ref rdfs_label "rdfs:label" set to the given \a label_ set.
						\sa LiveResource::setObjects
					 */
					void setLabels(LiveNodes const &label_) { setObjects(rdfs::label::iri(), label_); }
					/*
						\fn void removeLabels(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdfs_label "rdfs:label" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeLabels(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdfs::label::iri(), object_info); }
					/*
						\fn QString getLabel()


						Returns the property \ref rdfs_label "rdfs:label". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getLabel() { return AutoGen::Value< QString >::fromNode(getObject< rdfs::label >()); }
					/*
						\fn void setLabel(QString const &label_)


						Sets the property \ref rdfs_label "rdfs:label" to the given \a label.
						\sa LiveResource::setObject
					 */
					void setLabel(QString const &label_) { setObject(rdfs::label::iri(), AutoGen::Value< QString >::toNode(label_ )); }
					/*
						\fn QString firstLabel()


						Returns the first property \ref rdfs_label "rdfs:label". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstLabel() { return AutoGen::Value< QString >::fromNode(firstObject< rdfs::label >()); }
					/*
						\fn void addLabel(QString const &label_)


						Adds the given \a label_ to the property \ref rdfs_label "rdfs:label" set.
						\sa LiveResource::addObject
					 */
					void addLabel(QString const &label_) { addObject(rdfs::label::iri(), AutoGen::Value< QString >::toNode(label_)); }
					/*
						\fn void removeLabel(QString const &label_)


						Removes the given \a label_ from the property \ref rdfs_label "rdfs:label" set.
						\sa LiveResource::removeObject
					 */
					void removeLabel(QString const &label_) { removeObject(rdfs::label::iri(), AutoGen::Value< QString >::toNode(label_)); }

					/* Property generated from dc:creator
					 * \n An entity primarily responsible for making the resource.
					 * \n rdfs:label=Creator
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:weight=4
					 * \n tracker:modified=2267
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcCreators(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_creator "dc:creator" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcCreators(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::creator >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcCreators(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_creator "dc:creator" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcCreators(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::creator >(object_info, strategy, parent); }
					/*
						\fn void setDcCreators(LiveNodes const &dcCreator_)


						Sets the property \ref dc_creator "dc:creator" set to the given \a dcCreator_ set.
						\sa LiveResource::setObjects
					 */
					void setDcCreators(LiveNodes const &dcCreator_) { setObjects(dc::creator::iri(), dcCreator_); }
					/*
						\fn void removeDcCreators(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_creator "dc:creator" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcCreators(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::creator::iri(), object_info); }
					/*
						\fn QString getDcCreator()


						Returns the property \ref dc_creator "dc:creator". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcCreator() { return AutoGen::Value< QString >::fromNode(getObject< dc::creator >()); }
					/*
						\fn void setDcCreator(QString const &dcCreator_)


						Sets the property \ref dc_creator "dc:creator" to the given \a dcCreator.
						\sa LiveResource::setObject
					 */
					void setDcCreator(QString const &dcCreator_) { setObject(dc::creator::iri(), AutoGen::Value< QString >::toNode(dcCreator_ )); }
					/*
						\fn QString firstDcCreator()


						Returns the first property \ref dc_creator "dc:creator". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcCreator() { return AutoGen::Value< QString >::fromNode(firstObject< dc::creator >()); }
					/*
						\fn void addDcCreator(QString const &dcCreator_)


						Adds the given \a dcCreator_ to the property \ref dc_creator "dc:creator" set.
						\sa LiveResource::addObject
					 */
					void addDcCreator(QString const &dcCreator_) { addObject(dc::creator::iri(), AutoGen::Value< QString >::toNode(dcCreator_)); }
					/*
						\fn void removeDcCreator(QString const &dcCreator_)


						Removes the given \a dcCreator_ from the property \ref dc_creator "dc:creator" set.
						\sa LiveResource::removeObject
					 */
					void removeDcCreator(QString const &dcCreator_) { removeObject(dc::creator::iri(), AutoGen::Value< QString >::toNode(dcCreator_)); }

					/* Property generated from rdfs:comment
					 * \n rdfs:range=xsd:string
					 * \n tracker:modified=2252
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:added=2010-04-08T11:21:26Z
					 * \n nrl:maxCardinality=1
					 */
					/*
						\fn LiveNodes liveComments(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdfs_comment "rdfs:comment" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveComments(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdfs::comment >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getComments(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdfs_comment "rdfs:comment" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getComments(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdfs::comment >(object_info, strategy, parent); }
					/*
						\fn void setComments(LiveNodes const &comment_)


						Sets the property \ref rdfs_comment "rdfs:comment" set to the given \a comment_ set.
						\sa LiveResource::setObjects
					 */
					void setComments(LiveNodes const &comment_) { setObjects(rdfs::comment::iri(), comment_); }
					/*
						\fn void removeComments(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdfs_comment "rdfs:comment" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeComments(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdfs::comment::iri(), object_info); }
					/*
						\fn QString getComment()


						Returns the property \ref rdfs_comment "rdfs:comment". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getComment() { return AutoGen::Value< QString >::fromNode(getObject< rdfs::comment >()); }
					/*
						\fn void setComment(QString const &comment_)


						Sets the property \ref rdfs_comment "rdfs:comment" to the given \a comment.
						\sa LiveResource::setObject
					 */
					void setComment(QString const &comment_) { setObject(rdfs::comment::iri(), AutoGen::Value< QString >::toNode(comment_ )); }
					/*
						\fn QString firstComment()


						Returns the first property \ref rdfs_comment "rdfs:comment". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstComment() { return AutoGen::Value< QString >::fromNode(firstObject< rdfs::comment >()); }
					/*
						\fn void addComment(QString const &comment_)


						Adds the given \a comment_ to the property \ref rdfs_comment "rdfs:comment" set.
						\sa LiveResource::addObject
					 */
					void addComment(QString const &comment_) { addObject(rdfs::comment::iri(), AutoGen::Value< QString >::toNode(comment_)); }
					/*
						\fn void removeComment(QString const &comment_)


						Removes the given \a comment_ from the property \ref rdfs_comment "rdfs:comment" set.
						\sa LiveResource::removeObject
					 */
					void removeComment(QString const &comment_) { removeObject(rdfs::comment::iri(), AutoGen::Value< QString >::toNode(comment_)); }

					/* Property generated from dc:type
					 * \n The nature or genre of the resource.
					 * \n rdfs:label=Type
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:modified=2273
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_type "dc:type" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::type >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_type "dc:type" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcTypes(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::type >(object_info, strategy, parent); }
					/*
						\fn void setDcTypes(LiveNodes const &dcType_)


						Sets the property \ref dc_type "dc:type" set to the given \a dcType_ set.
						\sa LiveResource::setObjects
					 */
					void setDcTypes(LiveNodes const &dcType_) { setObjects(dc::type::iri(), dcType_); }
					/*
						\fn void removeDcTypes(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_type "dc:type" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcTypes(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::type::iri(), object_info); }
					/*
						\fn QString getDcType()


						Returns the property \ref dc_type "dc:type". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcType() { return AutoGen::Value< QString >::fromNode(getObject< dc::type >()); }
					/*
						\fn void setDcType(QString const &dcType_)


						Sets the property \ref dc_type "dc:type" to the given \a dcType.
						\sa LiveResource::setObject
					 */
					void setDcType(QString const &dcType_) { setObject(dc::type::iri(), AutoGen::Value< QString >::toNode(dcType_ )); }
					/*
						\fn QString firstDcType()


						Returns the first property \ref dc_type "dc:type". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcType() { return AutoGen::Value< QString >::fromNode(firstObject< dc::type >()); }
					/*
						\fn void addDcType(QString const &dcType_)


						Adds the given \a dcType_ to the property \ref dc_type "dc:type" set.
						\sa LiveResource::addObject
					 */
					void addDcType(QString const &dcType_) { addObject(dc::type::iri(), AutoGen::Value< QString >::toNode(dcType_)); }
					/*
						\fn void removeDcType(QString const &dcType_)


						Removes the given \a dcType_ from the property \ref dc_type "dc:type" set.
						\sa LiveResource::removeObject
					 */
					void removeDcType(QString const &dcType_) { removeObject(dc::type::iri(), AutoGen::Value< QString >::toNode(dcType_)); }

					/* Property generated from dc:language
					 * \n A language of the resource.
					 * \n rdfs:label=Language
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:modified=2277
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcLanguages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_language "dc:language" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcLanguages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::language >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcLanguages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_language "dc:language" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcLanguages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::language >(object_info, strategy, parent); }
					/*
						\fn void setDcLanguages(LiveNodes const &dcLanguage_)


						Sets the property \ref dc_language "dc:language" set to the given \a dcLanguage_ set.
						\sa LiveResource::setObjects
					 */
					void setDcLanguages(LiveNodes const &dcLanguage_) { setObjects(dc::language::iri(), dcLanguage_); }
					/*
						\fn void removeDcLanguages(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_language "dc:language" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcLanguages(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::language::iri(), object_info); }
					/*
						\fn QString getDcLanguage()


						Returns the property \ref dc_language "dc:language". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcLanguage() { return AutoGen::Value< QString >::fromNode(getObject< dc::language >()); }
					/*
						\fn void setDcLanguage(QString const &dcLanguage_)


						Sets the property \ref dc_language "dc:language" to the given \a dcLanguage.
						\sa LiveResource::setObject
					 */
					void setDcLanguage(QString const &dcLanguage_) { setObject(dc::language::iri(), AutoGen::Value< QString >::toNode(dcLanguage_ )); }
					/*
						\fn QString firstDcLanguage()


						Returns the first property \ref dc_language "dc:language". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcLanguage() { return AutoGen::Value< QString >::fromNode(firstObject< dc::language >()); }
					/*
						\fn void addDcLanguage(QString const &dcLanguage_)


						Adds the given \a dcLanguage_ to the property \ref dc_language "dc:language" set.
						\sa LiveResource::addObject
					 */
					void addDcLanguage(QString const &dcLanguage_) { addObject(dc::language::iri(), AutoGen::Value< QString >::toNode(dcLanguage_)); }
					/*
						\fn void removeDcLanguage(QString const &dcLanguage_)


						Removes the given \a dcLanguage_ from the property \ref dc_language "dc:language" set.
						\sa LiveResource::removeObject
					 */
					void removeDcLanguage(QString const &dcLanguage_) { removeObject(dc::language::iri(), AutoGen::Value< QString >::toNode(dcLanguage_)); }

					/* Property generated from dc:coverage
					 * \n The spatial or temporal topic of the resource, the spatial applicability of the resource, or the jurisdiction under which the resource is relevant.
					 * \n rdfs:label=Coverage
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:modified=2279
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcCoverages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_coverage "dc:coverage" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcCoverages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::coverage >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcCoverages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_coverage "dc:coverage" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcCoverages(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::coverage >(object_info, strategy, parent); }
					/*
						\fn void setDcCoverages(LiveNodes const &dcCoverage_)


						Sets the property \ref dc_coverage "dc:coverage" set to the given \a dcCoverage_ set.
						\sa LiveResource::setObjects
					 */
					void setDcCoverages(LiveNodes const &dcCoverage_) { setObjects(dc::coverage::iri(), dcCoverage_); }
					/*
						\fn void removeDcCoverages(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_coverage "dc:coverage" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcCoverages(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::coverage::iri(), object_info); }
					/*
						\fn QString getDcCoverage()


						Returns the property \ref dc_coverage "dc:coverage". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcCoverage() { return AutoGen::Value< QString >::fromNode(getObject< dc::coverage >()); }
					/*
						\fn void setDcCoverage(QString const &dcCoverage_)


						Sets the property \ref dc_coverage "dc:coverage" to the given \a dcCoverage.
						\sa LiveResource::setObject
					 */
					void setDcCoverage(QString const &dcCoverage_) { setObject(dc::coverage::iri(), AutoGen::Value< QString >::toNode(dcCoverage_ )); }
					/*
						\fn QString firstDcCoverage()


						Returns the first property \ref dc_coverage "dc:coverage". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcCoverage() { return AutoGen::Value< QString >::fromNode(firstObject< dc::coverage >()); }
					/*
						\fn void addDcCoverage(QString const &dcCoverage_)


						Adds the given \a dcCoverage_ to the property \ref dc_coverage "dc:coverage" set.
						\sa LiveResource::addObject
					 */
					void addDcCoverage(QString const &dcCoverage_) { addObject(dc::coverage::iri(), AutoGen::Value< QString >::toNode(dcCoverage_)); }
					/*
						\fn void removeDcCoverage(QString const &dcCoverage_)


						Removes the given \a dcCoverage_ from the property \ref dc_coverage "dc:coverage" set.
						\sa LiveResource::removeObject
					 */
					void removeDcCoverage(QString const &dcCoverage_) { removeObject(dc::coverage::iri(), AutoGen::Value< QString >::toNode(dcCoverage_)); }

					/* Property generated from dc:subject
					 * \n The topic of the resource.
					 * \n rdfs:label=Subject
					 * \n nrl:maxCardinality=1
					 * \n rdfs:range=xsd:string
					 * \n rdfs:domain=rdfs:Resource
					 * \n tracker:weight=5
					 * \n tracker:modified=2268
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveDcSubjects(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref dc_subject "dc:subject" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveDcSubjects(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< dc::subject >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getDcSubjects(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref dc_subject "dc:subject" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getDcSubjects(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< dc::subject >(object_info, strategy, parent); }
					/*
						\fn void setDcSubjects(LiveNodes const &dcSubject_)


						Sets the property \ref dc_subject "dc:subject" set to the given \a dcSubject_ set.
						\sa LiveResource::setObjects
					 */
					void setDcSubjects(LiveNodes const &dcSubject_) { setObjects(dc::subject::iri(), dcSubject_); }
					/*
						\fn void removeDcSubjects(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref dc_subject "dc:subject" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeDcSubjects(RDFVariable const &object_info = RDFVariable()) { removeObjects(dc::subject::iri(), object_info); }
					/*
						\fn QString getDcSubject()


						Returns the property \ref dc_subject "dc:subject". If the property didn't exist, it is set to a default initialized value of type QString and returned.
						\sa LiveResource::getObject
					 */
					QString getDcSubject() { return AutoGen::Value< QString >::fromNode(getObject< dc::subject >()); }
					/*
						\fn void setDcSubject(QString const &dcSubject_)


						Sets the property \ref dc_subject "dc:subject" to the given \a dcSubject.
						\sa LiveResource::setObject
					 */
					void setDcSubject(QString const &dcSubject_) { setObject(dc::subject::iri(), AutoGen::Value< QString >::toNode(dcSubject_ )); }
					/*
						\fn QString firstDcSubject()


						Returns the first property \ref dc_subject "dc:subject". If the property didn't exist, a default value is returned.
						\sa LiveResource::firstObject
					 */
					QString firstDcSubject() { return AutoGen::Value< QString >::fromNode(firstObject< dc::subject >()); }
					/*
						\fn void addDcSubject(QString const &dcSubject_)


						Adds the given \a dcSubject_ to the property \ref dc_subject "dc:subject" set.
						\sa LiveResource::addObject
					 */
					void addDcSubject(QString const &dcSubject_) { addObject(dc::subject::iri(), AutoGen::Value< QString >::toNode(dcSubject_)); }
					/*
						\fn void removeDcSubject(QString const &dcSubject_)


						Removes the given \a dcSubject_ from the property \ref dc_subject "dc:subject" set.
						\sa LiveResource::removeObject
					 */
					void removeDcSubject(QString const &dcSubject_) { removeObject(dc::subject::iri(), AutoGen::Value< QString >::toNode(dcSubject_)); }
					 // }
				};

				/*   Class generated from rdfs:Class
				 * \ingroup onto_rdfs_classes
				 *
				 * rdfs:label = Class
				 *
				 * tracker:modified = 2059
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 *
				 * 	\b Properties \b having \b this \b class \b as \b Range \n
				 * 		\ref rdfs_range "rdfs:range"
				 * 		\ref rdfs_subClassOf "rdfs:subClassOf"
				 * 		\ref rdf_type "rdf:type"
				 */
				class Class
					: public virtual Resource

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#Class"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<Resource, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_rdfs Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<Class> Live;
					Class() { this->attachInterfaceHelper< Class >(); }
					~Class() { this->detachInterfaceHelper< Class >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {

					/* Property generated from rdfs:subClassOf
					 * \n rdfs:range=rdfs:Class
					 * \n tracker:modified=2250
					 * \n rdfs:domain=rdfs:Class
					 * \n tracker:added=2010-04-08T11:21:26Z
					 */
					/*
						\fn LiveNodes liveSubClassOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns a live model of property \ref rdfs_subClassOf "rdfs:subClassOf" set.
						\sa LiveResource::liveObjects
					 */
					LiveNodes liveSubClassOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return liveObjects< rdfs::subClassOf >(object_info, strategy, parent); }
					/*
						\fn LiveNodes getSubClassOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())


						Returns all current values of property \ref rdfs_subClassOf "rdfs:subClassOf" set.
						\sa LiveResource::getObjects
					 */
					LiveNodes getSubClassOfs(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return getObjects< rdfs::subClassOf >(object_info, strategy, parent); }
					/*
						\fn void setSubClassOfs(LiveNodes const &subClassOf_)


						Sets the property \ref rdfs_subClassOf "rdfs:subClassOf" set to the given \a subClassOf_ set.
						\sa LiveResource::setObjects
					 */
					void setSubClassOfs(LiveNodes const &subClassOf_) { setObjects(rdfs::subClassOf::iri(), subClassOf_); }
					/*
						\fn void removeSubClassOfs(RDFVariable const &object_info = RDFVariable())


						Removes the properties from property \ref rdfs_subClassOf "rdfs:subClassOf" set which match the given \a object_info .
						\sa LiveResource::removeObjects
					 */
					void removeSubClassOfs(RDFVariable const &object_info = RDFVariable()) { removeObjects(rdfs::subClassOf::iri(), object_info); }
					/*
						\fn ::SopranoLive::Live< Class > firstSubClassOf()


						Returns the first property \ref rdfs_subClassOf "rdfs:subClassOf". If the property didn't exist, a null LiveNode is returned.
						\sa LiveResource::firstObject
					 */
					::SopranoLive::Live< Class > firstSubClassOf() { return firstObject< rdfs::subClassOf >(); }
					/*
						\fn ::SopranoLive::Live< Class > addSubClassOf()


						Adds a new uniquely named resource to the property \ref rdfs_subClassOf "rdfs:subClassOf" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Class > addSubClassOf() { return addObject< rdfs::subClassOf >(); }
					/*
						\fn ::SopranoLive::Live< Class > addSubClassOf(::SopranoLive::Live< Class > const &subClassOf_)


						Adds the given \a subClassOf_ to the property \ref rdfs_subClassOf "rdfs:subClassOf" set and returns a live version of it.
						\sa LiveResource::addObject
					 */
					::SopranoLive::Live< Class > addSubClassOf(::SopranoLive::Live< Class > const &subClassOf_) { return addObject(rdfs::subClassOf::iri(), subClassOf_); }
					/*
						\fn void removeSubClassOf(::SopranoLive::Live< Class > const &subClassOf_)


						Removes the given \a subClassOf_ from the property \ref rdfs_subClassOf "rdfs:subClassOf" set.
						\sa LiveResource::removeObject
					 */
					void removeSubClassOf(::SopranoLive::Live< Class > const &subClassOf_) { removeObject(rdfs::subClassOf::iri(), subClassOf_); }
					 // }
				};

				/*   Class generated from rdfs:Literal
				 * \ingroup onto_rdfs_classes
				 *
				 * rdfs:label = Literal
				 *
				 * tracker:modified = 2061
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class Literal
					: public virtual Resource

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2000/01/rdf-schema#Literal"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<Resource, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_rdfs Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<Literal> Live;
					Literal() { this->attachInterfaceHelper< Literal >(); }
					~Literal() { this->detachInterfaceHelper< Literal >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {
					 // }
				};
			}
		}
	}
}
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::rdfs::Class::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::rdfs::Literal::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::rdfs::Resource::Live)
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_CLASS_DEFINITIONS */
#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */


#if !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_POST_INCLUDES)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_RDFS_H_POST_INCLUDES
#include "dc.h"
#include "rdf.h"
#include "xsd.h"
#endif
