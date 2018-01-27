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

#include <libcorpus2/io/xcesvalidate.h>
#include <libcorpus2/tagset.h>

#include <boost/foreach.hpp>
#include <libxml++/libxml++.h>
#include <libxml++/nodes/node.h>
#include <libxml++/nodes/element.h>
#include <libxml++/parsers/saxparser.h>

#include <iostream>
namespace Corpus2 {

class XcesValidatorImpl : public xmlpp::SaxParser
{
public:
	XcesValidatorImpl(const Tagset& tagset, std::ostream& out);

protected:
	void on_start_element(const Glib::ustring & name,
			const AttributeList& attributes);
	void on_end_element(const Glib::ustring & name);
	void on_cdata_block(const Glib::ustring & text);
	void on_characters(const Glib::ustring & text);

	const Tagset& tagset_;

	enum state_t { XS_NONE, XS_TOK, XS_ORTH, XS_LEX, XS_LEMMA, XS_TAG };
	state_t state_;

	std::string last_orth_;

	std::string sbuf_;

	std::ostream& os_;

	int token_idx_;

	int tag_idx_;
};

XcesValidator::XcesValidator(const Tagset& tagset, std::ostream& out)
	: impl_(new XcesValidatorImpl(tagset, out))
{
}

XcesValidator::~XcesValidator()
{
}

void XcesValidator::validate_stream(std::istream &is)
{
	impl_->parse_stream(is);
}

void XcesValidator::validate_file(const std::string& filename)
{
	impl_->parse_file(filename);
}

XcesValidatorImpl::XcesValidatorImpl(const Tagset& tagset,
		std::ostream& out)
	: xmlpp::SaxParser()
	, tagset_(tagset), state_(XS_NONE)
	, last_orth_(), sbuf_(), os_(out), token_idx_(0), tag_idx_(0)
{
}



void XcesValidatorImpl::on_start_element(const Glib::ustring &name,
		const AttributeList& /*attributes*/)
{
	if (name == "tok") {
		state_ = XS_TOK;
		++token_idx_;
		tag_idx_ = 0;
		last_orth_ = "";
	} else if (state_ == XS_TOK && name == "orth") {
		state_ = XS_ORTH;
		sbuf_ = "";
	} else if (state_ == XS_TOK && name == "lex") {
		state_ = XS_LEX;
	} else if (state_ == XS_LEX && name == "ctag") {
		state_ = XS_TAG;
		++tag_idx_;
		sbuf_ = "";
	}
}


namespace {
	void error_preamble(std::ostream& os, const std::string& orth,
			const std::string& tag, int tokenid, int tagid) {
		os << "Token " << tokenid << " (" << orth << "), tag " << tagid
			<< " (" << tag << "): ";
	}
}

void XcesValidatorImpl::on_end_element(const Glib::ustring &name)
{
	if (state_ == XS_ORTH && name == "orth") {
		last_orth_ = sbuf_;
		state_ = XS_TOK;
	} else if (state_ == XS_TAG && name == "ctag") {
		try {
			Tag tag = tagset_.parse_simple_tag(sbuf_);
			std::stringstream ss;
			if (!tagset_.validate_tag(tag, Tagset::ParseStrict, &ss)) {
				error_preamble(os_, last_orth_, sbuf_, token_idx_, tag_idx_);
				os_ << ss.str() << "\n";
			}
		} catch (TagParseError& e) {
			error_preamble(os_, last_orth_, sbuf_, token_idx_, tag_idx_);
			os_ << e.info() << "\n";
		}
		state_ = XS_LEX;
	} else if (state_ == XS_LEX && name == "lex") {
		state_ = XS_TOK;
	} else if (state_ == XS_TOK && name == "tok") {
		state_ = XS_NONE;
	}
}

void XcesValidatorImpl::on_cdata_block(const Glib::ustring &/*text*/)
{
}

void XcesValidatorImpl::on_characters(const Glib::ustring &text)
{
	sbuf_ += (std::string)text;
}

} /* end ns Corpus2 */
