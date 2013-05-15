/*
 * livebag.h
 *
 *  Created on: Nov 7, 2009
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_LIVEBAG_H_
#define SOPRANOLIVE_LIVEBAG_H_

#include "live.h"
namespace SopranoLive
{
	/*!
	 * Live or non-live entity.
	 * \n By-value, smart, live, container based view into a given \ref LiveNodeModel object using a shared pointer.
	 * \n It also can be in a non-live state, where it only has a local set of values.
	 * By default, uses a QList to represent the nodes by mapping the first column of the model into the list.
	 * Alternatively, offers policy based design into how and what kind of object is used to map the \ref LiveNodeModel.
	 * \param ResourceType_ the type of objects stored in the QList.
	 * \param Policy_ the policy used to map the model into the list.
	 * Default policy has a blocking constructor policy (retrieves all data immediately).
	 * \sa \ref live_entities
	 */
		template< typename Node_
				, typename BagPolicy_>
	class Q_DECL_EXPORT LiveBag
		: public ValueOfLiveEntity< LiveNodeModel
								  , typename BagPolicy_::template ValuePolicy<Node_>
								  , LiveBag<Node_, BagPolicy_>
								  >
		, public BagPolicy_
	{
	public:
		typedef BagPolicy_ BagPolicy;

		typedef LiveBag MD;
		typedef ValueOfLiveEntity< LiveNodeModel
								 , typename BagPolicy::template ValuePolicy<Node_>
								 , MD
								 > Base;

		typedef typename Base::Value Value;
		typedef typename Base::Entity Entity;
		typedef typename Base::EntityPointer EntityPointer;
		typedef Node_ Node;

		/*!
		 * Non-blocking.
		 * Construct a non-live resource list from the given set of resources.
		 * \param resources the set of resources to be encapsulated.
		 */
		LiveBag(typename Base::Value const &nodes = typename Base::Value(), BagPolicy bag_policy = BagPolicy())
			: Base(EntityPointer(), this->template copyValue<MD>(nodes))
			, BagPolicy(bag_policy)
		{}

		/*!
		 * Blocking state depends on the given \a policy construct method.
		 * Construct a live resource list from the given \ref LiveNodeModel.
		 * \param model the live model.
		 * \param policy policy used to map the model into the list.
		 */
		LiveBag(LiveNodeModelPtr model, BagPolicy bag_policy = BagPolicy())
			: Base(model), BagPolicy(bag_policy)
		{}

		//! \overload
		LiveBag(LiveBag const &other, BagPolicy bag_policy = BagPolicy())
			: Base(), BagPolicy(bag_policy)
		{
			this->template copyConstruct<MD>(other);
		}

		/*!
		 * Non-blocking if \a other is non-live, else depends on the given \a policy construct method.
		 * Construct a resource list from an \a other \ref LiveBag.
		 * The live state will be the same as with the \a other.
		 * \param other the other \ref LiveBag.
		 * \param policy policy used to map the model into the list.
		 */
			template<typename OtherResourceType_, typename OtherPolicy_>
		LiveBag(LiveBag<OtherResourceType_, OtherPolicy_> const &other, BagPolicy bag_policy = BagPolicy())
			: Base(), BagPolicy(bag_policy)
		{
			this->template copyConstruct<MD>(other);
		}

		/*! \fn Value const &value() const
		 * \ref live_entities "Non-live blocking".
		 * \return the local value of the bag as a \ref QList<Node_>
		 */
		Value const &nodes() const
		{
			return *(this->value_ ? this->value_ : this->value_ = this->template evaluateValue<MD>());
		}

			template<typename Container_>
		Container_ toSequence() const
		{
			return this->template evaluatePlainColumn<MD, Container_>(this->template defaultColumn<MD>());
		}

			template<typename Container_>
		Container_ toSequence(int column) const
		{
			return this->template evaluatePlainColumn<MD, Container_>(column);
		}

			template<typename Container_, typename InserterFunctor_>
		Container_ toSequence(InserterFunctor_ inserter = InserterFunctor_()) const
		{
			return this->template evaluateColumn<MD, Container_>(this->template defaultColumn<MD>(), inserter);
		}

			template<typename Container_, typename InserterFunctor_>
		Container_ toSequence(int column, InserterFunctor_ inserter = InserterFunctor_()) const
		{
			return this->template evaluateColumn<MD, Container_>(column, inserter);
		}


		typedef typename Value::const_iterator const_iterator;
		typedef typename Value::iterator iterator;

		const_iterator begin() const { return this->value().begin(); }
		const_iterator end() const { return this->value().end(); }
		const_iterator constBegin() const { return this->value().constBegin(); }
		const_iterator constEnd() const { return this->value().constEnd(); }
		iterator begin() { return this->value().begin(); }
		iterator end() { return this->value().end(); }

		/*!
		 * \ref live_entities "Non-live non-blocking".
		 * \return a direct pointer to the model if live, or a null pointer otherwise.
		 */
		Entity *model() const { return this->entity_ ? &*this->entity_ : 0; }

			template<typename Res_>
		operator QList<Res_>() const { return toSequence<QList<Res_> >(); }

		operator QStringList() const { return toSequence<QStringList>(); }

		/*!
		 * Assigns a live \a model into this resource list, making it live.
		 * Blocking state depends on the active policy reconstruct method.
		 */
		LiveBag &operator=(LiveNodeModelPtr const &model)
		{
			this->template destruct<MD>();
			this->entity_ = model;
			return *this;
		}

		/*!
		 * Assigns a non-live set \a nodes of items into this node list, making it non-live.
		 */
		LiveBag &operator=(Value const &nodes)
		{
			this->template destruct<MD>();
			this->value_ = this->template copyValue<MD>(nodes);
			return *this;
		}

		/*!
		 * Blocking state depends on the active policy reconstruct method.
		 * Assigns an \a other \ref LiveBag into this resource list.
		 * The resulting live state will be the same as with \a other.
		 * \param other the other list to be assigned.
		 */
			template<typename OtherResourceType_, typename OtherPolicy_>
		LiveBag &operator=(LiveBag<OtherResourceType_, OtherPolicy_> const &other)
		{
			this->template assign<MD>(other);
			return *this;
		}

		LiveBag &operator=(LiveBag const &other)
		{
			this->template assign<MD>(other);
			return *this;
		}

		friend QDebug operator<<(QDebug s, LiveBag const &bag)
		{
			if(Entity const *model = bag.model())
				return s << *model;
			return s << bag.nodes();
		}
	};

	/*!
	 * Base policy class inherited by all \ref LiveBag policies.
	 */
	struct Q_DECL_EXPORT LiveBagPolicy
		: MostDerivedPolicy<LiveBagPolicy>
	{

			template<typename Resource_>
		struct ValuePolicy
			: ValueOfLiveEntity_Policy
		{
			/* (doxygen disabled)!
			 * A custom constructor called when a new resource is constructed in the list.
			 * \param node the node returned by the model
			 * \param row the row of the node about to be constructed
			 */
				template<typename This_, typename Result_>
			Result_ createNode(LiveNode const &node, int row = 0) const
			{
				Q_UNUSED(row);
				return AutoGen::Value<Result_>::fromNode(node);
			}

				template<typename Entity_>
			struct Defines
			{
				typedef Entity_ Entity;
				typedef QList<Resource_> Value;
				typedef Value *ValuePointer;
				typedef LiveNodeModelPtr EntityPointer;
			};

				template<typename This_>
			RDFVariable evaluateVariable() const;

				template<typename This_>
			int defaultColumn() const { return 0; }

				template<typename This_>
			typename This_::ValuePointer evaluateValue() const
			{
				This_ const &ths = this->template md<This_>();
				if(ths.value_)
					delete ths.value_;
				return ths.value_ =
					new typename This_::Value(
						ths.template evaluatePlainColumn<This_, typename This_::Value>(ths.template defaultColumn<This_>()));
			}

			struct DefaultInserter
			{
					template<typename This_, typename Result_>
				void operator()(This_ &ths, Result_ &res, LiveNode const &livenode, int row)
				{
					Q_UNUSED(row);
					res.push_back(ths.template createNode<This_, typename Result_::value_type>(livenode, row));
				}
			};

				template<typename This_, typename Result_>
			Result_ evaluatePlainColumn(int column) const
			{
				return this->template md<This_>().template evaluateColumn<This_, Result_>(column, DefaultInserter());
			}
		};
	};
}

#endif /* SOPRANOLIVE_LIVEBAG_H_ */
