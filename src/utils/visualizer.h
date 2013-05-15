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
 * visualizer.h
 *
 *  Created on: Oct 14, 2010
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_VISUALIZER_H_
#define SOPRANOLIVE_VISUALIZER_H_

#include <QDebug>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QProcess>
#include <QTime>

namespace SopranoLive
{
#ifdef VISUALIZER_ENABLE
	namespace Visualizer
	{
	 	QIODevice *visualizerDevice();
	 	void visualizerPostOutput(QIODevice *out_device);

		static struct Dummy { virtual ~Dummy() {}; } dummy;

#define VISUALIZER_KNOWNADDRESS ((char *) &::SopranoLive::Visualizer::dummy)

#define VISUALIZER_OFFSETOF(type, member) \
		(((char *) &((type *) VISUALIZER_KNOWNADDRESS)->member) - VISUALIZER_KNOWNADDRESS)

#define VISUALIZER_STR_STR(x) #x
#define VISUALIZER_STR(x) VISUALIZER_STR_STR(x)

		QDebug outputHeader(QIODevice *, const char *op);

			template<class T>
		struct LifetimeTracker
		{
			void out(const char *op)
			{
				if(QIODevice *out_device = visualizerDevice())
				{
					T::visualizerClassOut(outputHeader(out_device, op), (char*)this)
						<< "\t\nVZREND\n";
					visualizerPostOutput(out_device);
				}
			}

			LifetimeTracker() { out("C"); }
			LifetimeTracker(LifetimeTracker const &) { out("CP"); }
			~LifetimeTracker() { out("D"); }
		};

			template<class T>
		struct MemberBuiltin
		{
			T val_;
			MemberBuiltin(T const &val = T()) : val_(val) {}

			operator T() const { return val_; }

			bool operator!() const { return !val_; }
			T operator~() const { return ~val_; }

				template<class U>
			T operator+(U const &rhs) const { return val_ + rhs; }
				template<class U>
			T operator-(U const &rhs) const { return val_ - rhs; }
				template<class U>
			T operator*(U const &rhs) const { return val_ * rhs; }
				template<class U>
			T operator/(U const &rhs) const { return val_ / rhs; }
				template<class U>
			T operator|(U const &rhs) const { return val_ | rhs; }
				template<class U>
			T operator&(U const &rhs) const { return val_ & rhs; }
				template<class U>
			T operator||(U const &rhs) const { return val_ || rhs; }
				template<class U>
			T operator&&(U const &rhs) const { return val_ && rhs; }
				template<class U>
			T operator<<(U const &rhs) const { return val_ & rhs; }
				template<class U>
			T operator>>(U const &rhs) const { return val_ & rhs; }
		};

			template<class T>
		struct MemberBuiltin<T *>
		{
			T *val_;
			MemberBuiltin(T *val = 0) : val_(val) {}
			T *operator->() const { return val_; }
			T &operator*() const { return *val_; }

			operator T *() const { return val_; }

			bool operator!() const { return !val_; }
		};

			template<class T>
		char isClassTester(void(T::*)(void));

			template<class T>
		double isClassTester(...);

			template<class T, bool inheritable = (sizeof(isClassTester<T>(0)) == 1)>
		struct MemberContainerSelector { typedef T Type; };

			template<class T>
		struct MemberContainerSelector<T, false> { typedef MemberBuiltin<T> Type; };

			template<class T>
		T &visualizerValue(T &val) { return val; }

			template<class T>
		T &visualizerValue(MemberBuiltin<T> &c) { return c.val_; }

			template<class T>
		T &visualizerValue(MemberBuiltin<T &> &c) { return c.val_; }

			template<class T>
		typename T::VisualizerThis *outTypeSuffix(QDebug s, T const *)
		{
			s << "/" << T::visualizerClassName();
			return 0;
		}

			template<class T>
		void outTypeSuffix(QDebug, ...) {}

			template<class T>
		QDebug operator<<(QDebug s, T const *p)
		{
			outTypeSuffix<T>(s << "* " << (void*)p, p);
			return s;
		}

			template<class T>
		QDebug operator<<(QDebug s, QSharedPointer<T> const &p) { return s << p.data(); }

			template<class T>
		QDebug operator<<(QDebug s, QWeakPointer<T> const &p) { return s << p.data(); }

