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

#include <boost/foreach.hpp>
#include <libtoki/layers/init.h>
#include <libtoki/layers/tokenlayer.h>
#include <libtoki/token.h>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Toki {

TokenLayer::TokenLayer(TokenSource* input, const Config::Node& props)
	: input_(input), process_token_types_(), do_not_process_token_types_(),
	id_(), error_stream_(NULL)
{
	const std::string sep = ", ";
	std::vector<std::string> sv;
	std::string data = props.get("process_types", "");
	boost::algorithm::split(sv, data, boost::algorithm::is_any_of(sep));
	BOOST_FOREACH (const std::string& s, sv) {
		if (!s.empty()) {
			process_token_types_.insert(s);
		}
	}
	std::vector<std::string> sv2;
	std::string data2 = props.get("ignore_types", "");
	boost::algorithm::split(sv2, data2, boost::algorithm::is_any_of(sep));
	BOOST_FOREACH (const std::string& s2, sv2) {
		if (!s2.empty()) {
			do_not_process_token_types_.insert(s2);
		}
	}
	id_ = props.get("id", "?");
}

TokenLayer::~TokenLayer()
{
}

void TokenLayer::restart()
{
}

std::string TokenLayer::info() const
{
	return "tokenlayer";
}

std::string TokenLayer::long_info() const
{
	return "Layer '" + id_ + "'";
}

TokenLayer* TokenLayer::create(std::string class_id, TokenSource* input,
		const Config::Node& props)
{
	return TokenLayerFactory::Instance().CreateObject(
			class_id, input, props);
}

std::vector<std::string> TokenLayer::available_layer_types()
{
	return TokenLayerFactory::Instance().RegisteredIds();
}

Token* TokenLayer::get_token_from_input()
{
	return input_->get_next_token();
}

Token* TokenLayer::get_next_token()
{
	Token* t = get_token_from_input();
	if (t) {
		if (should_process_token_type(t->type())) {
			t =  process_token(t);
		}
	}
	return t;
}

Token* TokenLayer::process_token(Token *t)
{
	return t;
}

bool TokenLayer::should_process_token_type(const std::string &t)
{
	if (do_not_process_token_types_.find(t) !=
			do_not_process_token_types_.end()) {
		return false;
	} else {
		if (process_token_types_.empty()) {
			return true;
		} else {
			return process_token_types_.find(t) !=
					process_token_types_.end();
		}
	}
}

void TokenLayer::set_error_stream(std::ostream *os)
{
	error_stream_ = os;
}

static bool registered = init_token_layers();

} /* end namespace Toki */
