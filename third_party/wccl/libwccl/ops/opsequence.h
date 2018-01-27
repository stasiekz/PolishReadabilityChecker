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

#ifndef LIBWCCL_OPS_OPSEQUENCE_H
#define LIBWCCL_OPS_OPSEQUENCE_H

#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <boost/foreach.hpp>

#include <libwccl/ops/operator.h>


namespace Wccl {

class FunctionalOpSequence : public Expression
{
public:
	typedef boost::shared_ptr<FunctionalOperator> fun_op_ptr_t;
	typedef boost::shared_ptr<const FunctionalOperator> fun_op_ptr_c_t;
	typedef std::pair<std::string, fun_op_ptr_t> name_op_pair_t;
	typedef std::pair<std::string, fun_op_ptr_c_t> name_op_pair_c_t;
	typedef std::vector<name_op_pair_t> name_op_v_t;
	typedef std::vector<name_op_pair_c_t> name_op_v_c_t;

	std::string name() const {
		return name_;
	}

	virtual size_t size() const = 0;

	virtual bool empty() const = 0;

	virtual FunctionalOperator& get(size_t idx) = 0;

	virtual const FunctionalOperator& get(size_t idx) const = 0;

	virtual fun_op_ptr_t get_untyped_ptr(size_t idx) = 0;

	virtual fun_op_ptr_c_t get_untyped_ptr(size_t idx) const = 0;

	name_op_v_t& add_name_op_pairs_untyped(name_op_v_t& pairs);
	
	name_op_v_c_t& add_name_op_pairs_untyped(name_op_v_c_t& pairs) const;

	name_op_v_t gen_name_op_pairs_untyped() {
		name_op_v_t v;
		return add_name_op_pairs_untyped(v);
	}

	name_op_v_c_t gen_name_op_pairs_untyped() const {
		name_op_v_c_t v;
		return add_name_op_pairs_untyped(v);
	}

	std::string gen_op_name(size_t idx) const {
		return name_ + "-" + boost::lexical_cast<std::string, size_t>(idx);
	}

	name_op_pair_t gen_name_op_pair_untyped(size_t idx) {
		return name_op_pair_t(gen_op_name(idx), get_untyped_ptr(idx));
	}

	name_op_pair_c_t gen_name_op_pair_untyped(size_t idx) const {
		return name_op_pair_c_t(gen_op_name(idx), get_untyped_ptr(idx));
	}
	boost::shared_ptr<FunctionalOpSequence> clone() const {
		return boost::shared_ptr<FunctionalOpSequence>(clone_internal());
	}

	virtual ~FunctionalOpSequence() {
	}

protected:
	FunctionalOpSequence(const std::string& name) 
		: name_(name) {
	}
	virtual FunctionalOpSequence* clone_internal() const = 0;
	std::string name_;
};

class UntypedOpSequence : public FunctionalOpSequence
{
public:
	typedef FunctionalOperator op_t;

	UntypedOpSequence(const std::string& name)
		: FunctionalOpSequence(name),
			ops_()
	{
	}

	bool empty() const {
		return ops_.empty();
	}

	size_t size() const {
		return ops_.size();
	}

	void append(const fun_op_ptr_t& op) {
		ops_.push_back(op);
	}

	op_t& get(size_t idx) {
		if(idx >= ops_.size()) {
			throw InvalidArgument("idx", "Index out of range.");
		}
		return *ops_[idx];
	}

	const op_t& get(size_t idx) const {
		return *get_ptr(idx);
	}

	fun_op_ptr_t get_untyped_ptr(size_t idx) {
		return get_ptr(idx);
	}

	fun_op_ptr_c_t get_untyped_ptr(size_t idx) const {
		return get_ptr(idx);
	}

	fun_op_ptr_t get_ptr(size_t idx) {
		if(idx >= ops_.size()) {
			throw InvalidArgument("idx", "Index out of range.");
		}
		return ops_[idx];
	}

