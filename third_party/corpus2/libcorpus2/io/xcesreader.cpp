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

#include <libcorpus2/io/xcesreader.h>
#include <libcorpus2/io/xmlreader.h>
#include <boost/foreach.hpp>
#include <libxml++/libxml++.h>
#include <libxml2/libxml/parser.h>
#include <boost/make_shared.hpp>
#include <fstream>

namespace Corpus2 {

bool XcesReader::registered = TokenReader::register_reader<XcesReader>("xces",
	"ign,loose,strict,disamb_only,sh,no_warn_inconsistent");

class XcesReaderImpl : public XmlReader
{
public:
	XcesReaderImpl(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf,
		bool disamb_only, bool disamb_sh);

	~XcesReaderImpl();

protected:
};

XcesReader::XcesReader(const Tagset& tagset, std::istream& is,
		bool disamb_only, bool disamb_sh)
	: BufferedChunkReader(tagset),
	impl_(new XcesReaderImpl(*this, chunk_buf_, disamb_only, disamb_sh))
{
	this->is_ = &is;
}

XcesReader::XcesReader(const Tagset& tagset, const std::string& filename, bool disamb_only, bool disamb_sh)
	: BufferedChunkReader(tagset),
	impl_(new XcesReaderImpl(*this, chunk_buf_, disamb_only, disamb_sh))
{
	this->is_owned_.reset(new std::ifstream(filename.c_str(), std::ifstream::in));
	if (!this->is_owned_->good()) {
		throw FileNotFound(filename);
	}
	else {
		this->is_ = is_owned_.get();
	}
}

XcesReader::~XcesReader()
{
}

void XcesReader::ensure_more()
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

XcesReaderImpl::XcesReaderImpl(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf,
		bool disamb_only, bool disamb_sh)
	: XmlReader(base_reader, obuf)
{
	XmlReader::set_disamb_only(disamb_only);
	XmlReader::set_disamb_sh(disamb_sh);
	sentence_tag_name_ = "chunk";
}

XcesReaderImpl::~XcesReaderImpl()
{
}

void XcesReader::set_option(const std::string& option)
{
	if (option == "no_warn_inconsistent") {
		impl_->set_warn_on_inconsistent(false);
	} else if (option == "sh") {
		impl_->set_disamb_sh(true);
	} else if (option == "disamb_only") {
		impl_->set_disamb_only(true);
	} else {
		BufferedChunkReader::set_option(option);
	}
}

std::string XcesReader::get_option(const std::string& option) const
{
	if (option == "sh") {
		return impl_->get_disamb_sh() ? option : "";
	} else if (option == "disamb_only") {
		return impl_->get_disamb_only() ? option : "";
	} else if (option == "no_warn_inconsistent") {
		return impl_->get_warn_on_inconsistent() ? option : "";
	}
	return BufferedChunkReader::get_option(option);
}

} /* end ns Corpus2 */