			template<class T, int chooser = sizeof(qDebug() << T())>
		struct QDebuggable
		{
			struct Dummy { char val[2]; };
			typedef Dummy Type;
		};

			template<class T>
		typename QDebuggable<T>::Type outputSelector(T const *val, T const *val2);

			template<class T>
		char outputSelector(T const *val, ...);

			template<class T, int = sizeof(outputSelector((T*)0, (T*)0))>
		struct OutputValue
		{
			static QDebug out(QDebug d, T const *val)
			{
				return d << *val;
			}
		};

			template<class T>
		struct OutputValue<T, 1>
		{
			static QDebug out(QDebug d, T const *val)
			{
				return d << val;
			}
		};

			template
			< class T
			, class C
			, QDebug  (*baseOut)(QDebug, char *)>
		struct BaseTracker
		{
			void out(const char *op)
			{
				if(QIODevice *out_device = visualizerDevice())
				{
					(*baseOut)(outputHeader(out_device, op), (char*)this) << "\nVZREND\n";
					visualizerPostOutput(out_device);
				}
			}

			BaseTracker() { out("BC"); }
			BaseTracker(BaseTracker const &) { out("BCP"); }
			~BaseTracker() { out("BD"); }
		};

			template<class T>
		const char *baseClassName(const char *, T *, typename T::VisualizerThis * = 0)
		{
			return T::visualizerClassName();
		}

			template<class T>
		const char *baseClassName(const char *name, ...) { return name; }

			template<class P, class QP = P>
		struct Ptrify
		{
			typedef QP &Type;
		};

			template<class QP>
		struct Ptrify<int, QP>
		{
			typedef Ptrify<int, QP> Type;
			int p_;
			Ptrify(int p) : p_(p) {}
			operator int() { return p_; }
			template<class T> operator T *() { return 0; }
		};

			template
			< class T
			, class C
			, QDebug  (*memberOut)(QDebug, char *)
			, class B = typename MemberContainerSelector<T>::Type>
		struct MemberTracker
			: B
		{
			T &value() { return visualizerValue(*static_cast<B *>(this)); }

			void out(const char *op, bool mute_value = false)
			{
				if(QIODevice *out_device = visualizerDevice())
				{
					QDebug out = (*memberOut)(outputHeader(out_device, op), (char*)this)
											<< "\t";
					if(!mute_value)
						OutputValue<T>::out(out.nospace(), &value());
					out.nospace() << "\nVZREND\n";
					visualizerPostOutput(out_device);
				}
			}

			MemberTracker() : B() { out("MCD"); }
			MemberTracker(T const &val) : B(val) { out("MCP"); }
			MemberTracker(MemberTracker const &val) : B(val) { out("MCP"); }
			~MemberTracker() { out("MD", true); }

				template<class P1>
			MemberTracker(P1 &p1)
				: B(typename Ptrify<P1>::Type(p1))
			{ 	out("MC"); }

				template<class P1>
			MemberTracker(P1 const &p1)
				: B(typename Ptrify<P1, P1 const>::Type(p1))
			{ 	out("MC"); }

				template<class P1, class P2>
			MemberTracker(P1 const &p1, P2 const &p2)
				: B(p1, p2)
			{ 	out("MC"); }

				template<class P1, class P2, class P3>
			MemberTracker(P1 const &p1, P2 const &p2, P3 const &p3)
				: B(p1, p2, p3)
			{ 	out("MC"); }

				template<class P1, class P2, class P3, class P4>
			MemberTracker(P1 const &p1, P2 const &p2, P3 const &p3, P4 const &p4)
				: B(p1, p2, p3, p4)
			{ 	out("MC"); }

				template<class P1, class P2, class P3, class P4, class P5>
			MemberTracker(P1 const &p1, P2 const &p2, P3 const &p3, P4 const &p4, P5 const &p5)
				: B(p1, p2, p3, p4, p5)
			{ 	out("MC"); }

				template<class P1, class P2, class P3, class P4, class P5, class P6>
			MemberTracker(P1 const &p1, P2 const &p2, P3 const &p3, P4 const &p4, P5 const &p5, P6 const &p6)
				: B(p1, p2, p3, p4, p5, p6)
			{ 	out("MC"); }

				template<class U>
			MemberTracker &operator=(U &other)
			{
				value() = typename Ptrify<U, U>::Type(other);
				out("M=");
				return *this;
			}

				template<class U>
			MemberTracker &operator=(U const &other)
			{
				value() = typename Ptrify<U, U const>::Type(other);
				out("M=");
				return *this;
			}

			T *operator&()
			{
				out("M&");
				return &value();
			}

			MemberTracker &operator++()
			{
				value()++;
				out("M++");
				return *this;
			}

			T operator++(int)
			{
				T ret(value());
				value()++;
				out("M++");
				return ret;
			}

			MemberTracker &operator--()
			{
				value()--;
				out("M--");
				return *this;
			}

			T operator--(int)
			{
				T ret(value());
				value()--;
				out("M--");
				return ret;
			}

				template<class U>
			MemberTracker &operator+=(U const &other) { value() += other; out("M+="); return *this; }
				template<class U>
			MemberTracker &operator-=(U const &other) { value() -= other; out("M-="); return *this; }
				template<class U>
			MemberTracker &operator*=(U const &other) { value() *= other; out("M*="); return *this; }
				template<class U>
			MemberTracker &operator/=(U const &other) { value() /= other; out("M/="); return *this; }
				template<class U>
			MemberTracker &operator&=(U const &other) { value() &= other; out("M&="); return *this; }
				template<class U>
			MemberTracker &operator|=(U const &other) { value() |= other; out("M|="); return *this; }
				template<class U>
			MemberTracker &operator<<=(U const &other) { value() <<= other; out("M<<="); return *this; }
				template<class U>
			MemberTracker &operator>>=(U const &other) { value() >>= other; out("M>>="); return *this; }
		};

