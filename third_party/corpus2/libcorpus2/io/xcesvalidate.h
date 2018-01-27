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

#ifndef LIBCORPUS2_IO_XCESVALIDATE_H
#define LIBCORPUS2_IO_XCESVALIDATE_H

#include <boost/scoped_ptr.hpp>
#include <libcorpus2/tagset.h>
#include <libcorpus2/tokensource.h>

namespace Corpus2 {

class XcesValidatorImpl;

class XcesValidator
{
public:
	XcesValidator(const Tagset& tagset, std::ostream& out);

	~XcesValidator();

	void validate_stream(std::istream& is);

	void validate_file(const std::string& filename);

protected:
	boost::scoped_ptr<XcesValidatorImpl> impl_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_XCESVALIDATE_H