	fun_op_ptr_c_t get_ptr(size_t idx) const {
		if(idx >= ops_.size()) {
			throw InvalidArgument("idx", "Index out of range.");
		}
		return ops_[idx];
	}

	name_op_v_t& add_name_op_pairs(name_op_v_t& pairs) {
		return add_name_op_pairs_untyped(pairs);
	}

	name_op_v_c_t& add_name_op_pairs(name_op_v_c_t& pairs) const {
		return add_name_op_pairs_untyped(pairs);
	}

	name_op_v_t gen_name_op_pairs() {
		return gen_name_op_pairs_untyped();
	}

	name_op_v_c_t gen_name_op_pairs() const {
		return gen_name_op_pairs_untyped();
	}

	name_op_pair_t gen_name_op_pair(size_t idx) {
		return gen_name_op_pair_untyped(idx);
	}

	name_op_pair_c_t gen_name_op_pair(size_t idx) const {
		return gen_name_op_pair_untyped(idx);
	}

	std::string to_string(const Corpus2::Tagset& tagset) const;

	UntypedOpSequence(const UntypedOpSequence& seq);

	boost::shared_ptr<UntypedOpSequence> clone() const {
		return boost::make_shared<UntypedOpSequence>(*this);
	}
protected:
	std::ostream& write_to(std::ostream& ostream) const;

	UntypedOpSequence* clone_internal() const {
		return new UntypedOpSequence(*this);
	}
private:
	std::vector<fun_op_ptr_t> ops_;
};

template<class T>
class OpSequence : public FunctionalOpSequence
{
	BOOST_MPL_ASSERT( (boost::is_base_of<Value, T>) );
	BOOST_MPL_ASSERT_NOT( (boost::is_same<Value, T>) );
public:
	typedef Operator<T> op_t;
	typedef typename boost::shared_ptr<Operator<T> > op_ptr_t;
	typedef typename boost::shared_ptr<const Operator<T> > op_ptr_c_t;
	typedef typename std::pair<std::string, op_ptr_t> name_op_pair_t;
	typedef typename std::pair<std::string, op_ptr_c_t> name_op_pair_c_t;
	typedef typename std::vector<name_op_pair_t> name_op_v_t;
	typedef typename std::vector<name_op_pair_c_t> name_op_v_c_t;

	OpSequence(const std::string& name)
		: FunctionalOpSequence(name),
		  ops_()
	{
	}

	bool empty() const {
		return ops_.empty();
	}

	size_t size() const {
		return ops_.size();
	}

	void append(const op_ptr_t& op) {
		ops_.push_back(op);
	}

	op_t& get(size_t idx) {
		return *get_ptr(idx);
	}

	const op_t& get(size_t idx) const {
		return *get_ptr(idx);
	}

	op_ptr_t get_ptr(size_t idx) {
		if(idx >= ops_.size()) {
			throw InvalidArgument("idx", "Index out of range.");
		}
		return ops_[idx];
	}

	op_ptr_c_t get_ptr(size_t idx) const {
		if(idx >= ops_.size()) {
			throw InvalidArgument("idx", "Index out of range.");
		}
		return ops_[idx];
	}

	FunctionalOpSequence::fun_op_ptr_t get_untyped_ptr(size_t idx) {
		return get_ptr(idx);
	}

	FunctionalOpSequence::fun_op_ptr_c_t get_untyped_ptr(size_t idx) const {
		return get_ptr(idx);
	}

	name_op_v_t gen_name_op_pairs();

	name_op_v_c_t gen_name_op_pairs() const;

	name_op_v_t& add_name_op_pairs(name_op_v_t& pairs);

	name_op_v_c_t& add_name_op_pairs(name_op_v_c_t& pairs) const;

	name_op_pair_t gen_name_op_pair(size_t idx);

	name_op_pair_c_t gen_name_op_pair(size_t idx) const;

