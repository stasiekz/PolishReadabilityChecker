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

#ifndef LIBCORPUS2_EXCEPTION_H
#define LIBCORPUS2_EXCEPTION_H

#include <libpwrutils/exception.h>

namespace Corpus2 {

/**
 * Base class for all Corpus2 errors. Derives from
 * @c PwrNlp::Error. Call member function @c what to get a
 * human-readable message associated with the error.
 */
class Corpus2Error : public PwrNlp::PwrNlpError
{
public:
	/**
	 * Instantiate an Error instance with the given message.
	 * @param what The message to associate with this error.
	 */
	Corpus2Error(const std::string &what);

	~Corpus2Error() throw();

	/// PwrNlpError override
	std::string scope() const;
};

class FileNotFound : public Corpus2Error
{
public:
	FileNotFound(const std::string& filename, const std::string& paths = "",
			const std::string& where = "");

	~FileNotFound() throw();

	std::string info() const;

	std::string filename, paths, where;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_EXCEPTION_H
