/*
        Copyright (C) 2010 Tomasz Åniatowski, Adam Radziszewski, PaweÅ KÄdzia
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


#ifndef LIBCORPUS2_WHOLE_RELWRITER_H
#define LIBCORPUS2_WHOLE_RELWRITER_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include <libcorpus2_whole/relation.h>

#include <iostream>

namespace Corpus2 {
namespace whole {

	
/**
 * A writer for realtion documents. 
 */
class RelationWriter {
public:
	/**
	 * Writes a document with relations
	 * @param rela_path  path to file with relations
	 */
	RelationWriter(const std::string &rela_path);


	/**
	 * Writes given vector of Relations to the file specified before
	 */
	void write(const std::vector< boost::shared_ptr<Relation> >& relations);


private:

	/// Path to file with relations
	std::string rela_path_;


};

} // whole ns
} // Corpus2 ns

#endif // LIBCORPUS2_WHOLE_RELREADER_H

