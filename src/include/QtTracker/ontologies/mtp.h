/*
 * mtp.h, autogenerated
 *
 * Automatically created on: 2010-12-01 19:33:24.642094
 * Automatically created by: tools/build/detail/rdfxml2convenience.py -i data/rdf-xml/QtTracker/ontologies/mtp.xml -c QtTracker
 * DO NOT MAKE CHANGES TO THIS FILE AS THEY MIGHT BE LOST
 */

#ifndef SOPRANOLIVE_ONTOLOGIES_MTP_H_FORWARD_DECLARATIONS
#define SOPRANOLIVE_ONTOLOGIES_MTP_H_FORWARD_DECLARATIONS

// Namespace objects and forward declarations section

#include "../QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		/*! \defgroup onto_mtp_classes namespace mtp class definitions
		 *  @{
		 *  @}
		 */
		/*! \defgroup onto_mtp_properties namespace mtp property definitions
		 *  @{
		 *  @}
		 */
		/*! \defgroup onto_mtp_resources namespace mtp resource definitions
		 *  @{
		 *  @}
		 */
		static const char mtp_namespace_prefix[] = "http://www.tracker-project.org/temp/mtp#";
		struct Ontology_mtp
		{
			//! The defining context of this namespace
			typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
			static QUrl iri(QString suffix = QString())
			{
				return QLatin1String(mtp_namespace_prefix) + suffix;
			}
		};
		namespace mtp
		{
			inline QUrl iri(QString suffix) { return Ontology_mtp::iri(suffix); }

			/*!
			 * \section classes Classes
			 */
			//@{
			class ScanType;
			//@}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIES_MTP_H_FORWARD_DECLARATIONS */


// Property class definitions section

#if (!defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIES_MTP_H_PROPERTY_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIES_MTP_H_PROPERTY_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/nmm.h>
#elif defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/nmm.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#else
#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/nmm.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace mtp
		{
			/*!
			 * \section properties Property description classes
			 */
			//@{
			/*! Property generated from mtp:creator
			 * \anchor mtp_creator
			 * \n Following MTP spec, this property can contain a free text taking about creator of the Media
			 * \n rdfs:label=MTP Creator
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=nfo:Media
			 * \n tracker:weight=3
			 * \n tracker:modified=1444
			 * \n tracker:fulltextIndexed=true
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_mtp_properties
			class creator
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#creator"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_mtp Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef nfo::Media Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef nfo::Media RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from mtp:credits
			 * \anchor mtp_credits
			 * \n Following MTP spec, credits contains... FIXME
			 * \n rdfs:label=MTP credits
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=nfo:Media
			 * \n tracker:weight=2
			 * \n tracker:modified=1132
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_mtp_properties
			class credits
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#credits"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_mtp Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef nfo::Media Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef nfo::Media RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from mtp:fourCC
			 * \anchor mtp_fourCC
			 * \n These codes are used to uniquely identify video formats, and are used in various legacy and current software development kits (SDKs), including those for Microsoft Video for Windows (VfW), Microsoft DirectShow, and Microsoft Windows Media Format.
			 * \n rdfs:label=MTP fourCC code
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=nmm:Video
			 * \n tracker:modified=1025
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_mtp_properties
			class fourCC
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#fourCC"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_mtp Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef nmm::Video Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef nmm::Video RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from mtp:hidden
			 * \anchor mtp_hidden
			 * \n Hidden for the MTP protocol
			 * \n rdfs:range=xsd:boolean
			 * \n rdfs:label=Hidden
			 * \n tracker:modified=1577
			 * \n rdfs:domain=nfo:Media
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_mtp_properties
			class hidden
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#hidden"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_mtp Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef nfo::Media Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef nfo::Media RDFDomain;
				/// The localized c++ range type of this property
				typedef bool Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::boolean RDFRange;
			};
			/*! Property generated from mtp:scantype
			 * \anchor mtp_scantype
			 * \n Scan type of a video file (MTP requirement. FIXME Maybe should be move to a different ontology)
			 * \n rdfs:range=mtp:ScanType
			 * \n rdfs:label=Scan type
			 * \n tracker:modified=1167
			 * \n rdfs:domain=nmm:Video
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_mtp_properties
			class scantype
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_mtp Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = 0 };
				/// The domain/owning class of this property
				typedef nmm::Video Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef nmm::Video RDFDomain;
				/// The localized c++ range type of this property
				typedef ScanType Range;
				/// The full, non-localized rdf range type of this property
				typedef ScanType RDFRange;
			};
			/*! Property generated from mtp:waveformat
			 * \anchor mtp_waveformat
			 * \n the WAVE formats reserved with Microsoft as of June 1, 2003. These codes are used to uniquely identify audio formats, and are used in various legacy and current software development kits (SDKs), including MCI, Microsoft Video for Windows, Microsoft DirectShow, Microsoft DirectSound, and Microsoft Windows Media Format.
			 * \n rdfs:label=Wave format
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=nmm:Video
			 * \n tracker:modified=1055
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_mtp_properties
			class waveformat
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#waveformat"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_mtp Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef nmm::Video Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef nmm::Video RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			//@}
			/*!
			 * \section resources Resource description classes
			 */
			//@{
			/*! Resource generated from mtp:scantype-FieldInterleavedLowerFirst
			 * \anchor mtp_scantype_FieldInterleavedLowerFirst
			 */
			/// \ingroup onto_mtp_resources
			class scantype_FieldInterleavedLowerFirst
			{	public:
				/// \return encoded iri of this resource
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype-FieldInterleavedLowerFirst"; }
				/// \return iri of this resource
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this resource
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// The type of this resource
				typedef ScanType ResourceType;
				//! The ontology of this resource
				typedef Ontology_mtp Ontology;
				/// The base strategy of this resource
				enum { BaseStrategy = 0 };
			};
			/*! Resource generated from mtp:scantype-FieldInterleavedUpperFirst
			 * \anchor mtp_scantype_FieldInterleavedUpperFirst
			 */
			/// \ingroup onto_mtp_resources
			class scantype_FieldInterleavedUpperFirst
			{	public:
				/// \return encoded iri of this resource
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype-FieldInterleavedUpperFirst"; }
				/// \return iri of this resource
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this resource
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// The type of this resource
				typedef ScanType ResourceType;
				//! The ontology of this resource
				typedef Ontology_mtp Ontology;
				/// The base strategy of this resource
				enum { BaseStrategy = 0 };
			};
			/*! Resource generated from mtp:scantype-FieldSingleLowerFirst
			 * \anchor mtp_scantype_FieldSingleLowerFirst
			 */
			/// \ingroup onto_mtp_resources
			class scantype_FieldSingleLowerFirst
			{	public:
				/// \return encoded iri of this resource
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype-FieldSingleLowerFirst"; }
				/// \return iri of this resource
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this resource
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// The type of this resource
				typedef ScanType ResourceType;
				//! The ontology of this resource
				typedef Ontology_mtp Ontology;
				/// The base strategy of this resource
				enum { BaseStrategy = 0 };
			};
			/*! Resource generated from mtp:scantype-FieldSingleUpperFirst
			 * \anchor mtp_scantype_FieldSingleUpperFirst
			 */
			/// \ingroup onto_mtp_resources
			class scantype_FieldSingleUpperFirst
			{	public:
				/// \return encoded iri of this resource
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype-FieldSingleUpperFirst"; }
				/// \return iri of this resource
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this resource
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// The type of this resource
				typedef ScanType ResourceType;
				//! The ontology of this resource
				typedef Ontology_mtp Ontology;
				/// The base strategy of this resource
				enum { BaseStrategy = 0 };
			};
			/*! Resource generated from mtp:scantype-MixedInterlace
			 * \anchor mtp_scantype_MixedInterlace
			 */
			/// \ingroup onto_mtp_resources
			class scantype_MixedInterlace
			{	public:
				/// \return encoded iri of this resource
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype-MixedInterlace"; }
				/// \return iri of this resource
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this resource
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// The type of this resource
				typedef ScanType ResourceType;
				//! The ontology of this resource
				typedef Ontology_mtp Ontology;
				/// The base strategy of this resource
				enum { BaseStrategy = 0 };
			};
			/*! Resource generated from mtp:scantype-MixedInterlaceAndProgressive
			 * \anchor mtp_scantype_MixedInterlaceAndProgressive
			 */
			/// \ingroup onto_mtp_resources
			class scantype_MixedInterlaceAndProgressive
			{	public:
				/// \return encoded iri of this resource
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#scantype-MixedInterlaceAndProgressive"; }
				/// \return iri of this resource
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this resource
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// The type of this resource
				typedef ScanType ResourceType;
				//! The ontology of this resource
				typedef Ontology_mtp Ontology;
				/// The base strategy of this resource
				enum { BaseStrategy = 0 };
			};
			//@}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIES_MTP_H_PROPERTY_DEFINITIONS */
#endif /* (!defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */


#if ( ( !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) \
      && !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \
      ) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIES_MTP_H_CLASS_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIES_MTP_H_CLASS_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/nmm.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include <QtTracker/ontologies/nie.h>
#else // !defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/nmm.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include <QtTracker/ontologies/nie.h>
#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace mtp
		{

			using namespace nfo;
			using namespace nie;
			using namespace rdf;
			using namespace xsd;
			using namespace nmm;

			/*! Class generated from mtp:ScanType
			 * \ingroup onto_mtp_classes
			 * pre-defined set of scan types instances
			 * 
			 * rdfs:label = Scan types
			 * 
			 * tracker:modified = 1471
			 * 
			 * tracker:added = 2010-11-30T13:24:06Z
			 *
			 * 	\b Static \b resources \b of \b this \b class \b type \n
			 * 		\ref mtp_scantype_FieldInterleavedLowerFirst "mtp:scantype-FieldInterleavedLowerFirst"
			 * 		\ref mtp_scantype_FieldSingleLowerFirst "mtp:scantype-FieldSingleLowerFirst"
			 * 		\ref mtp_scantype_MixedInterlaceAndProgressive "mtp:scantype-MixedInterlaceAndProgressive"
			 * 		\ref mtp_scantype_FieldSingleUpperFirst "mtp:scantype-FieldSingleUpperFirst"
			 * 		\ref mtp_scantype_MixedInterlace "mtp:scantype-MixedInterlace"
			 * 		\ref mtp_scantype_FieldInterleavedUpperFirst "mtp:scantype-FieldInterleavedUpperFirst"
			 *
			 * 	\b Properties \b having \b this \b class \b as \b Range \n
			 * 		\ref mtp_scantype "mtp:scantype"
			 */
			class ScanType
				: public virtual nie::InformationElement

			{
//			Q_OBJECT

			public:
				/*! \return encoded iri of this class */
				static const char *encodedIri() { return "http://www.tracker-project.org/temp/mtp#ScanType"; }
				/*! \return iri of this class */
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				typedef QPair<nie::InformationElement, ::SopranoLive::Detail::EmptyBase > BaseClasses;

				//! The defining context of this class
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;

				//! When used as a resource, this class has a type rdfs:Class
				typedef rdfs::Class ResourceType;

				//! the ontology of this class
				typedef Ontology_mtp Ontology;

				/// The base strategy of this class
				enum { BaseStrategy = 0 };

				/// The base classes of this class

				/*!
				 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
				 */
				typedef ::SopranoLive::Live<ScanType> Live;
				ScanType() { this->attachInterfaceHelper< ScanType >(); }
				~ScanType() { this->detachInterfaceHelper< ScanType >(); }

				/*!
				 * \name Property accessor and manipulator methods
				 */
				// @{
				 //@}
			};


		}
	}
}
Q_DECLARE_METATYPE(SopranoLive::Ontologies::mtp::ScanType::Live)
#endif /* SOPRANOLIVE_ONTOLOGIES_MTP_H_CLASS_DEFINITIONS */
#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */


#if !defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIES_MTP_H_POST_INCLUDES)
#define SOPRANOLIVE_ONTOLOGIES_MTP_H_POST_INCLUDES
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/nmm.h>
#endif
