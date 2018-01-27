/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#include <libmaca/io/premorph.h>
#include <libcorpus2/io/sax.h>
#include <libcorpus2/io/xces.h>
#include <libcorpus2/io/xcescommon.h>
#include <libmaca/util/sentenceanalyser.h>
#include <libmaca/util/tokentimer.h>
#include <boost/foreach.hpp>
#include <libtoki/sentencesplitter.h>
#include <vector>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <fstream>
#include <iostream>

namespace Maca {

class PremorphProcessorImpl : public Corpus2::BasicSaxParser
{
public:
	PremorphProcessorImpl(std::ostream& os,
			const boost::shared_ptr<SentenceAnalyser>& sp);

	void set_stats(bool v) {
		stats_ = v;
	}

	void set_mark_sentences(bool v) {
		mark_sents_ = v;
	}

protected:
	void on_start_document();
	void on_end_document();
	void on_start_element(const Glib::ustring & name,
			const AttributeList& attributes);
	void on_end_element(const Glib::ustring & name);

	void output_sentence(const Corpus2::Sentence::Ptr& s);

private:
	std::ostream& os_;
	boost::shared_ptr<SentenceAnalyser> sa_;
	bool stats_;
	bool mark_sents_;
	TokenTimer timer_;
};

PremorphProcessor::PremorphProcessor(std::ostream &os,
		const boost::shared_ptr<Toki::Tokenizer>& tok,
		const boost::shared_ptr<Maca::MorphAnalyser>& ma)
: impl_(new PremorphProcessorImpl(os,
		boost::make_shared<SentenceAnalyser>(tok, ma)))
{
}

PremorphProcessor::PremorphProcessor(std::ostream &os,
		const boost::shared_ptr<SentenceAnalyser>& sa)
: impl_(new PremorphProcessorImpl(os, sa))
{
}

PremorphProcessor::~PremorphProcessor()
{
}

void PremorphProcessor::parse_stream(std::istream& is)
{
	impl_->parse_stream(is);
}

void PremorphProcessor::set_stats(bool v)
{
	impl_->set_stats(v);
}

void PremorphProcessor::set_mark_sentences(bool shall_mark_sents)
{
	impl_->set_mark_sentences(shall_mark_sents);
}

PremorphProcessorImpl::PremorphProcessorImpl(std::ostream& os,
		const boost::shared_ptr<SentenceAnalyser>& sa)
	: Corpus2::BasicSaxParser(), os_(os), sa_(sa),
	stats_(false), mark_sents_(true), timer_()
{
	grab_characters_ = true;
}

void PremorphProcessorImpl::on_start_document()
{
	timer_.restart();
	os_ << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	os_ << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
}

void PremorphProcessorImpl::on_end_document()
{
	if (stats_) {
		timer_.stats();
	}
}

void PremorphProcessorImpl::on_start_element(const Glib::ustring &name,
		const AttributeList &attributes)
{
	os_ << "<" << name;
	BOOST_FOREACH(const xmlpp::SaxParser::Attribute& a, attributes) {
		os_ << " " << a.name << "=\"";
		Corpus2::encode_xml_entities_into(os_, a.value);
		os_ << "\"";
	}
	os_ << ">";
	os_ << "\n";
}

void PremorphProcessorImpl::on_end_element(const Glib::ustring &name)
{
	sa_->set_input_source(UnicodeString::fromUTF8(buf_));
	sa_->process(boost::bind(
			&PremorphProcessorImpl::output_sentence, this, _1));
	clear_buf();
	os_ << "</" << name << ">" << "\n";
}

void PremorphProcessorImpl::output_sentence(const Corpus2::Sentence::Ptr& s)
{
	if (!s->empty()) {
		if (mark_sents_) {
			os_ << " <chunk type=\"s\">\n";
		}
		BOOST_FOREACH(Corpus2::Token* t, s->tokens()) {
			token_as_xces_xml(os_, sa_->tagset(), *t, 1);
		}
		if (mark_sents_) {
			os_ << " </chunk>\n";
		}
		timer_.count_sentence(*s);
		if (stats_) {
			timer_.check_slice();
		}
	}
}

class PremorphReaderImpl : public Corpus2::BasicSaxParserT<std::stringstream>
{
public:
	PremorphReaderImpl(const boost::shared_ptr<SentenceAnalyser>& sa,
		std::deque< boost::shared_ptr<Corpus2::Chunk> >& chunks);
protected:
	void on_start_element(const Glib::ustring & name,
			const AttributeList& attributes);
	void on_end_element(const Glib::ustring & name);
private:
	enum state_t { XS_NONE, XS_CHUNK, XS_SENTENCE, XS_TOK, XS_ORTH, XS_LEX,
			XS_LEMMA, XS_TAG };
	state_t state_;

