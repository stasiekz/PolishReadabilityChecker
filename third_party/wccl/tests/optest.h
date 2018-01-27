/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBWCCL_TESTS_OPTEST_H
#define LIBWCCL_TESTS_OPTEST_H

#include <libwccl/ops/functions/constant.h>
#include <libwccl/ops/function.h>
#include <libwccl/values/position.h>


namespace Wccl {

class PositionFixture
{
public:
	PositionFixture(int max_offset = 1);

	int max_offset() { return max_offset(); }

	Position begin_value() { return positions_[0]; }
	Position end_value() { return positions_[1]; }
	Position nowhere_value() { return positions_[2]; }
	Position pos_value(int offset) {
		return positions_[get_idx(offset)];
	}

	boost::shared_ptr< Wccl::Function<Position> > begin() { return constants_[0]; }
	boost::shared_ptr< Wccl::Function<Position> > end() { return constants_[1]; }
	boost::shared_ptr< Wccl::Function<Position> > nowhere() { return constants_[2]; }
	boost::shared_ptr< Wccl::Function<Position> > pos(int offset) {
		return constants_[get_idx(offset)];
	}

protected:

private:
	int get_idx(int offset) {
		assert(offset <= max_offset_);
		assert(offset >= -max_offset_);
		int idx = offset + max_offset_ + 3;
		assert(idx >= 0);
		assert(static_cast<size_t>(idx) < positions_.size());
		return idx;
	}

	int max_offset_;
	std::vector<Position> positions_;
	std::vector< boost::shared_ptr< Wccl::Function<Position> > > constants_;
};


} /* end ns Wccl */

#endif // LIBWCCL_TESTS_OPTEST_H
