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

#include <libcorpus2/tag.h>
#include <libcorpus2/tagsetmanager.h>

#include <boost/foreach.hpp>
#include <libpwrutils/util.h>

#include <cstring>
#include <sstream>

#include <boost/functional/hash.hpp>

#include <bitset>

namespace Corpus2 {

int Tag::pos_count() const
{
	return PwrNlp::count_bits_set(pos_);
}

int Tag::get_pos_index() const
{
	if (pos_.none()) return -1;
	return PwrNlp::lowest_bit(pos_);
}

std::string Tag::raw_dump() const
{
	std::ostringstream ss;
	ss << "[";
	std::bitset<sizeof(mask_t) * CHAR_BIT> binaryp(pos_);
	std::bitset<sizeof(mask_t) * CHAR_BIT> binaryv(values_);
	//ss << static_cast<int>(tagset_id_);
	ss << "" << pos_;
	ss << ":" << values_;
	ss << "]";
	return ss.str();
}

bool Tag::operator<(const Tag& other) const
{
	return pos_ < other.pos_ ||
		(pos_ == other.pos_ &&
		 values_ < other.values_);
}

bool Tag::operator ==(const Tag& other) const
{
	return pos_ == other.pos_ && values_ == other.values_;
}

size_t hash_value(const Tag& tag)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, tag.get_pos());
	boost::hash_combine(seed, tag.get_values());
	return seed;
}

const Tag Tag::all_pos_mask(filled_mask);

} /* end ns Corpus2 */
