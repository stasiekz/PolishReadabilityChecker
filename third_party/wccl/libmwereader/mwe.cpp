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

#include "mwe.h"
#include <boost/algorithm/string.hpp>
#include <libwccl/values/strset.h>
#include <boost/algorithm/string/predicate.hpp>

namespace Corpus2{

LexicalUnit::LexicalUnit(const std::string &base,
						 LexicalUnit::BoolOpPtr condition,
						 LexicalUnit::BoolOpPtr head_cond,
						 LexicalUnit::strmap variables)
	: condition_(condition),
	  head_cond_(head_cond),
	  base_(base),
	  nowhere_(Wccl::Position())
{

	for(strmap::iterator iter = variables.begin();
		iter != variables.end(); ++iter){
		potential_bases_.insert(iter->second);
		Wccl::StrSet ss;
		ss.insert_utf8(iter->second);
		variables_[iter->first] = ss;
	}

}

bool LexicalUnit::IsHere(const Wccl::SentenceContext &sc,
					std::set<int> &out_position, int &head_pos) const
{
	// set variables, skip vars with names starting with '!'
	for(variables_map::const_iterator ivars = variables_.begin();
		ivars != variables_.end(); ++ivars){			
			if(!boost::starts_with(ivars->first, "!")){
				/*std::cout << " -- " << base_ << " -- " << ivars->first << " -- " << std::endl;
				
				for (unsigned i = 0; i < condition_->valid_variable_names().size(); i++)
					std::cout << condition_->valid_variable_names()[i] << std::endl;*/
				
				condition_->set<Wccl::StrSet>(ivars->first, ivars->second);
				//std::cout << " -- egi --" << std::endl;
			}
		}

	// fire up the operator
	boost::shared_ptr<const Wccl::Bool> pResult = condition_->apply(sc);
	if(pResult->get_value() == false)
		return false;

	bool found_head = false;
	bool head_defined = false;

	Wccl::SentenceContext sc2(sc.get_sentence_ptr());

	// fill up positions
	BOOST_FOREACH (const std::string&varname, condition_->valid_variable_names()){
		if(boost::algorithm::starts_with(varname, "Pos")){
			Wccl::Position pos = condition_->get<Wccl::Position>(varname);
			if(pos.equals(nowhere_)){
				std::string errmsg("Position for found MWE cannot be zero.");
				errmsg += " Offending unit: " + base_;
				throw Wccl::WcclError(errmsg);
			}
			int abs_pos = sc.get_abs_position(pos);
			out_position.insert( abs_pos );
			if(!found_head){
				sc2.set_position(abs_pos);
				if(head_cond_->apply(sc2)->get_value()) {
					if (!head_defined)
						head_pos = abs_pos;
					found_head = true;
				}
			}
		}
		if(boost::algorithm::starts_with(varname, "Head")) {
			Wccl::Position predefined_head_pos = condition_->get<Wccl::Position>(varname);
			head_pos = sc.get_abs_position(predefined_head_pos);
			head_defined = true;
		}
	}

	if(!found_head){
		std::string errmsg("MWE found, but no head for it.");
		errmsg += " Offending unit: " + base_;
		//throw Wccl::WcclError(errmsg);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////
// TODO realy needed?

FixedLU::FixedLU(const std::string &base,
				boost::shared_ptr<Wccl::Operator<Wccl::Bool> > condition,
				boost::shared_ptr<Wccl::Operator<Wccl::Bool> > head_cond,
				std::map<std::string, std::string> variables)
		: LexicalUnit(base, condition, head_cond, variables)
{
}

FlexLU::FlexLU(const std::string &base,
				boost::shared_ptr<Wccl::Operator<Wccl::Bool> > condition,
				boost::shared_ptr<Wccl::Operator<Wccl::Bool> > head_cond,
				std::map<std::string, std::string> variables)
		: LexicalUnit(base, condition, head_cond, variables)
{
}

////////////////////////////////////////////////////////////////////

MWEIndex::MWEIndex() : index_(), empty_()
{
	// noop
}

void MWEIndex::add_lexicalunit(LexicalUnit::Ptr lu)
{
	BOOST_FOREACH (const std::string& base, lu->get_potential_bases()){
		value_type::iterator find = index_.find(base);
		//std::cout << "b:"<<base<<std::endl;
		if(find == index_.end()){ // not found -> create new one
			luvec v;
			v.push_back(lu);
			index_.insert( std::make_pair(base, v));
		}else{// already exists -> add lu
			(find->second).push_back(lu);
		}
	}
}

const MWEIndex::luvec& MWEIndex::get_potential_lu(const std::string &base){
	//std::cout << "index " << index_.size()<< std::endl;
	//std::cout << "sb:"<<base<<std::endl;
	value_type::iterator find = index_.find(base);
	if(find == index_.end()){ // not found -> return empty
		return empty_;
	}

	return (find->second);
}

}//ns Corpus2
