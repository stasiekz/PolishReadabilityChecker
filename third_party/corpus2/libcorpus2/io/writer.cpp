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

#include <libcorpus2/io/writer.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <libcorpus2/exception.h>
#include <libcorpus2/io/pathwriter.h>
#include <boost/make_shared.hpp>
#include <sstream>
#include <fstream>
#include "compressor.h"
#include <fstream>

namespace Corpus2 {
using namespace std;

TokenWriter::TokenWriter(std::ostream& os, const Tagset& tagset,
		const string_range_vector& /*params*/)
    : os_(os), tagset_(tagset), needs_footer_(true), indent_(0)
{
    pc_compressor = NULL;
}

TokenWriter::~TokenWriter()
{
    finish();
    if(pc_compressor!=NULL){
        delete pc_compressor;
        pc_compressor = NULL;
    }

}

void TokenWriter::finish()
{
    if (needs_footer_ == true){
        do_footer();
        needs_footer_ = false;
    }
}
stringstream& TokenWriter::ssi(){
   for (int i = 0; i < indent_; ++i) {
        sstream_ << " ";
    }
   return sstream_;
}

std::ostream& TokenWriter::osi()
{
    for (int i = 0; i < indent_; ++i) {
         os_ << " ";
     }
    return os_;
}


void TokenWriter::indent_more(int n)
{
	indent_ += n;
}

void TokenWriter::indent_less(int n)
{
	indent_ -= n;
}

void TokenWriter::write_token_dispose(Token* t)
{
	write_token(*t);
	delete t;
}


boost::shared_ptr<TokenWriter> TokenWriter::create_stream_writer(const std::string& class_id,
		std::ostream& os,
		const Tagset& tagset,
		const string_range_vector& params)
{
	try {
		return boost::shared_ptr<TokenWriter>(
			detail::TokenWriterFactorySingleton::Instance().factory.CreateObject(
				class_id, os, tagset, params));
	} catch (detail::TokenWriterFactoryException&) {
		throw Corpus2Error("Writer class not found: " + class_id);
	}
}

boost::shared_ptr<TokenWriter> TokenWriter::create_stream_writer(
		const std::string& class_id_params,
		std::ostream& os,
		const Tagset& tagset)
{
	string_range_vector params;
	boost::algorithm::split(params, class_id_params,
							boost::is_any_of(std::string(",")));
	std::string class_id = boost::copy_range<std::string>(params[0]);
	params.erase(params.begin(), params.begin() + 1);
	return create_stream_writer(class_id, os, tagset, params);
}

boost::shared_ptr<TokenWriter> TokenWriter::create_path_writer(const std::string& class_id,
		const std::string& path,
		const Tagset& tagset,
		const string_range_vector& params)
{
	boost::shared_ptr<std::ofstream> ofs = boost::make_shared<std::ofstream>(path.c_str());
	if (!ofs->good()) {
		throw Corpus2::FileNotFound(path, "", "writer creation");
	}
	boost::shared_ptr<TokenWriter> underlying = create_stream_writer(
			class_id, *ofs, tagset, params);
	assert(underlying);
	boost::shared_ptr<TokenWriter> wrapped = boost::make_shared<PathWriter>(
			underlying, ofs);
	return wrapped;
}

boost::shared_ptr<TokenWriter> TokenWriter::create_path_writer(
		const std::string& class_id_params,
		const std::string& path,
		const Tagset& tagset)
{
	string_range_vector params;
	boost::algorithm::split(params, class_id_params,
							boost::is_any_of(std::string(",")));
	std::string class_id = boost::copy_range<std::string>(params[0]);
	params.erase(params.begin(), params.begin() + 1);
	return create_path_writer(class_id, path, tagset, params);
}

std::vector<std::string> TokenWriter::available_writer_types()
{
	return detail::TokenWriterFactorySingleton::Instance().factory.RegisteredIds();
}

std::string TokenWriter::writer_help(const std::string& class_id)
{
	std::map<std::string, std::string>::const_iterator c;
	c = detail::TokenWriterFactorySingleton::Instance().help.find(class_id);
	if (c != detail::TokenWriterFactorySingleton::Instance().help.end()) {
		return c->second;
	} else {
		return "";
	}
}

std::vector<std::string> TokenWriter::available_writer_types_help()
{
	std::vector<std::string> v = available_writer_types();
	BOOST_FOREACH(std::string& id, v) {
		std::stringstream ss;
		std::map<std::string, std::string>::const_iterator c;
		c = detail::TokenWriterFactorySingleton::Instance().help.find(id);
		if (c != detail::TokenWriterFactorySingleton::Instance().help.end()) {
			ss << id << "[";
			ss << c->second;
			ss << "]";
		}
		id = ss.str();
	}
	return v;
}

} /* end ns Corpus2 */