	std::string to_string(const Corpus2::Tagset& tagset) const;

	OpSequence(const OpSequence<T>& seq);

	boost::shared_ptr<OpSequence<T> > clone() const;
protected:
	std::ostream& write_to(std::ostream& os) const;
	
	virtual OpSequence<T>* clone_internal() const;

private:
	std::vector<op_ptr_t> ops_;
};

} /* end ns Wccl */



//
// Implementation details
//

namespace Wccl {

template<class T> inline
typename OpSequence<T>::name_op_pair_t OpSequence<T>::gen_name_op_pair(
	size_t idx)
{
	return name_op_pair_t(gen_op_name(idx), get_ptr(idx));
}

template<class T> inline
typename OpSequence<T>::name_op_pair_c_t OpSequence<T>::gen_name_op_pair(
	size_t idx) const
{
	return name_op_pair_c_t(gen_op_name(idx), get_ptr(idx));
}

template<class T> inline
typename OpSequence<T>::name_op_v_t& OpSequence<T>::add_name_op_pairs(
	name_op_v_t& pairs)
{
	for(size_t i = 0; i < size(); ++i) {
		pairs.push_back(gen_name_op_pair(i));
	}
	return pairs;
}

template<class T> inline
typename OpSequence<T>::name_op_v_c_t& OpSequence<T>::add_name_op_pairs(
	name_op_v_c_t& pairs) const
{
	for(size_t i = 0; i < size(); ++i) {
		pairs.push_back(gen_name_op_pair(i));
	}
	return pairs;
}

template<class T> inline
typename OpSequence<T>::name_op_v_t OpSequence<T>::gen_name_op_pairs() {
	name_op_v_t v;
	return add_name_op_pairs(v);
}

template<class T> inline
typename OpSequence<T>::name_op_v_c_t OpSequence<T>::gen_name_op_pairs() const {
	name_op_v_c_t v;
	return add_name_op_pairs(v);
}

template<class T> inline
OpSequence<T>::OpSequence(const OpSequence<T>& seq)
	: FunctionalOpSequence(seq.name_),
	  ops_(seq.size())
{
	BOOST_FOREACH (const op_ptr_t& op, seq.ops_) {
		ops_.push_back(op->clone_ptr());
	}
}

template<class T> inline
OpSequence<T>* OpSequence<T>::clone_internal() const
{
	return new OpSequence(*this);
}

template<class T> inline
typename boost::shared_ptr<OpSequence<T> > OpSequence<T>::clone() const
{
	return boost::make_shared<OpSequence<T> >(*this);
}

namespace detail {

template<class T>
class SectionStrProvider;

template<>
class SectionStrProvider<Position>
{
public:
	static std::string typed_section_str() {
		return "@p:";
	}
};

template<>
class SectionStrProvider<Bool>
{
public:
	static std::string typed_section_str() {
		return "@b:";
	}
};

template<>
class SectionStrProvider<StrSet>
{
public:
	static std::string typed_section_str() {
		return "@s:";
	}
};

template<>
class SectionStrProvider<TSet>
{
public:
	static std::string typed_section_str() {
		return "@t:";
	}
};

template<>
class SectionStrProvider<Match>
{
public:
	static std::string typed_section_str() {
		return "@m:";
	}
};

} /* end ns detail */


template<class T> inline
std::string OpSequence<T>::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << detail::SectionStrProvider<T>::typed_section_str() << name() << " (";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ", ";
		}
		os << ops_[i]->to_string(tagset);
	}
	os << ")";
	return os.str();
}

template<class T> inline
std::ostream& OpSequence<T>::write_to(std::ostream& os) const
{
	os << detail::SectionStrProvider<T>::typed_section_str() << name() << " (";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ", ";
		}
		os << *ops_[i];
	}
	return os << ")";
}

} /* end ns Wccl */
#endif // LIBWCCL_OPS_OPSEQUENCE_H
