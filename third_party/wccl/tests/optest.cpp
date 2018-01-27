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

#include "optest.h"

namespace Wccl {

PositionFixture::PositionFixture(int max_offset /*=1*/)
	: max_offset_(max_offset)
{
	assert(max_offset >= 0);
	positions_.push_back(Position(Position::Begin));
	positions_.push_back(Position(Position::End));
	positions_.push_back(Position(Position::Nowhere));
	for (int i = -max_offset; i <= max_offset; ++i) {
		positions_.push_back(Position(i));
	}
	for (size_t i = 0; i < positions_.size(); ++i) {
		constants_.push_back(
			boost::make_shared< Constant<Position> >(positions_[i]));
	}
}

} /* end ns Wccl */
