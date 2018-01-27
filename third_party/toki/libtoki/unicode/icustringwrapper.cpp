/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#include <libtoki/unicode/icustringwrapper.h>

namespace Toki {

UnicodeIcuStringWrapper::UnicodeIcuStringWrapper(const UnicodeString& u)
	: iter_(u)
{
}

UnicodeIcuStringWrapper::~UnicodeIcuStringWrapper()
{
}

UChar UnicodeIcuStringWrapper::peek_next_char()
{
	return iter_.current();
}

UChar UnicodeIcuStringWrapper::get_next_char()
{
	return iter_.nextPostInc();
}

bool UnicodeIcuStringWrapper::has_more_chars()
{
	return iter_.hasNext();
}

} /* end namespace Toki */
