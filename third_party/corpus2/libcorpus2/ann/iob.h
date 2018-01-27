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

#ifndef LIBCORPUS2_ANN_IOB_H
#define LIBCORPUS2_ANN_IOB_H

#include <string>

namespace Corpus2 {

namespace IOB {
	/**
	 * A simple enumeration for IOB annotation chunk tagging
	 * O indicates not part of a chunk
	 * B indicates beginning of a new chunk
	 * I indicates continuation of a chunk started by a preceeding BI* sequence
	 * The only invalid sequence is O followed by I
	 */
	enum Enum {
		O = 0,
		B = 1,
		I = 2,
		PostLast
	};
	/// Convert an enum value to an uppercase I, O or B string
	/// Returns ? on invalid value
	const char* to_string(Enum iob);
	/// Create an enum value from an I, O or B string
	Enum from_string(const std::string& s);
}

} /* end ns Corpus2 */

#endif // LIBCORPUS2_ANN_IOB_H