			template
			< class T
			, class LC, class RC
			, QDebug (*LmemberOut)(QDebug, char *), QDebug (*RmemberOut)(QDebug, char *)
			, class B>
		void qSwap
				( MemberTracker<T, LC, LmemberOut, B> &lh
				, MemberTracker<T, RC, RmemberOut, B> &rh)
		{
			T val = lh.value();
			lh = rh.value();
			rh = val;
		}
	}

#define VZR_CLASS_C_0(class_name_, custom_string_) \
	typedef class_name_ VisualizerThis; \
	static const char *visualizerClassName() { return #class_name_; } \
	static QDebug visualizerClassOut(QDebug d, char *lifetime_tracker) \
	{ \
		return d.nospace() << (void*)(lifetime_tracker \
						- VISUALIZER_OFFSETOF(VisualizerThis, visualizer_lifetime_tracker_)) \
				<< "/" << visualizerClassName() << "\t" << #class_name_ << "\t\t" \
				<< #custom_string_ "&File=" __FILE__ "&Line=" VISUALIZER_STR(__LINE__); \
	} \
	::SopranoLive::Visualizer::LifetimeTracker<VisualizerThis> visualizer_lifetime_tracker_;

#define VISUALIZER_CAT_DO(a,b) a ## b
#define VISUALIZER_CAT(a,b) VISUALIZER_CAT_DO(a,b)
#define VISUALIZER_CALL(f,args) f args
#define VISUALIZER_EXPAND(...) __VA_ARGS__

#define VISUALIZER_BASE_CLASS(index_, base_) \
	typedef VISUALIZER_CALL(VISUALIZER_EXPAND, base_) VisualizerBase##index_; \
	static QDebug visualizerBaseString_##index_(QDebug d, char *base_tracker) \
	{ \
		VisualizerThis *ths = (VisualizerThis *)(base_tracker \
						- VISUALIZER_OFFSETOF(VisualizerThis, visualizer_base_tracker_##index_)); \
		VisualizerBase##index_ *base = ths; \
		return d.nospace() << (void*)ths << "/" << visualizerClassName() \
				<< "\t" << ::SopranoLive::Visualizer::baseClassName<VisualizerBase##index_>(#base_, base) \
				<< "\t" << "Base" #base_ << "\t&Relation=Owns&File=" __FILE__ "&Line=" VISUALIZER_STR(__LINE__) "\t* " \
				<< (void *)base << "/" << ::SopranoLive::Visualizer::baseClassName<VisualizerBase##index_>(#base_, base); \
	} \
	::SopranoLive::Visualizer::BaseTracker \
			<VisualizerBase##index_, VisualizerThis, &VisualizerThis::visualizerBaseString_##index_>  \
					visualizer_base_tracker_##index_;

#define VZR_CLASS_C_1(class_name, custom_string_, b0) \
			VZR_CLASS_C_0(class_name, custom_string_) VISUALIZER_BASE_CLASS(0, b0)
#define VZR_CLASS_C_2(class_name, custom_string_, b0, b1) \
			VZR_CLASS_C_1(class_name, custom_string_, b0) VISUALIZER_BASE_CLASS(1, b1)
#define VZR_CLASS_C_3(class_name, custom_string_, b0, b1, b2) \
			VZR_CLASS_C_2(class_name, custom_string_, b0, b1) VISUALIZER_BASE_CLASS(2, b2)
#define VZR_CLASS_C_4(class_name, custom_string_, b0, b1, b2, b3) \
			VZR_CLASS_C_3(class_name, custom_string_, b0, b1, b2) VISUALIZER_BASE_CLASS(3, b3)
#define VZR_CLASS_C_5(class_name, custom_string_, b0, b1, b2, b3, b4) \
			VZR_CLASS_C_4(class_name, custom_string_, b0, b1, b2, b3) VISUALIZER_BASE_CLASS(4, b4)
#define VZR_CLASS_C_6(class_name, custom_string_, b0, b1, b2, b3, b4, b5) \
			VZR_CLASS_C_5(class_name, custom_string_, b0, b1, b2, b3, b4) VISUALIZER_BASE_CLASS(5, b5)
#define VZR_CLASS_C_7(class_name, custom_string_, b0, b1, b2, b3, b4, b5, b6) \
			VZR_CLASS_C_6(class_name, custom_string_, b0, b1, b2, b3, b4, b5) VISUALIZER_BASE_CLASS(6, b6)
#define VZR_CLASS_C_8(class_name, custom_string_, b0, b1, b2, b3, b4, b5, b6, b7) \
			VZR_CLASS_C_7(class_name, custom_string_, b0, b1, b2, b3, b4, b5, b6) VISUALIZER_BASE_CLASS(7, b7)
#define VZR_CLASS_C_9(class_name, custom_string_, b0, b1, b2, b3, b4, b5, b6, b7, b8) \
			VZR_CLASS_C_8(class_name, custom_string_, b0, b1, b2, b3, b4, b5, b6, b7) VISUALIZER_BASE_CLASS(8, b8)

	/*
	* The PP_NARG macro evaluates to the number of arguments that have
	been
	* passed to it.
	*
	* Laurent Deniau, "__VA_NARG__," 17 January 2006, <comp.std.c> (29
	November 2007).
	*/
	#define PP_NARG(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
	#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)

	#define PP_ARG_N( \
	_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,N,...) N

	#define PP_RSEQ_N() \
	62,61,60, \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	9,8,7,6,5,4,3,2,1,0

#define VZR_CLASS_C_N(custom_string_, class_name_, bases_...) \
public: \
	VISUALIZER_CAT(VZR_CLASS_C_, PP_NARG( , ## bases_)) (class_name_, custom_string_ , ## bases_)


#define VZR_MEMBER_M_T_CV_N_C(mutable_, type_, cv_, name_, custom_string_) \
public: \
	static QDebug visualizerMemberString_##name_(QDebug d, char *member) \
	{ \
		return d.nospace() << (void*)(member - VISUALIZER_OFFSETOF(VisualizerThis, name_.value())) \
				<< "/" << visualizerClassName() \
				<< "\t" #type_ "\t" #name_ "\t" \
						#custom_string_ "&File=" __FILE__ "&Line=" VISUALIZER_STR(__LINE__); \
	} \
	mutable_ ::SopranoLive::Visualizer::MemberTracker \
	 	 	 <type_, VisualizerThis, &VisualizerThis::visualizerMemberString_##name_> cv_ name_;

#else

#define VZR_CLASS_C_N(custom_string_, class_name_and_bases_...)
#define VZR_MEMBER_M_T_CV_N_C(mutable_, type_, cv_, name_, custom_string_) mutable_ type_ cv_ name_;

#endif

#define VZRX_CLASS(class_name_and_bases_...) VZR_CLASS_C_N( , ## class_name_and_bases_)
#define VZRX_CLASS_C(class_name_, custom_string_, bases_...) \
	VZR_CLASS_C_N(custom_string_, class_name, ## bases_)
}
#endif /* SOPRANOLIVE_VISUALIZER_H_ */
