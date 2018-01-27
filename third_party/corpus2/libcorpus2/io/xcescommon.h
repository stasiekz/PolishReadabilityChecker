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

#ifndef LIBCORPUS2_IO_XCESCOMMON_H
#define LIBCORPUS2_IO_XCESCOMMON_H

#include <libcorpus2/token.h>

namespace Corpus2 {

/**
 * Output the token's XCES XML representation, at the requested indent
 * level. Use -1 to disable indentation.
 */
void token_as_xces_xml(std::ostream& os, const Tagset& tagset,
		const Token& t, int indent, bool output_disamb = false,
		bool sort = false, bool whitespace_info = false);

void token_as_xces_xml_head(std::ostream& os,
		const Token& t, int indent, bool whitespace_info /* false */);

void token_as_xces_xml_body(std::ostream& os, const Tagset& tagset,
		const Token& t, int indent, bool output_disamb /* = false */,
		bool sort /* = false */);
/**
 * Output a xml-encoded version of the given string into the given ostream.
 * The default XML entity substitutions are made: less than, greater than,
 * ampersand, single quote and double quote.
 */
void encode_xml_entities_into(std::ostream& buf, const std::string& input);

/**
 * In-place XML entity encoding.
 * The default XML entity substitutions are made: less than, greater than,
 * ampersand, single quote and double quote.
 */
void encode_xml_entities(std::string &input);

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_XCESCOMMON_H
