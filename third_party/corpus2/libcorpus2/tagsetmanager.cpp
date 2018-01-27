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

#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/tagsetparser.h>
#include <libcorpus2/util/settings.h>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string/join.hpp>

namespace Corpus2 {

TagsetNotFound::TagsetNotFound(tagset_idx_t id)
	: Corpus2Error("Tagset not found")
	, id(id)
{
}
std::string TagsetNotFound::info() const
{
	std::ostringstream ss;
	ss << "Tagset with id " << (int)id << " not found in cache";
	return ss.str();
}

TagsetManager::TagsetManager()
	: cache_(), id_cache_()
{
}

TagsetManager::~TagsetManager()
{
}

const Tagset& TagsetManager::get_tagset(const std::string &name)
{
	boost::shared_ptr<Tagset> ptr = get_cache_entry(name);
	if (!ptr) {
		std::string fn = Path::Instance().find_file_or_throw(
				name + ".tagset", "tagset");
		std::ifstream ifs;
		ifs.open(fn.c_str());
		ptr.reset(new Tagset);
		*ptr = TagsetParser::load_ini(ifs);
		cache_.insert(std::make_pair(name, ptr));
		id_cache_.insert(std::make_pair(ptr->id(), ptr));
		ptr->set_name(name);
		if (Path::Instance().get_verbose()) {
			std::cerr << "Tagset '" << name << "' loaded with id "
				<< (int)ptr->id() << "\n";
		}
	}
	return *ptr;
}

const Tagset& TagsetManager::get_tagset(tagset_idx_t id)
{
	boost::shared_ptr<Tagset> ptr = get_cache_entry(id);
	if (!ptr) {
		throw TagsetNotFound(id);
	}
	return *ptr;
}

boost::shared_ptr<Tagset> TagsetManager::get_cache_entry(
		const std::string &name)
{
	cache_t::const_iterator i = cache_.find(name);
	if (i == cache_.end()) {
		return boost::shared_ptr<Tagset>();
	} else {
		return i->second;
	}
}

boost::shared_ptr<Tagset> TagsetManager::get_cache_entry(tagset_idx_t id)
{
	id_cache_t::const_iterator i = id_cache_.find(id);
	if (i == id_cache_.end()) {
		return boost::shared_ptr<Tagset>();
	} else {
		return i->second;
	}
}

std::string available_tagsets()
{
	return boost::algorithm::join(Path::Instance().list_files(".tagset"), " ");
}

} /* end ns Corpus2 */
