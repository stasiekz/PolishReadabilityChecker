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

#ifndef LIBCORPUS2_TAGSETMANAGER_H
#define LIBCORPUS2_TAGSETMANAGER_H

#include <libcorpus2/tagset.h>
#include <libcorpus2/exception.h>

#include <boost/shared_ptr.hpp>
#include <loki/Singleton.h>

namespace Corpus2 {

/**
 * Exception class for signalling cache errors
 */
class TagsetNotFound : public Corpus2Error
{
public:
	/// Constructor
	explicit TagsetNotFound(const tagset_idx_t id);

	/// Destructor
	~TagsetNotFound() throw() {}

	/// Error override
	std::string info() const;

	/// The invalid tagset id
	tagset_idx_t id;
};

/**
 * Singleton class for accesing tagsets by name that will load tagsets
 * from the data dir and cache them for future acceses.
 * @todo this should be a generic utility class
 */
class TagsetManager : private boost::noncopyable
{
public:
	TagsetManager();

	~TagsetManager();

	const Tagset& get_tagset(const std::string& name);

	const Tagset& get_tagset(tagset_idx_t id);

	boost::shared_ptr<Tagset> get_cache_entry(const std::string& name);

	boost::shared_ptr<Tagset> get_cache_entry(tagset_idx_t id);

private:
	typedef std::map<std::string, boost::shared_ptr<Tagset> > cache_t;
	cache_t cache_;
	typedef std::map<tagset_idx_t, boost::shared_ptr<Tagset> > id_cache_t;
	id_cache_t id_cache_;
};

typedef Loki::SingletonHolder< TagsetManager > TagsetManagerSingleton;

inline const Tagset& get_named_tagset(const std::string& name) {
	return TagsetManagerSingleton::Instance().get_tagset(name);
}

inline const Tagset& get_named_tagset(tagset_idx_t id) {
	return TagsetManagerSingleton::Instance().get_tagset(id);
}

std::string available_tagsets();

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TAGSETMANAGER_H