	boost::shared_ptr<Corpus2::Chunk> chunk_;
	boost::shared_ptr<SentenceAnalyser> sa_;
	std::deque< boost::shared_ptr<Corpus2::Chunk> >& chunks_;
};

PremorphReader::PremorphReader(std::istream& is,
		const boost::shared_ptr<Toki::Tokenizer>& tok,
		const boost::shared_ptr<Maca::MorphAnalyser>& ma)
	: Corpus2::BufferedChunkReader(ma->tagset()), is_(is)
	, impl_(new PremorphReaderImpl(
			boost::make_shared<SentenceAnalyser>(tok, ma), chunk_buf_))
{
}

PremorphReader::PremorphReader(std::istream& is,
		const boost::shared_ptr<SentenceAnalyser>& sa)
	: BufferedChunkReader(sa->tagset()), is_(is)
	, impl_(new PremorphReaderImpl(sa, chunk_buf_))
{
}

PremorphReader::~PremorphReader()
{
}

void PremorphReader::ensure_more()
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

PremorphReaderImpl::PremorphReaderImpl(
		const boost::shared_ptr<SentenceAnalyser>& sa,
		std::deque< boost::shared_ptr<Corpus2::Chunk> > &chunks)
	: Corpus2::BasicSaxParserT<std::stringstream>(), state_(XS_NONE)
	, chunk_(), sa_(sa)
	, chunks_(chunks)
{
}

void PremorphReaderImpl::on_start_element(const Glib::ustring &name,
		const AttributeList &attributes)
{
	if (name == "chunk") {
		std::string type;
		BOOST_FOREACH(const Attribute& a, attributes) {
			if (a.name == "type") {
				type = a.value;
			}
		}
		if (state_ == XS_NONE) {
			if (type == "s") {
				throw Corpus2::XcesError("Top level <chunk> is type=\"s\"");
			}
			state_ = XS_CHUNK;
			chunk_ = boost::make_shared<Corpus2::Chunk>();
			BOOST_FOREACH(const Attribute& a, attributes) {
				chunk_->set_attribute(a.name, a.value);
			}
		} else if (state_ == XS_CHUNK) {
			throw Corpus2::XcesError("More than one level of <chunk>s in premorph");
		} else {
			throw Corpus2::XcesError("Unexpected <chunk>");
		}
	}
}

void PremorphReaderImpl::on_end_element(const Glib::ustring &name)
{
	if (state_ == XS_CHUNK && name == "chunk") {
		assert(chunk_);
		sa_->set_input_source(buf_);
		sa_->process(boost::bind(&Corpus2::Chunk::append, chunk_, _1));
		clear_buf();
		chunks_.push_back(chunk_);
		chunk_.reset();
		state_ = XS_NONE;
	}
}

PremorphTextReader::PremorphTextReader(boost::shared_ptr<std::ifstream> inputStream, const boost::shared_ptr<SentenceAnalyser>& sentenceAnalyser) : PremorphReader(boost::ref(*inputStream), sentenceAnalyser), inputFileStream(inputStream){
}
PremorphTextReader::~PremorphTextReader(){

}

boost::shared_ptr<Corpus2::TokenReader> PremorphTextReader::create_file_reader(const std::string& filename, const std::string& config){
    boost::shared_ptr<SentenceAnalyser> sentenceAnalyser = SentenceAnalyser::create_from_named_config(config);

    boost::shared_ptr<std::ifstream> is_ = boost::make_shared<std::ifstream>();
    is_->open(filename.c_str(), std::ios_base::in);

    return boost::make_shared<PremorphTextReader>(is_, sentenceAnalyser);
}

boost::shared_ptr<Corpus2::TokenReader> PremorphTextReader::create_stream_reader(const std::string& config){
    boost::shared_ptr<SentenceAnalyser> sentenceAnalyser = SentenceAnalyser::create_from_named_config(config);

    return boost::make_shared<PremorphReader>(boost::ref(std::cin), sentenceAnalyser);
}

} /* end ns Maca */
