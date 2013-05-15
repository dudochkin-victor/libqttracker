/*
 * nrl.h, manually edited
 */

#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_FORWARD_DECLARATIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_FORWARD_DECLARATIONS

// Namespace objects and forward declarations section

#include "../../QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace Core
		{
			/*   \defgroup onto_nrl_classes namespace nrl class definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_nrl_properties namespace nrl property definitions
			 *   {
			 *   }
			 */
			/*   \defgroup onto_nrl_resources namespace nrl resource definitions
			 *   {
			 *   }
			 */
			static const char nrl_namespace_prefix[] = "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#";
			struct Ontology_nrl
			{
				//   The defining context of this namespace
				typedef ::SopranoLive::Contexts::Core ResourceContext;
				static QUrl iri(QString suffix = QString())
				{
					return QLatin1String(nrl_namespace_prefix) + suffix;
				}
			};
			namespace nrl
			{
				inline QUrl iri(QString suffix) { return Ontology_nrl::iri(suffix); }

				/*
				 * \section classes Classes
				 */
				// {
				class InverseFunctionalProperty;
				// }
			}
		}
	}
}
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_FORWARD_DECLARATIONS */


// Property class definitions section

#if (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_PROPERTY_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_PROPERTY_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "rdf.h"
#include "xsd.h"
#elif defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#include "rdf.h"
#include "xsd.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
#else
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY
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
			namespace nrl
			{
				/*
				 * \section properties Property description classes
				 */
				// {
				/*   Property generated from nrl:maxCardinality
				 * \anchor nrl_maxCardinality
				 * \n rdfs:range=xsd:integer
				 * \n tracker:modified=2265
				 * \n rdfs:domain=rdf:Property
				 * \n tracker:added=2010-04-08T11:21:26Z
				 * \n nrl:maxCardinality=1
				 */
				/// \ingroup onto_nrl_properties
				class maxCardinality
				{	public:
					/// \return encoded iri of this property
					static const char *encodedIri() { return "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#maxCardinality"; }
					/// \return iri of this property
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					/// The defining context of this property
					typedef ::SopranoLive::Contexts::Core ResourceContext;
					/// When this property class is used as a resource it has type rdf:Property
					typedef rdf::Property ResourceType;
					//   The ontology of this property
					typedef Ontology_nrl Ontology;
					/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes.
					enum { BaseStrategy = ::SopranoLive::RDFStrategy::NonMultipleValued | ::SopranoLive::RDFStrategy::Literal };
					/// The domain/owning class of this property
					typedef rdf::Property Domain;
					/// The domain/owning class of this property (for symmetry with RDFRange)
					typedef rdf::Property RDFDomain;
					/// The localized c++ range type of this property
					typedef int Range;
					/// The full, non-localized rdf range type of this property
					typedef ::SopranoLive::Ontologies::Core::xsd::integer RDFRange;
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
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_PROPERTY_DEFINITIONS */
#endif /* (!defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */


#if ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) \
      && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \
      ) || defined(Q_MOC_RUN))
#ifndef SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_CLASS_DEFINITIONS
#define SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_CLASS_DEFINITIONS

#ifdef SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "rdf.h"
#include "xsd.h"
#undef SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#else // !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
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
			namespace nrl
			{

				using namespace rdf;
				using namespace xsd;

				/*   Class generated from nrl:InverseFunctionalProperty
				 * \ingroup onto_nrl_classes
				 *
				 * tracker:modified = 2063
				 *
				 * tracker:added = 2010-04-08T11:21:26Z
				 */
				class InverseFunctionalProperty
					: public virtual LiveResourceBridge

				{
	//			Q_OBJECT

				public:
					/*   \return encoded iri of this class */
					static const char *encodedIri() { return "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InverseFunctionalProperty"; }
					/*   \return iri of this class */
					static QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
					typedef QPair<LiveResourceBridge, ::SopranoLive::Detail::EmptyBase > BaseClasses;

					//   The defining context of this class
					typedef ::SopranoLive::Contexts::Core ResourceContext;

					//   When used as a resource, this class has a type rdfs:Class
					typedef rdfs::Class ResourceType;

					//   the ontology of this class
					typedef Ontology_nrl Ontology;

					/// The base strategy of this class
					enum { BaseStrategy = 0 };

					/// The base classes of this class

					/*
					 * A smart, by-value convenience smart \ref SopranoLive::Live pointer to this class
					 */
					typedef ::SopranoLive::Live<InverseFunctionalProperty> Live;
					InverseFunctionalProperty() { this->attachInterfaceHelper< InverseFunctionalProperty >(); }
					~InverseFunctionalProperty() { this->detachInterfaceHelper< InverseFunctionalProperty >(); }

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
Q_DECLARE_METATYPE(SopranoLive::Ontologies::Core::nrl::InverseFunctionalProperty::Live)
#endif /* SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_CLASS_DEFINITIONS */
#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */


#if !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_POST_INCLUDES)
#define SOPRANOLIVE_ONTOLOGIESGENERAL_NRL_H_POST_INCLUDES
#include "rdf.h"
#include "xsd.h"
#endif
