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

#include <libcorpus2/io/cclwriter.h>
#include <boost/foreach.hpp>
#include <libcorpus2/ann/annotatedsentence.h>
#include <libcorpus2/io/xcescommon.h>
#include <libcorpus2/tokenmetadata.h>

#include "boostcompressor.h"

namespace Corpus2 {

bool CclWriter::registered = TokenWriter::register_writer<CclWriter>("ccl",
		"flat,chunk,nochunk,nodisamb,sorttags,split,ws");

//ktagowski Added for uncompressed files
CclWriter::CclWriter(std::ostream& os, const Tagset& tagset,
        const string_range_vector& params, bool enable_compression
                     )
    : XmlWriter(os, tagset, params)
{
    if(enable_compression)
       pc_compressor = new BoostCompressor();

    do_header();
}

CclWriter::CclWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: XmlWriter(os, tagset, params)
{
	do_header();
}

CclWriter::~CclWriter()
{
    finish();
}

void CclWriter::finish()
{
    if (needs_footer_ == true)
     {
        do_footer();
        needs_footer_ = false;
        if(pc_compressor!=NULL)
        {
            pc_compressor->compress(sstream(),os());
    //        pc_compressor->finish_compression(os_);
        }
        else
        {
            os() << sstream().str();
        }
    }
}
void CclWriter::write_sentence(const Sentence& s)
{
    paragraph_head();
	if (use_indent_) indent_more();
	write_sentence_int(s);
	if (use_indent_) indent_less();
    ssi() << "</chunk>\n";
}

void CclWriter::write_sentence_int(const Sentence &s)
{
	const boost::shared_ptr<Sentence> sentence = s.clone_shared();
	const boost::shared_ptr<AnnotatedSentence> ann = AnnotatedSentence::wrap_sentence(sentence);

	std::string id = s.id();
	if (id == "") {
        ssi() << "<sentence>\n";
	}
    else {
        ssi() << "<sentence id=\"";
        sstream() << id << "\">\n";
	}

	if (use_indent_) indent_more();
	for (size_t idx = 0; idx < s.size(); ++idx) {
		const Token* t = s.tokens()[idx];
		if (ann) {
            token_as_xces_xml_head(sstream(), *t, use_indent_ ? indent_level() : -1, whitespace_info_);
			if (use_indent_) indent_more();
            token_as_xces_xml_body(sstream(), tagset(), *t, use_indent_ ? indent_level() : -1, output_disamb_, sort_tags_);
			BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& v, ann->all_channels()) {
                ssi() << "<ann chan=\"";
                sstream() << v.first << "\"";
				if (v.second.is_head_at(idx)) {
                    sstream() << " head=\"1\"";
				}
                sstream() << ">";
                sstream() << v.second.get_segment_at(idx);
                sstream() << "</ann>\n";
			}
			boost::shared_ptr<TokenMetaData> md = t->get_metadata();
			if (md) {
				BOOST_FOREACH(const TokenMetaData::attr_map_t::value_type& v, md->attributes()) {
                    ssi() << "<prop key=\"";
                    sstream() << v.first << "\"" << ">";
                    encode_xml_entities_into(sstream(), v.second);
                    sstream() << "</prop>\n";
				}
			}
			if (use_indent_) indent_less();
            ssi() << "</tok>\n";
		} else {
			// TODO: currently writing of token metadata is supported only when
			// we've got an AnnotatedSentence.
			XmlWriter::write_token(*t);
		}
	}
    if (use_indent_) indent_less();	ssi() << "</sentence>\n"; //sstream() << "</sentence>\n";
}

void CclWriter::write_chunk(const Chunk &c)
{
    paragraph_head(c);
	if (use_indent_) indent_more();
	BOOST_FOREACH(const Sentence::ConstPtr& s, c.sentences()) {
		write_sentence_int(*s);
	}
	if (use_indent_) indent_less();
    ssi() << "</chunk>\n";
    //sstream() << "</chunk>\n";
}

void CclWriter::do_header()
{
	XmlWriter::do_header();
    sstream() << "<!DOCTYPE chunkList SYSTEM \"ccl.dtd\">\n";
    sstream() << "<chunkList";
    sstream() << ">\n";
	if (use_indent_) indent_more();
}

void CclWriter::do_footer()
{
    needs_footer_ = false;
	if (use_indent_) indent_less();
    sstream() << "</chunkList>\n";
}

void CclWriter::paragraph_head()
{
    //ktagowski: Added for compression.
    //Every 3000 chunk compression or saving sstream
    if(++cid_%CHUNKS_PER_SAVE==0)
    {
       if(pc_compressor != NULL){
             pc_compressor->compress(sstream(),os());
             sstream().str(std::string());
             sstream().clear();
       }
       else
       {
            os() << sstream().str();
            sstream().str(std::string());
            sstream().clear();
       }

    }
    ssi() << "<chunk id=\"ch";
    sstream() << cid_ << "\"" << " type=\"p\">\n";
}

void CclWriter::paragraph_head(const Chunk& c)
{
	// in CCL format chunks may have at most two attributes:
	// id (unique XML-style id) and type (typically p for paragraphs)
    ssi() << "<chunk";
	if (c.has_attribute("id")) {
		const std::string &val = c.get_attribute("id");
		if (!val.empty()) {
            sstream() << " id=\"" << val << "\"";
		}
	}
	if (c.has_attribute("type")) {
		const std::string &val = c.get_attribute("type");
		if (!val.empty()) {
            sstream() << " type=\"" << val << "\"";
		}
	}
    sstream() << ">\n";
}

} /* end ns Corpus2 */
