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

#include <libwccl/ops/opsequence.h>

namespace Wccl {

FunctionalOpSequence::name_op_v_t& FunctionalOpSequence::add_name_op_pairs_untyped(
	name_op_v_t& pairs)
{
	for(size_t i = 0; i < size(); ++i) {
		pairs.push_back(gen_name_op_pair_untyped(i));
	}
	return pairs;
}

FunctionalOpSequence::name_op_v_c_t& FunctionalOpSequence::add_name_op_pairs_untyped(
	name_op_v_c_t& pairs) const
{
	for(size_t i = 0; i < size(); ++i) {
		pairs.push_back(gen_name_op_pair_untyped(i));
	}
	return pairs;
}

std::string UntypedOpSequence::to_string(const Corpus2::Tagset& tagset) const
{
	std::ostringstream os;
	os << "@" << name() << " (";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ", ";
		}
		os << ops_[i]->to_string(tagset);
	}
	os << ")";
	return os.str();
}

std::ostream& UntypedOpSequence::write_to(std::ostream& os) const
{
	os << "@" << name() << " (";
	for (size_t i = 0; i < size(); ++i) {
		if (i != 0) {
			os << ", ";
		}
		os << *ops_[i];
	}
	return os << ")";
}

UntypedOpSequence::UntypedOpSequence(const UntypedOpSequence& seq)
	: FunctionalOpSequence(seq.name_),
	  ops_(seq.size())
{
	BOOST_FOREACH (const fun_op_ptr_t& op, seq.ops_) {
		ops_.push_back(op->clone_ptr());
	}
}

} /* end ns Wccl */
