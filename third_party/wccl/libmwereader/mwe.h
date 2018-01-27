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

#ifndef LIBMWEREADER_MWE_H
#define LIBMWEREADER_MWE_H

#include <boost/unordered_map.hpp>

#include <libcorpus2/io/reader.h>
#include <libwccl/ops/operator.h>
#include <libwccl/values/strset.h>

namespace Corpus2 {



class LexicalUnit
{
public:
	typedef std::map<std::string, std::string> strmap;

	typedef std::map<std::string, Wccl::StrSet> variables_map;

	typedef std::set<std::string> strset;
	typedef boost::shared_ptr<Wccl::Operator<Wccl::Bool> > BoolOpPtr;

	LexicalUnit(const std::string &base, BoolOpPtr condition,
				BoolOpPtr head_cond, strmap variables
				);

	/**
	  * \param sc SentenceContext with position set to value which
	  * will be checked
	  * \param out_positions will contain absolute position in
	  * SentenceContext (called with sc->get_abs_position) only if
	  * the main condition of this LexicalUnit will return true in current
	  * sentence context
	  * \returns true if this lexical unit was found here
	  */
	virtual bool IsHere(const Wccl::SentenceContext& sc,
						std::set<int> &out_positions, int &head_pos) const;

	const std::string & get_base() const{ return base_;}
	const variables_map & get_variables() const{ return variables_;}
	const strset& get_potential_bases() const{ return potential_bases_;}


	typedef boost::shared_ptr<LexicalUnit> Ptr;
protected:

	boost::shared_ptr<Wccl::Operator<Wccl::Bool> > condition_;
	boost::shared_ptr<Wccl::Operator<Wccl::Bool> > head_cond_;
	variables_map variables_;
	std::string base_;

	strset potential_bases_;

	const Wccl::Position nowhere_;
};




// TODO: czy bedzie potrzebny podzial na fix/flex w kodzie?
class FixedLU : public LexicalUnit
{
public:
	FixedLU(const std::string &base,
				LexicalUnit::BoolOpPtr condition,
				LexicalUnit::BoolOpPtr head_cond,
				LexicalUnit::strmap variables
				);
};

class FlexLU : public LexicalUnit
{
public:
	FlexLU(const std::string &base,
				LexicalUnit::BoolOpPtr condition,
				LexicalUnit::BoolOpPtr head_cond,
				LexicalUnit::strmap variables
				);

};

class MWEIndex // lub base -> vector<LexicalUnit>
{
public:
	typedef std::vector<LexicalUnit::Ptr> luvec;

	MWEIndex();

	void add_lexicalunit(LexicalUnit::Ptr lu);

	const luvec& get_potential_lu(const std::string& base);

	// TODO: method for reordering units by "length"

protected:

	typedef boost::unordered_map<std::string,luvec> value_type;


	value_type index_;

	const luvec empty_;
};

}// ns Corpus2

#endif //LIBMWEREADER_MWE_H
