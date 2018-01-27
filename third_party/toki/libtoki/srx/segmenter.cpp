/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/srx/segmenter.h>
#include <libtoki/srx/exception.h>
#include <boost/foreach.hpp>
#include <libpwrutils/util.h>

#include <map>
#include <iostream>
#include <sstream>

namespace Toki { namespace Srx {

Segmenter::Segmenter()
{
}

Segmenter::~Segmenter()
{
}

std::vector<bool> Segmenter::get_break_mask() const {
	std::vector<bool> breaks(length_);
	BOOST_FOREACH (break_map_t::value_type v, break_map_) {
		breaks[v.first] = v.second;
	}
	return breaks;
}

std::vector<int> Segmenter::get_break_positions() const {
	std::vector<int> breaks;
	BOOST_FOREACH (break_map_t::value_type v, break_map_) {
		if (v.second) {
			breaks.push_back(v.first);
		}
	}
	return breaks;
}

Segmenter* Segmenter::get_segmenter_by_name(const std::string &name)
{
	if (name == "icu") return new NaiveIcuSegmenter;
#ifdef HAVE_BOOST_REGEX
	if (name == "boost") return new NaiveBoostSegmenter;
#endif
	if (name == "icu-hxo") return new HxoIcuSegmenter;
	return NULL;
}

NaiveIcuSegmenter::NaiveIcuSegmenter()
{
}

NaiveIcuSegmenter::~NaiveIcuSegmenter()
{
	BOOST_FOREACH (const CompiledRule& cr, crules_) {
		delete cr.matcher;
	}
}

void NaiveIcuSegmenter::load_rules(const std::vector<Rule>& rules)
{
	BOOST_FOREACH (const Rule& r, rules) {
		UErrorCode status = U_ZERO_ERROR;
		CompiledRule cr = r.compile(status);
		if (U_SUCCESS(status)) {
			crules_.push_back(cr);
		} else {
			std::stringstream ss;
			ss << r.before << " : " << r.after;
			throw SrxError("Rule failed to compile: " + ss.str());
		}
	}
}

void NaiveIcuSegmenter::compute_breaks(const UnicodeString& str, int from,
		int to)
{
	//std::cerr << "SRXP run from " << from << " to " << to
	//		<< " on [" << Util::to_utf8(str) << "] :";
	break_map_.clear();
	to -= from;
	length_ = to;
	BOOST_FOREACH (const CompiledRule& cr, crules_) {
		UErrorCode ue = U_ZERO_ERROR;
		int i = 0;
		cr.matcher->reset(str);
		while (cr.matcher->find(i, ue)) {
			UErrorCode status = U_ZERO_ERROR;
			int n = cr.matcher->end(1, status);
			n -= from;
			if (n >= 0) {
				if (n < to) {
					break_map_t::value_type v(n, cr.breaks);
					 //only insert if the index was not in the map
					break_map_.insert(v);
				} else {
					break;
				}
			}
			i = cr.matcher->start(status) + 1;
		}
	}
	//std::vector<int> b = get_break_positions();
	//for (size_t i = 0; i < b.size(); ++i) {
	//	std::cerr << b[i] << " ";
	//}
	//std::cerr << " <-- \n";
}


HxoIcuSegmenter::HxoIcuSegmenter()
{
}

HxoIcuSegmenter::~HxoIcuSegmenter()
{
	BOOST_FOREACH (RegexMatcher* r, nobreak_back_) {
		delete r;
	}
	BOOST_FOREACH (RegexMatcher* r, nobreak_fwd_) {
		delete r;
	}
	BOOST_FOREACH (CompiledRule& r, crules_break_) {
		delete r.matcher;
	}
}

void HxoIcuSegmenter::load_rules(const std::vector<Rule> &rules)
{
	BOOST_FOREACH (const Rule& r, rules) {
		if (r.breaks) {
			UErrorCode status = U_ZERO_ERROR;
			CompiledRule cr = r.compile(status);
			if (U_SUCCESS(status)) {
				crules_break_.push_back(cr);
				break_exception_idx_.push_back(nobreak_back_.size());
			} else {
				std::stringstream ss;
				ss << r.before << " : " << r.after;
				throw SrxError("Rule failed to compile: " + ss.str());
			}
		} else {
			UErrorCode status = U_ZERO_ERROR;
			RegexMatcher *b, *a;
			std::string mod_before = r.before + "$";
			b = new RegexMatcher(UnicodeString::fromUTF8(mod_before), 0,
					status);
			if (!U_SUCCESS(status)) {
				throw SrxError("BeforeRule failed to compile: " + mod_before);
			}
			//std::cerr << "XXX" << r.after << "\n";
			if (r.after.empty()) {
				a = NULL;
			} else {
				a = new RegexMatcher(UnicodeString::fromUTF8(r.after), 0,
					status);
				if (!U_SUCCESS(status)) {
					throw SrxError("AfterRule failed to compile: " + r.after);
				}
			}
			nobreak_back_.push_back(b);
			nobreak_fwd_.push_back(a);
		}

	}
}

void HxoIcuSegmenter::compute_breaks(const UnicodeString &str, int from,
		int to)
{
	break_map_.clear();
	to -= from;
	length_ = to;
	for (size_t bri = 0; bri < crules_break_.size(); ++bri) {
		CompiledRule& cr = crules_break_[bri];
		UErrorCode ue = U_ZERO_ERROR;
		int i = 0;
		cr.matcher->reset(str);
		while (cr.matcher->find(i, ue)) {
			UErrorCode status = U_ZERO_ERROR;
			int n = cr.matcher->end(1, status);
			// n is where we found the break position
			//std::cerr << "Break match at " << n << "\n";
			int n_out = n - from;
			if (n_out >= 0 && n_out < to) {
				// check if anything matched there already
				if (break_map_.find(n_out) == break_map_.end()) {
					//std::cerr << "Break match promising\n";
					// now we need to check if there are any nobreak rules
					// that match here and appear earlier
					UnicodeString h(false, str.getBuffer(), n);
					bool nobreak_matched = false;
					for (size_t ri = 0; ri < break_exception_idx_[bri];
							++ri) {
						//std::cerr << "Checking nobreak rule "
						//	<< ri << "\n";
						//std::cerr << "'" << Util::to_utf8(h) << "'\n";
						//std::cerr << "'" << Util::to_utf8(
						//		UnicodeString(str.getBuffer()+n)) << "'\n";
						// check the "after" pattern
						RegexMatcher* arm = nobreak_fwd_[ri];
						if (arm) {
							arm->reset(str);
						}
						if (!arm || (arm->find(n, status)
							&& arm->start(status) == n)) {
							//std::cerr << "After pattern match\n";
							// after pattern matches
							// need to check the before pattern
							RegexMatcher& brm = *nobreak_back_[ri];
							brm.reset(h);
							if (brm.find()) {
								//std::cerr << "Before pattern match\n";
								//nobreak rule matched fully
								nobreak_matched = true;
								break;
							}
						}
					}
					//std::cerr << "Inserting break at " << n << " "
					// << !nobreak_matched << "\n";
					break_map_t::value_type v(n_out, !nobreak_matched);
					//only insert if the index was not in the map
					break_map_.insert(v);
				}
			}
			i = cr.matcher->start(status) + 1;
		}
	}
}


#ifdef HAVE_BOOST_REGEX
NaiveBoostSegmenter::NaiveBoostSegmenter()
{
}

void NaiveBoostSegmenter::load_rules(const std::vector<Rule>& rules)
{
	std::pair<boost::u32regex, bool> cr;
	BOOST_FOREACH (const Rule& r, rules) {
		cr.first = boost::make_u32regex(r.create_pattern());
		cr.second = r.breaks;
		crules_.push_back(cr);
	}
}

void NaiveBoostSegmenter::compute_breaks(const UnicodeString& str,
		int from, int to)
{
	//std::cerr << "SRXP run from " << from << " to " << to
	//		<< " on [" << Util::to_utf8(str) << "] :";
	break_map_.clear();
	to -= from;
	length_ = to;

	typedef std::pair<boost::u32regex, bool> cr_t;
	BOOST_FOREACH (const cr_t& cr, crules_) {
		boost::match_results<const UChar*> what;
		const UChar* start = str.getBuffer();
		const UChar* end = start + str.length();
		boost::match_flag_type flags = boost::match_default;
		while (boost::u32regex_search(start, end, what, cr.first, flags)) {
			int n = what[1].second - str.getBuffer();
			n -= from;
			if (n >= 0) {
				if (n < to) {
					break_map_t::value_type v(n, cr.second);
					//only insert if the index was not in the map
					break_map_.insert(v);
				} else {
					break;
				}
			}
			start = what[0].first + 1; // sad panda
			flags |= boost::match_prev_avail;
			flags |= boost::match_not_bob;
		}
	}
	//std::vector<int> b = get_break_positions();
	//for (size_t i = 0; i < b.size(); ++i) {
	//	std::cerr << b[i] << " ";
	//}
	//std::cerr << "\n";
}
#endif

} /* end ns Srx */
} /* end ns Toki */
