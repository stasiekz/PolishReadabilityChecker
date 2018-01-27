/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libwccl/variables.h>
#include <boost/mpl/always.hpp>
#include <boost/mpl/pop_front.hpp>
#include <libwccl/values/match.h>
#include <libwccl/values/position.h>
#include <libwccl/values/strset.h>
#include <libwccl/values/tset.h>

namespace Wccl {

Variables::Variables()
{
}

namespace {
struct delhelper
{
	Variables& v;
	const std::string& s;
	bool& r;
	delhelper(Variables& v, const std::string& s, bool& r): v(v), s(s), r(r) {}

	template<typename T>
	void operator()(const boost::mpl::always<T>&) {
		r = v.del_variable<T>(s) || r;
	}
};
} /* end anon ns */

bool Variables::del_any(const std::string &s)
{
	bool rv = false;
	typedef boost::mpl::pop_front< types >::type concrete;
	// call delhelper::operator()<T> once for each of the allowed
	// Value subtypes (but not for Value itself).
	boost::mpl::for_each<concrete, boost::mpl::always<boost::mpl::_1> >(
		delhelper(*this, s, rv));
	return rv;
}

namespace {
struct puthelper
{
	Variables& v;
	const std::string& s;
	bool& rv;
	const boost::shared_ptr<Value>& p;
	puthelper(Variables& v, const std::string& s, bool& rv,
		const boost::shared_ptr<Value>& p): v(v), s(s), rv(rv), p(p) {}

	template<typename T>
	void operator()(const boost::mpl::always<T>&) {
		if (rv) return;
		boost::shared_ptr<T> t = boost::dynamic_pointer_cast<T>(p);
		if (t) {
			rv = true;
			v.put(s, t);
		}
	}
};
} /* end anon ns */

void Variables::put_any(const std::string &s, const boost::shared_ptr<Value> &v)
{
	bool rv = false;
	typedef boost::mpl::pop_front< types >::type concrete;
	// call puthelper::operator()<T> once for each of the allowed
	// Value subtypes (but not for Value itself).
	boost::mpl::for_each<concrete, boost::mpl::always<boost::mpl::_1> >(
		puthelper(*this, s, rv, v));
	if (!rv) throw VariableTypeMismatch(s);
}

namespace {
struct resethelper
{
	const Variables& v;
	resethelper(Variables& v): v(v) {}

	template<typename T>
	void operator()(const boost::mpl::always<T>&) {
		typedef std::pair< std::string, boost::shared_ptr<T> > v_t;
		BOOST_FOREACH (const v_t& a, v.get_all<T>()) {
			*a.second = T();
		}
	}
};
} /* end anon ns */


void Variables::reset_values()
{
	typedef boost::mpl::pop_front< types >::type concrete;
	boost::mpl::for_each<concrete, boost::mpl::always<boost::mpl::_1> >(
		resethelper(*this));
}

namespace {
struct clonehelper
{
	const Variables& vfrom;
	Variables::AccessHelper vto;
	clonehelper(const Variables& vfrom, Variables::AccessHelper vto)
		: vfrom(vfrom), vto(vto) {}

	template<typename T>
	void operator()(const boost::mpl::always<T>&) {
		vto.access<T>() = vfrom.get_all<T>();
		typedef typename detail::Vmap<T>::map_t::value_type value_type;
		BOOST_FOREACH (const value_type& a, vto.access<T>()) {
			vto.vars.put(a.first, *a.second);
		}
	}
};
template<> inline
void clonehelper::operator()(const boost::mpl::always<Value>&) {
	vto.access<Value>() = vfrom.get_all<Value>();
}
} /* end anon ns */

Variables* Variables::clone() const
{
	Variables* copy = new Variables;
	boost::mpl::for_each<types, boost::mpl::always<boost::mpl::_1> >(
		clonehelper(*this, Variables::AccessHelper(*copy)));
	return copy;
}

} /* end ns Wccl */
