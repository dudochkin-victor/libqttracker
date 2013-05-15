/*
 * xsd.h, manually edited
 */

#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_FORWARD_DECLARATIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_FORWARD_DECLARATIONS

// Namespace objects and forward declarations section

#include "../../QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			/*   \defgroup onto_xsd_classes namespace xsd class definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_xsd_properties namespace xsd property definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_xsd_resources namespace xsd resource definitions
			 *   {
			 *   }
			 */
			static const char xsd_namespace_prefix[] = "http://www.w3.org/2001/XMLSchema#";
			struct Ontology_xsd
			{
				//   The defining context of this namespace
				typedef ::SopranoLive::Contexts::Core ResourceContext;
				static QUrl iri(QString suffix = QString())
				{
					return QLatin1String(xsd_namespace_prefix) + suffix;
				}
			};
			namespace xsd
			{
				inline QUrl iri(QString suffix) { return Ontology_xsd::iri(suffix); }

				/*
				 * \section classes Classes
				 */
				// {
				class boolean;
				class date;
				class dateTime;
				class double_;
				class integer;
				class string;
				// }
			}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_FORWARD_DECLARATIONS */


// Property class definitions section

#if (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_PROPERTY_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_PROPERTY_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "rdf.h"
#elif defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "rdf.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#else
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "rdf.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			namespace xsd
			{
				/*
				 * \section properties Property description classes
				 */
				// {
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
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_PROPERTY_DEFINITIONS */
#endif /* (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */


#if ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) \
      && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \
      ) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_CLASS_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_CLASS_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "rdf.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#else // !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "rdf.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#endif // SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			namespace xsd
			{

				using namespace rdf;

				/*   Class generated from xsd:boolean
				 * \ingroup onto_xsd_classes
				 *
				 * tracker:modified = 2053
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class boolean
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2001/XMLSchema#boolean"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_xsd Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<boolean> Live;
					boolean() { this->attachInterfaceHelper< boolean >(); }
					~boolean() { this->detachInterfaceHelper< boolean >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {
					 // }
				};

				/*   Class generated from xsd:date
				 * \ingroup onto_xsd_classes
				 *
				 * tracker:modified = 2056
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class date
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2001/XMLSchema#date"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_xsd Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<date> Live;
					date() { this->attachInterfaceHelper< date >(); }
					~date() { this->detachInterfaceHelper< date >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {
					 // }
				};

				/*   Class generated from xsd:dateTime
				 * \ingroup onto_xsd_classes
				 *
				 * tracker:modified = 2057
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class dateTime
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2001/XMLSchema#dateTime"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_xsd Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<dateTime> Live;
					dateTime() { this->attachInterfaceHelper< dateTime >(); }
					~dateTime() { this->detachInterfaceHelper< dateTime >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {
					 // }
				};

				/*   Class generated from xsd:double
				 * \ingroup onto_xsd_classes
				 *
				 * tracker:modified = 2055
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class double_
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2001/XMLSchema#double"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_xsd Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<double_> Live;
					double_() { this->attachInterfaceHelper< double_ >(); }
					~double_() { this->detachInterfaceHelper< double_ >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {
					 // }
				};

				/*   Class generated from xsd:integer
				 * \ingroup onto_xsd_classes
				 *
				 * tracker:modified = 2054
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class integer
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2001/XMLSchema#integer"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_xsd Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<integer> Live;
					integer() { this->attachInterfaceHelper< integer >(); }
					~integer() { this->detachInterfaceHelper< integer >(); }

					/*
					 * \name Property accessor and manipulator methods
					 */
					//  {
					 // }
				};

				/*   Class generated from xsd:string
				 * \ingroup onto_xsd_classes
				 *
				 * tracker:modified = 2052
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class string
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.w3.org/2001/XMLSchema#string"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_xsd Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<string> Live;
					string() { this->attachInterfaceHelper< string >(); }
					~string() { this->detachInterfaceHelper< string >(); }

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
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::xsd::boolean::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::xsd::date::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::xsd::dateTime::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::xsd::double_::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::xsd::integer::Live)
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::xsd::string::Live)
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_CLASS_DEFINITIONS */
#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */


#if !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_POST_INCLUDES)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_XSD_H_POST_INCLUDES
#include "rdf.h"
#endif
