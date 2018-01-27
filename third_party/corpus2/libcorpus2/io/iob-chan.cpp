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

#include <libcorpus2/io/iob-chan.h>
#include <boost/foreach.hpp>
#include <libcorpus2/ann/annotatedsentence.h>

#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <fstream>
#include <boost/algorithm/string/split.hpp>

namespace Corpus2 {

bool IobChanWriter::registered = TokenWriter::register_writer<IobChanWriter>(
		"iob-chan", "nowarn,noforce");

IobChanWriter::IobChanWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
	: TokenWriter(os, tagset, params), warn_on_no_lexemes_(true), force_(true)
{
	BOOST_FOREACH(const string_range& param, params) {
		std::string p = boost::copy_range<std::string>(param);
		if (p == "nowarn") {
			warn_on_no_lexemes_ = false;
		} else if (p == "noforce") {
			force_ = false;
		}
	}
}

void IobChanWriter::write_token(const Token& t)
{
	os() << t.orth_utf8();
	if (t.lexemes().empty()) {
		if (warn_on_no_lexemes_) {
			std::cerr << "No lexemes for token!";
		}
	} else {
		const Lexeme& pref = t.get_preferred_lexeme(tagset());
		os() << "\t";
		write_tag(pref.tag());
	}
	os() << "\n";
}

void IobChanWriter::write_sentence(const Sentence& s)
{
	const AnnotatedSentence* ann = dynamic_cast<const AnnotatedSentence*>(&s);
	if (ann && force_) {
		// I sincerely apologize
		AnnotatedSentence* hax = const_cast<AnnotatedSentence*>(ann);
		BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& v, hax->all_channels()) {
			hax->get_channel(v.first).make_iob_from_segments();
		}
	}
	for (size_t idx = 0; idx < s.size(); ++idx) {
		const Token* t = s.tokens()[idx];
		os() << t->orth_utf8();
		if (t->lexemes().empty()) {
			if (warn_on_no_lexemes_) {
				std::cerr << "No lexemes for token!";
			}
		} else {
			const Lexeme& pref = t->get_preferred_lexeme(tagset());
			os() << "\t";
			os() << pref.lemma_utf8();
			os() << "\t";
			write_tag(pref.tag());
			os() << "\t";
		}
		if (ann) {
			bool first = true;
			BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& v, ann->all_channels()) {
				if (!first) {
					os() << ",";
				}
				os() << v.first << "-";
				os() << Corpus2::IOB::to_string(v.second.get_iob_at(idx));
				first = false;
			}
		}
		os() << "\n";
	}
	os() << "\n";
}

void IobChanWriter::write_chunk(const Chunk& c)
{
	BOOST_FOREACH(const Sentence::ConstPtr& s, c.sentences()) {
		write_sentence(*s);
	}
}

void IobChanWriter::write_tag(const Tag& tag)
{
	os() << tagset().tag_to_string(tag);
}


bool IobChanReader::registered = TokenReader::register_reader<IobChanReader>("iob-chan",
	"ign,loose,strict,no_set_disamb");


IobChanReader::IobChanReader(const Tagset& tagset, std::istream& is)
	: BufferedSentenceReader(tagset), is_(&is), disamb_(true)
{
}

IobChanReader::IobChanReader(const Tagset& tagset, const std::string& filename)
	: BufferedSentenceReader(tagset), is_(), disamb_(true)
{
	is_owned_.reset(new std::ifstream(filename.c_str(), std::ifstream::in));
	if (!this->is_owned_->good()) {
		throw FileNotFound(filename);
	}
	else {
		this->is_ = is_owned_.get();
	}
}

Sentence::Ptr IobChanReader::actual_next_sentence()
{
	std::string line;
	boost::shared_ptr<AnnotatedSentence> s;
	typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;

	while (is().good()) {
		std::getline(is(), line);
		if (line.empty()) {
			break;
		}
		std::vector<std::string> spl;
		boost::algorithm::split(spl, line, boost::is_any_of("\t"));
		if (spl.size() != 3 and spl.size() != 4) {
			std::cerr << "Invalid line: " << line << "(" << spl.size() << ")\n";
		} else {
			const std::string& orth = spl[0];
			const std::string& lemma = spl[1];
			const std::string& tag_string = spl[2];
			// if no annotations, let anns = ""
			const std::string& anns = (spl.size() == 4) ? spl[3] : "";
			Tag tag = parse_tag(tag_string);
			Token* t = new Token();
			t->set_orth(UnicodeString::fromUTF8(orth));
			t->set_wa(PwrNlp::Whitespace::Space);
			t->add_lexeme(Lexeme(UnicodeString::fromUTF8(lemma), tag));
			if (disamb_) {
				t->lexemes().back().set_disamb(true);
			}
			if (!s) {
				s = boost::make_shared<AnnotatedSentence>();
				t->set_wa(PwrNlp::Whitespace::Newline);
			}
			s->append(t);
			if (!anns.empty()) {
				std::vector<std::string> annsplit;
				boost::algorithm::split(annsplit, anns, boost::is_any_of(","));
				BOOST_FOREACH(const std::string& a, annsplit) {
					std::vector<std::string> one_ann_split;
					boost::algorithm::split(one_ann_split, a, boost::is_any_of("-"));
					if (one_ann_split.size() != 2) {
						std::cerr << "Invalid annotation:" << a << "\n";
					} else {
						const std::string& aname = one_ann_split[0];
						const std::string& aiob = one_ann_split[1];
						Corpus2::IOB::Enum iob = Corpus2::IOB::from_string(aiob);
						if (iob == Corpus2::IOB::PostLast) {
							std::cerr << "Invalid IOB tag: " << aiob << "\n";
						} else {
							if (!s->has_channel(aname)) {
								s->create_channel(aname);
							}
							s->get_channel(aname).set_iob_at(s->size() - 1, iob);
						}
					}
				}
			}
		}
	}
	if (s) {
		BOOST_FOREACH(const AnnotatedSentence::chan_map_t::value_type& v, s->all_channels()) {
			s->get_channel(v.first).make_segments_from_iob();
		}
	}
	return s;
}

void IobChanReader::set_option(const std::string &option)
{
	if (option == "no_set_disamb") {
		disamb_ = false;
	} else {
		BufferedSentenceReader::set_option(option);
	}
}

std::string IobChanReader::get_option(const std::string &option) const
{
	if (option == "no_set_disamb") {
		return !disamb_ ? option : "";
	}
	return BufferedSentenceReader::get_option(option);
}

} /* end ns Corpus2 */
