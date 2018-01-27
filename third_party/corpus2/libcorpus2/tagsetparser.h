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

#ifndef LIBCORPUS2_TAGSETPARSER_H
#define LIBCORPUS2_TAGSETPARSER_H

#include <libcorpus2/tagset.h>

namespace Corpus2 {

class TagsetParseError : public Corpus2Error
{
public:
	TagsetParseError(const std::string& w, int line,
			const std::string& data)
		: Corpus2Error("Tagset parse error: " + w), line(line), data(data)
	{
	}

	~TagsetParseError() throw()
	{
	}

	virtual std::string info() const;

	int line;
	std::string data;
};

class TagsetParser
{
public:
	static Tagset load_ini(const std::string& filename);

	static Tagset load_ini(std::istream& is);

	static void save_ini(const Tagset& tagset,
			const std::string& filename);

	static void save_ini(const Tagset& tagset, std::ostream& os);

};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TAGSETPARSER_H
