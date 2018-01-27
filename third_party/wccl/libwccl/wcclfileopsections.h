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

#ifndef LIBWCCL_WCCLFILEOPSECTIONS_H
#define LIBWCCL_WCCLFILEOPSECTIONS_H

#include <libwccl/ops/opsequence.h>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

namespace Wccl {

template<class T>
class WcclFileOpSections : boost::noncopyable
{
	BOOST_MPL_ASSERT( (boost::is_base_of<FunctionalOpSequence, T>) );
	BOOST_MPL_ASSERT_NOT( (boost::is_same<FunctionalOpSequence, T>) );
public:
	typedef typename T::op_t op_t;
	typedef typename boost::shared_ptr<op_t> op_ptr_t;
	typedef typename boost::shared_ptr<const op_t> op_ptr_c_t;
	typedef typename boost::shared_ptr<T> ptr_t;
	typedef typename boost::shared_ptr<const T> ptr_c_t;
	typedef typename std::vector<ptr_t> ptr_v_t;
	typedef typename boost::unordered_map<std::string, ptr_t> map_t;
	typedef typename T::name_op_v_t name_op_v_t;
	typedef typename T::name_op_v_c_t name_op_v_c_t;

protected:	
	
	bool has_section(const std::string& name) const;

	const ptr_v_t& sections();
	
	size_t size() const;
	bool empty() const;

	std::vector<std::string> section_names() const;

	T& get_section(const std::string& name);
	const T& get_section(const std::string& name) const;

	ptr_t get_section_ptr(const std::string& name);
	ptr_c_t get_section_ptr(const std::string& name) const;

	op_t& get_op(const std::string& name, size_t idx = 0);
	const op_t& get_op(const std::string& name, size_t idx = 0) const;

	op_ptr_t get_op_ptr(const std::string& name, size_t idx = 0);
	op_ptr_c_t get_op_ptr(const std::string& name, size_t idx = 0) const;

	name_op_v_t& add_name_op_pairs(name_op_v_t& pairs);
	name_op_v_c_t& add_name_op_pairs(name_op_v_c_t& pairs) const;

	name_op_v_t gen_name_op_pairs();
	name_op_v_c_t gen_name_op_pairs() const;
	
	WcclFileOpSections()
		: sections_(),
		  name_section_map_()
	{
	}

	void append(const ptr_t& section);

	ptr_v_t sections_;
	map_t name_section_map_;
};

} /* end ns Wccl */


//
// Implementation details
//
namespace Wccl {

template<class T> inline
bool WcclFileOpSections<T>::has_section(const std::string& name) const
{
	return name_section_map_.find(name) != name_section_map_.end();
}

template<class T> inline
const typename WcclFileOpSections<T>::ptr_v_t& WcclFileOpSections<T>::sections()
{
	return sections_;
}

template<class T> inline
bool WcclFileOpSections<T>::empty() const
{
	return sections_.empty();
}

template<class T> inline
size_t WcclFileOpSections<T>::size() const
{
	return sections_.size();
}

template<class T> inline
std::vector<std::string> WcclFileOpSections<T>::section_names() const
{
	std::vector<std::string> v;
	BOOST_FOREACH (const ptr_t& section, sections_) {
		v.push_back(section->name());
	}
	return v;
}

template<class T> inline
typename WcclFileOpSections<T>::ptr_t WcclFileOpSections<T>::get_section_ptr(const std::string& name)
{
	typename map_t::iterator i = name_section_map_.find(name);
	if (i == name_section_map_.end()) {
		throw InvalidArgument("name", "Section named \"" + name + "\" does not exist.");
	}
	return i->second;
}

template<class T> inline
typename WcclFileOpSections<T>::ptr_c_t WcclFileOpSections<T>::get_section_ptr(const std::string& name) const
{
	typename map_t::const_iterator i = name_section_map_.find(name);
	if (i == name_section_map_.end()) {
		throw InvalidArgument("name", "Section named \"" + name + "\" does not exist.");
	}
	return i->second;
}

template<class T> inline
T& WcclFileOpSections<T>::get_section(const std::string& name)
{
	return *get_section_ptr(name);
}

template<class T> inline
const T& WcclFileOpSections<T>::get_section(const std::string& name) const
{
	return *get_section_ptr(name);
}

template<class T> inline
typename T::name_op_v_t& WcclFileOpSections<T>::add_name_op_pairs(name_op_v_t& pairs)
{
	BOOST_FOREACH (const ptr_t& section, sections_) {
		section->add_name_op_pairs(pairs);
	}
	return pairs;
}

template<class T> inline
typename T::name_op_v_c_t& WcclFileOpSections<T>::add_name_op_pairs(name_op_v_c_t& pairs) const
{
	BOOST_FOREACH (const ptr_t& section, sections_) {
		section->add_name_op_pairs(pairs);
	}
	return pairs;
}

template<class T> inline
typename T::name_op_v_t WcclFileOpSections<T>::gen_name_op_pairs()
{
	name_op_v_t pairs;
	BOOST_FOREACH (const ptr_t& section, sections_) {
		section->add_name_op_pairs(pairs);
	}
	return pairs;
}

template<class T> inline
typename T::name_op_v_c_t WcclFileOpSections<T>::gen_name_op_pairs() const
{
	name_op_v_c_t pairs;
	BOOST_FOREACH (const ptr_t& section, sections_) {
		section->add_name_op_pairs(pairs);
	}
	return pairs;
}

template<class T> inline
void WcclFileOpSections<T>::append(const ptr_t& section)
{
	if (has_section(section->name())) {
		throw InvalidArgument("section", "Section named \"" + section->name() + "\" already added.");
	}
	sections_.push_back(section);
	name_section_map_[section->name()] = section;
}

template<class T> inline
typename T::op_t& WcclFileOpSections<T>::get_op(
	const std::string& name,
	size_t idx)
{
	return get_section(name).get(idx);
}

template<class T> inline
const typename T::op_t& WcclFileOpSections<T>::get_op(
	const std::string& name,
	size_t idx) const
{
	return get_section(name).get(idx);
}

template<class T> inline
typename WcclFileOpSections<T>::op_ptr_t WcclFileOpSections<T>::get_op_ptr(
	const std::string& name,
	size_t idx)
{
	return get_section(name).get_ptr(idx);
}

template<class T> inline
typename WcclFileOpSections<T>::op_ptr_c_t WcclFileOpSections<T>::get_op_ptr(
	const std::string& name,
	size_t idx) const
{
	return get_section(name).get_ptr(idx);
}

} /* end ns Wccl */
#endif // LIBWCCL_WCCLFILEOPSECTIONS_H
