/*
 * rdfchain.h
 *
 *  Created on: Oct 31, 2009
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_RDFCHAIN_H_
#define SOPRANOLIVE_RDFCHAIN_H_

#include "rdfunbound.h"

namespace SopranoLive
{
	namespace Detail
	{
		struct Q_DECL_EXPORT RDFIdentityHead
			: IriClassBase<RDFIdentityHead>
		{
			static QString encodedIri() { return QString(); }
			enum
			{ 	BaseStrategy = RDFStrategy::ChainedProperty
			,	ReverseStrategy = 0
			,	chain_index = 0
			};

			RDFVariable source_and_target_;
			RDFIdentityHead(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
				: source_and_target_(source)
			{
				source.merge(target);
			}
			RDFVariable const &source() const { return source_and_target_; }
			RDFVariable const &target() const { return source_and_target_; }

			RDFVariable bind() { return source_and_target_; }
			RDFVariable bindReverse() { return source_and_target_; }

			// If you get error here, it means there is no requested custom Iri in
			// the property chain.
				template<typename Iri_>
			void custom() const;

				template<typename Predicate_>
			struct GetDomain : Predicate_ {};
		};

			template<typename Predicate_, RDFStrategyFlags property_flags, typename Head_>
		struct RDFChainProperty;

			template<typename Predicate_, RDFStrategyFlags property_flags, typename Head_>
		struct RDFChainReverseProperty;

			template<RDFStrategyFlags choose_flags, typename Head_>
		struct RDFChainChoose;

			template<typename Iri_, RDFStrategyFlags custom_flags, typename Head_>
		struct RDFChainCustom;

			template< typename BinaryOp_, typename Var_, typename Head_>
		struct RDFChainBinaryOp;

#define SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(name__, op__, flags__) \
		struct name__ \
			: IriClassBase< name__ > \
		{ \
			RDFVariable operator()(RDFVariable const &lhs, RDFVariable const &rhs) \
			{ return (lhs op__ rhs); } \
			enum { BaseStrategy = flags__ }; \
			static const char *encodedIri() { return #op__; } \
		};

		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(EqualOp, == , RDFStrategy::NonMultipleValued )
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(NotEqualOp, == , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(LessOp, < , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(LessOrEqualOp, <= , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(GreaterOp, > , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(GreaterOrEqualOp, >= , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(AddOp, + , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(SubOp, - , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(MulOp, * , 0)
		SOPRANOLIVE_DEFINE_UNBOUND_BINARYOP(DivOp, / , 0)

			template<typename Link_, typename Head_>
		struct Q_DECL_EXPORT RDFChainBase
			: IriClassBase<Link_>
		{
			typedef Head_ Head;

			static QString encodedIri() { return Link_::encodedIri(); }

				template<typename Predicate_, RDFStrategyFlags property_flags = RDFStrategy::DefaultStrategy>
			struct Property
				: RDFPropertyChain<RDFChainProperty<Predicate_, property_flags, RDFPropertyChain<Link_> > >
			{
				Property(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
					: RDFPropertyChain<RDFChainProperty<Predicate_, property_flags, RDFPropertyChain<Link_> > >
						(source, target) {}
			};

				template<typename Predicate_, RDFStrategyFlags property_flags = RDFStrategy::DefaultStrategy>
			struct ReverseProperty
				: RDFPropertyChain<RDFChainReverseProperty<Predicate_, property_flags, RDFPropertyChain<Link_> > >
			{
				ReverseProperty(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
					: RDFPropertyChain<RDFChainReverseProperty<Predicate_, property_flags, RDFPropertyChain<Link_> > >
						(source, target) {}
			};

				template<RDFStrategyFlags choose_flags>
			struct Choose
				: RDFPropertyChain<RDFChainChoose<choose_flags, RDFPropertyChain<Link_> > >
			{
				Choose(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
					: RDFPropertyChain<RDFChainChoose<choose_flags, RDFPropertyChain<Link_> > >
						(source, target) {}
			};

				template<typename Iri_, RDFStrategyFlags custom_flags = RDFStrategy::DefaultStrategy>
			struct Custom
				: RDFPropertyChain<RDFChainCustom<Iri_, custom_flags, RDFPropertyChain<Link_> > >
			{
				Custom(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
					: RDFPropertyChain<RDFChainCustom<Iri_, custom_flags, RDFPropertyChain<Link_> > >
						(source, target) {}
			};

				template<typename Iri_>
			struct Equal
				: RDFPropertyChain<RDFChainBinaryOp<EqualOp, Iri_, RDFPropertyChain<Link_> > >
			{
				Equal(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
					: RDFPropertyChain<RDFChainBinaryOp<EqualOp, Iri_, RDFPropertyChain<Link_> > >
						(source, target) {}
			};

				template<typename Iri_>
			RDFVariable custom() const
			{
				return static_cast<RDFPropertyChain<Link_> const &>(*this).head_.template custom<Iri_>();
			}

				template<typename>
			struct GetDomain : Link_ {};

			typedef Choose<RDFStrategy::CountColumn> Count;
			typedef Choose<RDFStrategy::SumColumn> Sum;
			typedef Choose<RDFStrategy::AvgColumn> Avg;
			typedef Choose<RDFStrategy::MinColumn> Min;
			typedef Choose<RDFStrategy::MaxColumn> Max;
			typedef Choose<RDFStrategy::FirstColumn> First;
		};


			template<RDFStrategyFlags choose_flags, typename Head_>
		struct Q_DECL_EXPORT RDFChainChooseRange : Head_ {};

			template<typename Head_>
		struct Q_DECL_EXPORT RDFChainChooseRange<RDFStrategy::CountColumn, Head_> { typedef int Range; };

			template<RDFStrategyFlags choose_flags, typename Head_>
		struct Q_DECL_EXPORT RDFChainChoose
			: RDFChainBase<RDFChainChoose<choose_flags, Head_>, Head_>
		{
			typedef typename Detail::RDFChainChooseRange<choose_flags, Head_>::Range Range;
			typedef typename Head_::Domain Domain;
			enum
			{
				BaseStrategy = Head_::BaseStrategy
						| (choose_flags & ~RDFStrategy::AggregateColumnMask)
						| RDFStrategy::ChainedProperty
			};

			static QString encodedIri()
			{
				return QString(Head_::encodedIri()) + "/choose" + QString::number(choose_flags) + "/";
			}

			RDFVariable connect(RDFVariable const &source, RDFVariable const &target)
			{
				RDFSubSelect sel;
				RDFStrategyFlags mode = choose_flags & RDFStrategy::AggregateColumnMask;
				if(mode == RDFStrategy::AggregateFirst)
				{
					sel.limit(1);
					mode = 0;
				} else
					mode >>= RDFStrategy::offset_AggregateColumnMode;

				source.merge(sel.newColumnAs(target, mode));
				RDFVariable ret = sel.asExpression();

				if(mode == RDFStrategy::CountColumn)
					ret.isOfType<int>();

				RDFProperty prop
						( RDFVariable(), RDFVariable(), RDFVariableLink(ret)
						, BaseStrategy | RDFStrategy::UpdateProperty);
				source.addDerivedProperty(prop);

				return ret;
			}
		};

			template<typename Iri_, RDFStrategyFlags custom_flags, typename Head_>
		struct Q_DECL_EXPORT RDFChainCustom
			: RDFChainBase<RDFChainCustom<Iri_, custom_flags, Head_>, Head_>
		{
			typedef typename Head_::Range Range;
			typedef typename Head_::Domain Domain;
			enum
			{
				BaseStrategy = Head_::BaseStrategy | custom_flags | RDFStrategy::ChainedProperty
			};
			static QString encodedIri()
			{
				return QString(Head_::encodedIri()) + "/" + Iri_::encodedIri();
			}
			RDFVariable connect(RDFVariable const &source, RDFVariable const &target)
			{
				source.merge(target); return target;
			};

				template<typename IriCompare_, typename IriAgainst_>
			struct Custom
			{
					template<typename T>
				RDFVariable operator()(T const &ths)
				{
					return ths.head_.template custom<IriAgainst_>();
				}
			};

				template<typename IriEquals_>
			struct Custom<IriEquals_, IriEquals_>
			{
					template<typename T>
				RDFVariable operator()(T const &ths)
				{
					return ths.target_;
				}
			};

				template<typename Iri2_>
			RDFVariable custom() const
			{
				return Custom<Iri_, Iri2_>()(static_cast<RDFPropertyChain<RDFChainCustom> const &>(*this));
			}

		};

			template< typename BinaryOp_, typename Var_, typename Head_>
		struct RDFChainBinaryOp
			: RDFChainBase<RDFChainBinaryOp<BinaryOp_, Var_, Head_>, Head_>
		{
			typedef typename Head_::Range Range;
			typedef typename Head_::Domain Domain;
			enum
			{
				BaseStrategy = Head_::BaseStrategy
						| RDFStrategy::ChainedProperty
						| BinaryOp_::BaseStrategy
			};
			static QString encodedIri()
			{
				return QString(Head_::encodedIri()) + "/"
						+ BinaryOp_::encodedIri() + Var_::encodedIri();
			}

			bool custom_var_;
			RDFChainBinaryOp() : custom_var_(false) {}

			RDFVariable connect(RDFVariable const &source, RDFVariable const &target)
			{
				BinaryOp_()(source, custom_var_ ? target : RDFVariable(Var_::iri()));
				return source;
			};

				template<typename IriCompare_, typename IriAgainst_>
			struct Custom
			{
					template<typename T>
				RDFVariable operator()(T const &ths)
				{
					return ths.head_.template custom<IriAgainst_>();
				}
			};

				template<typename IriEquals_>
			struct Custom<IriEquals_, IriEquals_>
			{
					template<typename T>
				RDFVariable operator()(T const &ths)
				{
					ths.custom_var_ = true;
					return ths.target_;
				}
			};

				template<typename Iri2_>
			RDFVariable custom() const
			{
				return Custom<Var_, Iri2_>()(static_cast<RDFPropertyChain<RDFChainBinaryOp> const &>(*this));
			}
		};

			template<typename Predicate_
				, RDFStrategyFlags property_flags = RDFStrategy::DefaultStrategy
				, typename Head_ = Detail::RDFIdentityHead>
		struct Q_DECL_EXPORT RDFChainProperty
			: Detail::RDFChainBase<RDFChainProperty<Predicate_, property_flags, Head_>, Head_>
		{
			typedef Detail::RDFChainBase<RDFChainProperty, Head_> Base;

			typedef typename Predicate_::Range Range;
			// should ensure that Head_::Range and Predicate_::Domain are compatible
			typedef typename Head_::template GetDomain<Predicate_>::Domain Domain;
			enum
			{ 	BaseStrategy
					= (Predicate_::BaseStrategy
						& (Head_::BaseStrategy
							| ~RDFStrategyFlags(RDFStrategy::SingleValued
							| RDFStrategy::InverseSingleValued)))
						| property_flags | RDFStrategy::ChainedProperty
			};

			static QString encodedIri()
			{
				QString tail = Head_::encodedIri();
				return (tail.size() ? tail + "/" : QString()) + Predicate_::encodedIri();
			}

			RDFVariable connect(RDFVariable const &source, RDFVariable const &target)
			{
				RDFProperty prop = RDFProperty::fromObjectOf<Predicate_>
						( RDFVariableLink(target), BaseStrategy | RDFStrategy::UpdateProperty);
				source.addDerivedProperty(prop);

				return source.template object<Predicate_>(target, property_flags);
			}
		};

			template<typename Predicate_
				, RDFStrategyFlags property_flags = RDFStrategy::DefaultStrategy
				, typename Head_ = Detail::RDFIdentityHead>
		struct Q_DECL_EXPORT RDFChainReverseProperty
			: Detail::RDFChainBase<RDFChainReverseProperty<Predicate_, property_flags, Head_>, Head_>
		{
			typedef Detail::RDFChainBase<RDFChainReverseProperty, Head_> Base;

			typedef typename Predicate_::Domain Range;
			// should ensure that Predicate_::Range and Head_::Range are compatible
			typedef typename Head_::template GetDomain<Predicate_>::Domain Domain;
			enum
			{	BaseStrategy
					= (Predicate_::BaseStrategy
						& (((Predicate_::BaseStrategy & RDFStrategy::SingleValued) << 2)
							| ((Predicate_::BaseStrategy & RDFStrategy::InverseSingleValued) >> 2)
							| ~RDFStrategyFlags(RDFStrategy::SingleValued | RDFStrategy::InverseSingleValued)))
						| property_flags | RDFStrategy::ChainedProperty
			};

			static QString encodedIri()
			{
				QString tail = Head_::encodedIri();
				return (tail.size() ? tail + "/property/" : QString()) + Predicate_::encodedIri() + "/asReversed";
			}

			RDFVariable connect(RDFVariable const &source, RDFVariable const &target)
			{
				RDFProperty prop = RDFProperty::fromSubjectOf<Predicate_>
						( RDFVariableLink(target), BaseStrategy | RDFStrategy::UpdateProperty);
				source.addDerivedProperty(prop);

				return source.template subject<Predicate_>(target, property_flags);
			}
		};
	}

		template<typename Predicate_, RDFStrategyFlags property_flags = RDFStrategy::DefaultStrategy>
	struct Q_DECL_EXPORT RDFForwardProperty
		: Detail::RDFPropertyChain<Detail::RDFChainProperty<Predicate_, property_flags> >
	{
		RDFForwardProperty(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
			: Detail::RDFPropertyChain<Detail::RDFChainProperty<Predicate_, property_flags> >(source, target) {}
	};

		template<typename Predicate_, RDFStrategyFlags property_flags = RDFStrategy::DefaultStrategy>
	struct Q_DECL_EXPORT RDFReverseProperty
		: Detail::RDFPropertyChain<Detail::RDFChainReverseProperty<Predicate_, property_flags> >
	{
		RDFReverseProperty(RDFVariable const &source = RDFVariable(), RDFVariable const &target = RDFVariable())
			: Detail::RDFPropertyChain<Detail::RDFChainReverseProperty<Predicate_, property_flags> >(source, target) {}
	};


}

#endif /* RDFCHAIN_H_ */
