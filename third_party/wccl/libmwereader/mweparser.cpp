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

#include "mweparser.h"

#include <algorithm>

#include <boost/foreach.hpp>
#include <libcorpus2/tagsetmanager.h>


#include <libxml++/libxml++.h>
#include <libxml2/libxml/parser.h>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

namespace Corpus2 {

	MWEBuilder::MWEBuilder(const Tagset& tagset)
		: tagset_(tagset), parser_(tagset)
	{
	}

	MWEBuilder::BoolOpPtr MWEBuilder::get_head_condition(
		const std::string & headcond)
	{
		return get_condition(headcond, head_conditions_);
	}
	MWEBuilder::BoolOpPtr MWEBuilder::get_mwe_condition(
		const std::string &cond)
	{
		return get_condition(cond, main_conditions_);
	}

	MWEBuilder::BoolOpPtr MWEBuilder::get_condition(
		const std::string & cond, value_type& where)
	{
		value_type::iterator search = where.find(cond);

		if(search != where.end())
			return search->second;
		BoolOpPtr op = parser_.parseBoolOperator(cond);
		where[cond] = op;

		return op;
	}

	//////////////////////////////////////////////////////////////////////

	MWEParser::MWEParser(MWEIndex &index)
		: BasicSaxParser(), state_(NONE), mwe_index_(index)
	{
	}

	MWEParser::~MWEParser()
	{
		// TODO: something to do?
	}

	void MWEParser::create_mwe()
	{
		MWEBuilder::BoolOpPtr main = mwe_builder_->get_mwe_condition(
					wccl_operator_);
		MWEBuilder::BoolOpPtr head = mwe_builder_->get_head_condition(
					head_cond_);
		
		std::vector<std::string> valid_vars = main->valid_variable_names();
		for (str_map::iterator it = variables_.begin(); it != variables_.end(); ++it)
		{
			if (std::find(valid_vars.begin(), valid_vars.end(), it->first) != valid_vars.end())
			{
				if(group_type_ == "fix"){ // group_name_  -> lower case
					mwe_index_.add_lexicalunit( LexicalUnit::Ptr(new FixedLU(mwe_base_, main, head,
													  variables_)));
				} else if(group_type_ == "flex"){
					mwe_index_.add_lexicalunit(LexicalUnit::Ptr(new FlexLU(mwe_base_, main, head,
													variables_)));
				} else {
					throw Wccl::WcclError("Unknown type of lexical unit:"
											+ group_type_);
				}
		
			}
			else
			{
				std::cerr << "Warning: " << mwe_base_ << " has unknown variable " << it->first << "! Skipping." << std::endl;
			}
		}
		variables_.clear();
	}

	std::string MWEParser::get_attribute(const AttributeList& attributes,
										 const std::string &name) const
	{
		std::string value;
        BOOST_FOREACH (const Attribute& a, attributes) {
			if (a.name == name) {
				value = a.value;
			}
		}
		if (value == "")
			throw Wccl::WcclError("Attribute: "+name+" not found");
		return value;
	}

	void MWEParser::parse_mwegroup_attributes(const AttributeList& attributes)
	{
		BOOST_FOREACH (const Attribute& a, attributes) {
			if (a.name == "name") {
				group_name_ = a.value;
			} else if(a.name == "type"){
				group_type_ = a.value;
				boost::algorithm::to_lower(group_type_);
			} else if(a.name == "class"){
				group_class_ = a.value;
			}
		}

	}

	void MWEParser::on_start_element(const Glib::ustring &name,
			const AttributeList& attributes)
	{
		if(state_ == NONE && name == "units_description"){
			tagset_ = get_attribute(attributes, "tagset");
			mwe_builder_ = boost::shared_ptr<MWEBuilder>(new MWEBuilder(Corpus2::get_named_tagset(tagset_)));
			state_ = UNITSDESC;
		} else if(state_ == UNITSDESC && name == "mwegroup"){
			parse_mwegroup_attributes(attributes);
			state_ = MWEGROUP;
		} else if(state_ == MWEGROUP && name == "condition"){
			state_ = CONDITION;
			grab_characters_ = true;
			clear_buf();
		} else if(state_ == MWEGROUP && name == "instances"){
			state_ = INSTANCES;
		} else if(state_ == INSTANCES && name == "MWE"){
			state_ = MWE;
			mwe_base_ = get_attribute(attributes, "base");
		} else if(state_ == MWE && name == "var"){
			state_ = VAR;
			var_name_ = get_attribute(attributes, "name");
			grab_characters_ = true;
			clear_buf();
		} else if(state_ == MWE && name == "head"){
			state_ = HEAD;
			grab_characters_ = true;
			clear_buf();
		}

	}

	void MWEParser::on_end_element(const Glib::ustring &name)
	{
		if(name == "units_description"){
			state_ = NONE;
		} else if(state_ == CONDITION && name == "condition"){
			wccl_operator_ = finish_get_text();
			state_ = MWEGROUP;
		} else if(state_ == MWEGROUP && name == "mwegroup"){
			state_ = UNITSDESC;
		} else if(state_ == INSTANCES && name == "instances"){
			state_ = MWEGROUP;
		} else if(state_ == MWE && name == "MWE"){
			state_ = INSTANCES;
			create_mwe();
		} else if(state_ == VAR && name == "var"){
			state_ = MWE;
			variables_[var_name_] = finish_get_text();
		} else if(state_ == HEAD && name == "head"){
			state_ = MWE;
			head_cond_ = finish_get_text();
		} else{
			std::cerr << "Wrong state_:" << state_ << " for name: "
					<< name << std::endl;
		}
	}

	void MWEParser::print_current_mwe(bool with_condition, std::ostream &out)
	{
		out << "Forma podstawowa: " << mwe_base_ << "\nZmienne: ";

		BOOST_FOREACH (str_map::value_type &i, variables_)
			out << i.first << ": " << i.second << ", ";
		out << "\nWarunek głowy: " << head_cond_ << "\n";
		if(with_condition){
			out << "Grupa jednostek: " << group_name_ << std::endl;
			out << "Operator: " << wccl_operator_ << std::endl;

		}

	}

	std::string MWEParser::finish_get_text()
	{
		std::string str = get_buf();
		boost::algorithm::trim(str);
		grab_characters_ = false;
		return str;
	}

	Corpus2::Tagset MWEParser::get_tagset() const
	{
		return Corpus2::get_named_tagset(tagset_);
	}

} // ns Corpus2
