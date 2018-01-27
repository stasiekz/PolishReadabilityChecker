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

#include <libcorpus2/io/cclreader.h>
#include <boost/foreach.hpp>
#include <libxml2/libxml/parser.h>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "boostcompressor.h"


namespace Corpus2 {

bool CclReader::registered = TokenReader::register_reader<CclReader>("ccl",
	"ign,loose,strict,disamb_only,no_warn_inconsistent,no_warn_unexpected_xml,autogen_chunk_id,autogen_sent_id");

CclReader::CclReader(const Tagset& tagset, std::istream& is,
		bool disamb_only, bool disamb_sh)
	: BufferedChunkReader(tagset),
	impl_(new CclReaderImpl(*this, chunk_buf_, disamb_only, disamb_sh))
{

    //mpol: detecting compression
    unsigned char uc_start_char = is.peek();
    //byte[0] == 0x1f //gzip!
    if (uc_start_char == 0x1f)
        s_try_decompres(is);
    else
        this->is_ = &is;

}

CclReader::CclReader(const Tagset& tagset, const std::string& filename,
		bool disamb_only, bool disamb_sh)
	: BufferedChunkReader(tagset),
	impl_(new CclReaderImpl(*this, chunk_buf_, disamb_only, disamb_sh))
{
	this->is_owned_.reset(new std::ifstream(filename.c_str(),
				std::ifstream::in));

    if (!this->is_owned_->good()) {
        throw FileNotFound(filename);
    }
    else {
        //ktagowski: detecting compression
        unsigned char uc_start_char = is_owned_.get()->peek();
        if (uc_start_char == 0x1f)
            s_try_decompres(*is_owned_.get());
        else
            this->is_ = is_owned_.get();
    }
}

CclReader::~CclReader()
{
}

void CclReader::s_try_decompres(std::istream &is)
{
    is.seekg (0, is.end);
    int input_size = is.tellg();
    is.seekg(0,is.beg);

    std::string input_string;
    input_string.reserve(input_size);
    unsigned char buf[READ_BUFFER_SIZE+1];

    while(is.read(reinterpret_cast<char*>(buf),READ_BUFFER_SIZE)) {
        input_string.append(reinterpret_cast<char*>(buf),READ_BUFFER_SIZE);
    }
    input_string.append(reinterpret_cast<char*>(buf),is.gcount());

    Compressor * com = new BoostCompressor();

    if(com)
    {
        com->decompress(input_string,this->is_);
        delete com;
        com = NULL;
    }//if(com)

}

void CclReader::ensure_more()
{
	static const int BUFSIZE=1024;
	while (chunk_buf_.empty() && is().good()) {
		unsigned char buf[BUFSIZE+1];
		is().read(reinterpret_cast<char*>(buf), BUFSIZE);
		impl_->parse_chunk_raw(buf, is().gcount());
		if (is().eof()) {
			impl_->finish_chunk_parsing();
		}
	}
}

CclReaderImpl::CclReaderImpl(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf,
		bool disamb_only, bool disamb_sh)
	: XmlReader(base_reader, obuf)
{
	XmlReader::set_disamb_only(disamb_only);
	XmlReader::set_disamb_sh(disamb_sh);
	sentence_tag_name_ = "sentence";
}

CclReaderImpl::~CclReaderImpl()
{
}

void CclReaderImpl::start_chunk(const AttributeList& attributes)
{
	std::string id = get_id_from_attributes(attributes);

	chunk_ = boost::make_shared<Chunk>();
	chunk_->set_attribute("id", id);

	std::string type = get_type_from_attributes(attributes);
	if (type == "s") {
		throw XcesError("Trying to parse XCES as CCL (<chunk type=\"s\">)");
	}
	BOOST_FOREACH(const Attribute& a, attributes) {
		chunk_->set_attribute(a.name, a.value);
	}
	state_ = STATE_CHUNK;
}



void CclReaderImpl::start_sentence(const AttributeList &attributes)
{
	// find sentence id
	std::string id = "";
	BOOST_FOREACH(const Attribute& a, attributes) {
		if (a.name == "id") {
			id = a.value;
			break;
		}
	}

	ann_sent_ = boost::make_shared<AnnotatedSentence>(id);
	sent_ = ann_sent_;
	state_ = STATE_SENTENCE;
}


void CclReaderImpl::start_token(const AttributeList& attributes)
{
	XmlReader::start_token(attributes);
	token_anns_.clear();
	token_ann_heads_.clear();
}

bool CclReaderImpl::process_start_element(const Glib::ustring & name,
	const AttributeList& attributes)
{
	if (state_ == STATE_TOK && name == "ann") {
		state_ = STATE_ANN;
		grab_characters_ = true;
		clear_buf();
		ann_chan_ = "";
		ann_head_ = false;
		BOOST_FOREACH(const Attribute& a, attributes) {
			if (a.name == "chan") {
				ann_chan_ = a.value;
			} else if (a.name == "head" && a.value == "1") {
				ann_head_ = true;
			}
		}
		if (ann_chan_.empty()) {
			throw XcesError("<ann> with no channel name");
		}
		return true;
	} else if (state_ == STATE_TOK && name == "prop") {
		state_ = STATE_PROP;
		grab_characters_ = true;
		clear_buf();
		prop_key_ = "";
		BOOST_FOREACH(const Attribute& a, attributes) {
			if (a.name == "key") {
				prop_key_ = a.value;
			}
		}
		return true;
	} else {
		return false;
	}
}

bool CclReaderImpl::process_end_element(const Glib::ustring & name)
{
	if (state_ == STATE_ANN && name == "ann") {
		std::string buf = get_buf();
		grab_characters_ = false;
		int segid = atoi(buf.c_str());
		if (!ann_sent_->has_channel(ann_chan_)) {
			ann_sent_->create_channel(ann_chan_);
		}
		if (segid > 0) {
			token_anns_.insert(std::make_pair(ann_chan_, segid));
			if (ann_head_) {
				token_ann_heads_.insert(ann_chan_);
			}
		}
		state_ = STATE_TOK;
		return true;
	} else if (state_ == STATE_PROP && name == "prop") {
		std::string prop_val = get_buf();
		boost::algorithm::trim(prop_val);
		grab_characters_ = false;
		if (!tok_->get_metadata()) {
			tok_->create_metadata();
		}
		tok_->get_metadata()->set_attribute(prop_key_, prop_val);

		state_ = STATE_TOK;
		return true;
	} else {
		return false;
	}
}

void CclReaderImpl::finish_token()
{
	XmlReader::finish_token();
	BOOST_FOREACH(const token_ann_t::value_type& v, token_anns_) {
		ann_sent_->get_channel(v.first).set_segment_at(sent_->size() - 1, v.second);
		if (token_ann_heads_.find(v.first) != token_ann_heads_.end()) {
			ann_sent_->get_channel(v.first).set_head_at(sent_->size() - 1, true);
		}
	}
}

void CclReader::set_option(const std::string& option)
{
	if (option == "no_warn_inconsistent") {
		impl_->set_warn_on_inconsistent(false);
	} else if (option == "disamb_only") {
		impl_->set_disamb_only(true);
	} else if (option == "autogen_sent_id") {
		// no action, left for backwards compatibility
	} else if (option == "autogen_chunk_id") {
		// no action, left for backwards compatibility
	} else if (option == "no_warn_unexpected_xml") {
		impl_->set_warn_on_unexpected(false);
	}
	else {
		BufferedChunkReader::set_option(option);
	}
}

std::string CclReader::get_option(const std::string& option) const
{
	if (option == "disamb_only") {
		return impl_->get_disamb_only() ? option : "";
	} else if (option == "no_warn_inconsistent") {
		return impl_->get_warn_on_inconsistent() ? "" : option;
	} else if (option == "autogen_sent_id") {
		return option; // left for backward compatibility
	} else if (option == "autogen_chunk_id") {
		return option; // left for backward compatibility
	} else if (option == "no_warn_unexpected_xml") {
		return impl_->get_warn_on_unexpected() ? "" : option;
	}
	return BufferedChunkReader::get_option(option);
}

} /* end ns Corpus2 */
