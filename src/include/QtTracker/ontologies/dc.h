/*
 * dc.h, autogenerated
 *
 * Automatically created on: 2010-12-01 19:33:24.207785
 * Automatically created by: tools/build/detail/rdfxml2convenience.py -i data/rdf-xml/QtTracker/ontologies/dc.xml -c QtTracker
 * DO NOT MAKE CHANGES TO THIS FILE AS THEY MIGHT BE LOST
 */

#ifndef SOPRANOLIVE_ONTOLOGIES_DC_H_FORWARD_DECLARATIONS
#define SOPRANOLIVE_ONTOLOGIES_DC_H_FORWARD_DECLARATIONS

// Namespace objects and forward declarations section

#include "../QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		/*! \defgroup onto_dc_classes namespace dc class definitions
		 *  @{
		 *  @}
		 */
		/*! \defgroup onto_dc_properties namespace dc property definitions
		 *  @{
		 *  @}
		 */
		/*! \defgroup onto_dc_resources namespace dc resource definitions
		 *  @{
		 *  @}
		 */
		static const char dc_namespace_prefix[] = "http://purl.org/dc/elements/1.1/";
		struct Ontology_dc
		{
			//! The defining context of this namespace
			typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
			static QUrl iri(QString suffix = QString())
			{
				return QLatin1String(dc_namespace_prefix) + suffix;
			}
		};
		namespace dc
		{
			inline QUrl iri(QString suffix) { return Ontology_dc::iri(suffix); }

			/*!
			 * \section classes Classes
			 */
			//@{
			//@}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIES_DC_H_FORWARD_DECLARATIONS */


// Property class definitions section

#if (!defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIES_DC_H_PROPERTY_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIES_DC_H_PROPERTY_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/rdfs.h>
#elif defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/rdfs.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#else
#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/rdfs.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace dc
		{
			/*!
			 * \section properties Property description classes
			 */
			//@{
			/*! Property generated from dc:contributor
			 * \anchor dc_contributor
			 * \n An entity responsible for making contributions to the resource.
			 * \n rdfs:label=Contributor
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:weight=4
			 * \n tracker:modified=1068
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class contributor
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/contributor"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:coverage
			 * \anchor dc_coverage
			 * \n The spatial or temporal topic of the resource, the spatial applicability of the resource, or the jurisdiction under which the resource is relevant.
			 * \n rdfs:label=Coverage
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:modified=1569
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class coverage
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/coverage"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:creator
			 * \anchor dc_creator
			 * \n An entity primarily responsible for making the resource.
			 * \n rdfs:label=Creator
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:weight=4
			 * \n tracker:modified=1015
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class creator
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/creator"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:date
			 * \anchor dc_date
			 * \n A point or period of time associated with an event in the lifecycle of the resource.
			 * \n rdfs:range=xsd:dateTime
			 * \n rdfs:label=Date
			 * \n tracker:modified=1865
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class date
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/date"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QDateTime Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::dateTime RDFRange;
			};
			/*! Property generated from dc:description
			 * \anchor dc_description
			 * \n An account of the resource.
			 * \n rdfs:label=Description
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:weight=3
			 * \n tracker:modified=1460
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class description
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/description"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:format
			 * \anchor dc_format
			 * \n The file format, physical medium, or dimensions of the resource.
			 * \n rdfs:label=Format
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:modified=1305
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class format
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/format"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:identifier
			 * \anchor dc_identifier
			 * \n An unambiguous reference to the resource within a given context.
			 * \n rdfs:label=Identifier
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:modified=1757
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class identifier
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/identifier"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:language
			 * \anchor dc_language
			 * \n A language of the resource.
			 * \n rdfs:label=Language
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:modified=1433
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class language
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/language"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:publisher
			 * \anchor dc_publisher
			 * \n An entity responsible for making the resource available.
			 * \n rdfs:label=Publisher
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:weight=4
			 * \n tracker:modified=1566
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class publisher
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/publisher"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:relation
			 * \anchor dc_relation
			 * \n A related resource.
			 * \n rdfs:range=xsd:string
			 * \n rdfs:label=Relation
			 * \n tracker:modified=1312
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class relation
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/relation"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:rights
			 * \anchor dc_rights
			 * \n Information about rights held in and over the resource.
			 * \n rdfs:label=Rights
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:modified=1314
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class rights
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/rights"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:source
			 * \anchor dc_source
			 * \n A related resource from which the described resource is derived.
			 * \n rdfs:range=rdfs:Resource
			 * \n rdfs:label=Source
			 * \n tracker:modified=1622
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class source
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/source"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = 0 };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef rdfs::Resource Range;
				/// The full, non-localized rdf range type of this property
				typedef rdfs::Resource RDFRange;
			};
			/*! Property generated from dc:subject
			 * \anchor dc_subject
			 * \n The topic of the resource.
			 * \n rdfs:label=Subject
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:weight=5
			 * \n tracker:modified=1802
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class subject
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/subject"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:title
			 * \anchor dc_title
			 * \n A name given to the resource.
			 * \n rdfs:label=Title
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:weight=10
			 * \n tracker:modified=1370
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class title
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/title"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
				/// The localized c++ range type of this property
				typedef QString Range;
				/// The full, non-localized rdf range type of this property
				typedef ::SopranoLive::Ontologies::xsd::string RDFRange;
			};
			/*! Property generated from dc:type
			 * \anchor dc_type
			 * \n The nature or genre of the resource.
			 * \n rdfs:label=Type
			 * \n nrl:maxCardinality=1
			 * \n rdfs:range=xsd:string
			 * \n rdfs:domain=rdfs:Resource
			 * \n tracker:modified=1883
			 * \n tracker:added=2010-11-30T13:24:06Z
			 */
			/// \ingroup onto_dc_properties
			class type
			{	public:
				/// \return encoded iri of this property
				static const char *encodedIri() { return "http://purl.org/dc/elements/1.1/type"; }
				/// \return iri of this property
				static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
				/// The defining context of this property
				typedef ::SopranoLive::Contexts::QtTracker ResourceContext;
				/// When this property class is used as a resource it has type rdf:Property
				typedef rdf::Property ResourceType;
				//! The ontology of this property
				typedef Ontology_dc Ontology;
				/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
				enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
				/// The domain/owning class of this property
				typedef rdfs::Resource Domain;
				/// The domain/owning class of this property (for symmetry with RDFRange)
				typedef rdfs::Resource RDFDomain;
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
			//@}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIES_DC_H_PROPERTY_DEFINITIONS */
#endif /* (!defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */


#if ( ( !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) \
      && !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \
      ) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIES_DC_H_CLASS_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIES_DC_H_CLASS_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/rdfs.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#else // !defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/rdfs.h>
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace dc
		{

			using namespace rdf;
			using namespace xsd;
			using namespace rdfs;


		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIES_DC_H_CLASS_DEFINITIONS */
#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */


#if !defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIES_DC_H_POST_INCLUDES)
#define SOPRANOLIVE_ONTOLOGIES_DC_H_POST_INCLUDES
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/xsd.h>
#include <QtTracker/ontologies/rdfs.h>
#endif