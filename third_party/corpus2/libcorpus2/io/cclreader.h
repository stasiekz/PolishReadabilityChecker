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

#ifndef LIBCORPUS2_IO_CCLREADER_H
#define LIBCORPUS2_IO_CCLREADER_H

#include <libcorpus2/io/reader.h>
#include <libcorpus2/io/xces.h>
#include <libcorpus2/chunk.h>
#include <deque>
#include <boost/scoped_ptr.hpp>
#include <libcorpus2/io/xmlreader.h>
#include <libxml++/libxml++.h>
#include <libcorpus2/ann/annotatedsentence.h>

#define READ_BUFFER_SIZE 4096


namespace Corpus2 {

class CclReaderImpl : public XmlReader
{
public:
	CclReaderImpl(const TokenReader& base_reader,
		std::deque< boost::shared_ptr<Chunk> >& obuf,
		bool disamb_only, bool disamb_sh);

	~CclReaderImpl();

protected:
	bool process_start_element(const Glib::ustring & name,
		const AttributeList& attributes);

	bool process_end_element(const Glib::ustring& name);

	void start_chunk(const AttributeList &attributes);

	void start_sentence(const AttributeList &attributes);

	void start_token(const AttributeList &attributes);

	void finish_token();

	static const int STATE_ANN = 901;
	static const int STATE_REL = 902; // currently unused
	static const int STATE_PROP = 910;

	boost::shared_ptr<AnnotatedSentence> ann_sent_;

	std::string ann_chan_;

	std::string prop_key_;

	bool ann_head_;

	typedef std::map<std::string, int> token_ann_t;

	token_ann_t token_anns_;

	std::set<std::string> token_ann_heads_;
};


class CclReader : public BufferedChunkReader
{
public:
	CclReader(const Tagset& tagset, std::istream& is,
			bool disamb_only = false, bool disamb_sh = false);

	CclReader(const Tagset& tagset, const std::string& filename,
			bool disamb_only = false, bool disamb_sh = false);

	~CclReader();

	std::istream& is() {
		return *is_;
	}

	void set_option(const std::string& option);
	
	std::string get_option(const std::string& option) const;

	static bool registered;

protected:
	void ensure_more();

	// std::istream& is_;
	std::istream* is_;
    boost::scoped_ptr<std::ifstream> is_owned_;

	boost::scoped_ptr<CclReaderImpl> impl_;

    //ktagowski: Added for decompression
    void s_try_decompres(std::istream &is);
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_CCLREADER_H
