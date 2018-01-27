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

#include <libcorpus2/ann/iob.h>

namespace Corpus2 {

const char* IOB::to_string(IOB::Enum iob)
{
	if (iob == IOB::I) return "I";
	if (iob == IOB::O) return "O";
	if (iob == IOB::B) return "B";
	return "?";
}

IOB::Enum IOB::from_string(const std::string &s)
{
	if (s == "I") return IOB::I;
	if (s == "O") return IOB::O;
	if (s == "B") return IOB::B;
	return IOB::PostLast;
}

} /* end ns Corpus2 */
