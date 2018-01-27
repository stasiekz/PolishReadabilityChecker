/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#include "conllwriter.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include <iostream>

namespace Corpus2 {

bool ConllWriter::registered = TokenWriter::register_writer<ConllWriter>("conll");

ConllWriter::ConllWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), myTagset(tagset)
{
}

ConllWriter::~ConllWriter()
{
	finish();
}

void ConllWriter::write_token(const Token &tok)
{
	const Lexeme &lex = tok.get_preferred_lexeme(myTagset);
	std::vector<std::string> segs = this->get_columns_from_tag(lex.tag());

	os() << tok.orth_utf8() << "\t" << lex.lemma_utf8() << "\t";
	os() << segs[0] << "\t" << segs[1] << "\t" << segs[2] << "\t";
	os()<< "_\t_\t_\t_";
}

std::vector<std::string> ConllWriter::get_columns_from_tag(const Tag& tag)
{
	std::vector<std::string> columns(3);

	std::vector<std::string> segs;
	std::string tagstr = myTagset.tag_to_string(tag);
	std::transform(tagstr.begin(), tagstr.end(), tagstr.begin(), ::tolower);
	boost::split(segs, tagstr, boost::is_any_of(":"));

	bool has_superpos = this->tag_has_superpos(tag);

	columns[0] = has_superpos ? segs[1] : segs[0];
	columns[1] = segs[0];

	std::vector<std::string> features(segs.begin() + (has_superpos ? 2 : 1), segs.end());
	columns[2] = boost::algorithm::join(features, "|");
	columns[2] = columns[2].empty() ? "_" : columns[2];

	return columns;
}

bool ConllWriter::tag_has_superpos(const Tag& tag)
{
	idx_t superpos_attr_idx = myTagset.get_attribute_index(std::string("superpos"));
	if(superpos_attr_idx == -1)
		return false;
	idx_t pos_idx = myTagset.get_pos_index(tag.get_pos());

	bool requires = myTagset.pos_requires_attribute(pos_idx, superpos_attr_idx);
	bool has = myTagset.pos_has_attribute(pos_idx, superpos_attr_idx);
	bool has_value = tag.get_values_for(myTagset.get_attribute_mask(superpos_attr_idx)) != 0;
	return requires || (has && has_value);
}

void ConllWriter::write_sentence(const Sentence& s)
{
	int i=1;
	BOOST_FOREACH(const Token* t, s.tokens()) {
		os()<<i<<"\t";
		write_token(*t);
		os()<<"\n";
		i++;
	}
	os()<<"\n";
}

void ConllWriter::write_chunk(const Chunk &c)
{
	BOOST_FOREACH(const Sentence::ConstPtr& s, c.sentences()) {
		write_sentence(*s);
	}
}

void ConllWriter::do_header()
{

}

void ConllWriter::do_footer()
{
}

} /* end ns Corpus2 */
