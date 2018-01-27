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

#ifndef LIBMWEREADER_MWEPARSER_H
#define LIBMWEREADER_MWEPARSER_H

#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/sax.h>
#include <boost/unordered_map.hpp>

#include <libwccl/parser/Parser.h>
#include <libwccl/ops/operator.h>
#include "mwe.h"

namespace Corpus2 {

class MWEBuilder
{
public:
	typedef boost::shared_ptr<Wccl::Operator<Wccl::Bool> > BoolOpPtr;

	MWEBuilder(const Tagset& tagset);

	BoolOpPtr get_head_condition(const std::string &);
	BoolOpPtr get_mwe_condition(const std::string &);


private:
	typedef boost::unordered_map<std::string, BoolOpPtr> value_type;

	BoolOpPtr get_condition(const std::string & cond, value_type& where);

	const Tagset& tagset_;
	/// str -> ptr to ccl operator
	value_type main_conditions_;
	/// str -> ptr to ccl operator
	value_type head_conditions_;

	Wccl::Parser parser_;

};

class MWEParser : public BasicSaxParser
{
public:
	MWEParser(MWEIndex &index);
	~MWEParser();

	Corpus2::Tagset get_tagset() const;

protected:
	void create_mwe();
	typedef std::map<std::string, std::string> str_map;
	void on_start_element(const Glib::ustring &name,
						  const AttributeList& attributes);

	void on_end_element(const Glib::ustring &name);
	std::string finish_get_text();

	void print_current_mwe(bool with_condition = false,
						   std::ostream& out = std::cout);


	/// retrives tagset= attribute
	std::string get_attribute(const AttributeList& attributes,
							  const std::string &name) const;
	void parse_mwegroup_attributes(const AttributeList& attributes);

	/// tagset name used in wccl operators
	std::string tagset_;

	enum States{NONE,  // not started
				UNITSDESC, // in <units_description
				MWEGROUP, // in <mwegroup>
				CONDITION, // in <condition>
				INSTANCES, // <instances>
				MWE, // start of MWE, <MWE>
				VAR, // <var> of <MWE>
				HEAD, // <head> condition of MWE
			};

	States state_;

	str_map variables_; // name -> val
	std::string wccl_operator_;
	std::string mwe_base_;
	std::string var_name_;
	std::string group_name_;
	std::string group_type_;
	std::string group_class_;
	std::string head_cond_;

	MWEIndex &mwe_index_;
	boost::shared_ptr<MWEBuilder> mwe_builder_;
};

} // ns Corpus2

#endif // LIBMWEREADER_MWEPARSER_H
