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

#include <libtoki/unicode/istreamwrapper.h>

#include <iostream>

namespace Toki {

UnicodeIstreamWrapper::UnicodeIstreamWrapper(std::istream& is,
		int buf_size /* = 200 */,
		const char* input_encoding /* = "UTF8" */)
	: is_(is)
	, converter_(NULL)
	, buf_size_(buf_size)
	, source_buf_(new char[buf_size])
	, target_buf_(new UChar[buf_size * 2])
	, target_(target_buf_)
	, out_(target_)
{
	UErrorCode err = U_ZERO_ERROR;
	converter_ = ucnv_open(input_encoding, &err);
}

UnicodeIstreamWrapper::~UnicodeIstreamWrapper()
{
	delete [] source_buf_;
	delete [] target_buf_;
	ucnv_close(converter_);
}

UChar UnicodeIstreamWrapper::peek_next_char()
{
	ensure_more();
	if (target_ > out_) {
		return *out_;
	} else {
		std::cerr << "peek_next_char 0!\n";
		return 0;
	}
}

UChar UnicodeIstreamWrapper::get_next_char()
{
	ensure_more();
	if (target_ > out_) {
		return *out_++;
	} else {
		std::cerr << "get_next_char 0!\n";
		return 0;
	}
}

UnicodeString UnicodeIstreamWrapper::get_buffer()
{
	ensure_more();
	UnicodeString ret(out_, target_ - out_);
	out_ = target_;
	return ret;
}

bool UnicodeIstreamWrapper::has_more_chars()
{
	ensure_more();
	return target_ > out_;
}

int UnicodeIstreamWrapper::more()
{
	is_.read(source_buf_, buf_size_);
	int buf_read = is_.gcount();
	bool flush = is_.eof();
	char* source_limit = source_buf_ + buf_read;
	UChar* target_limit_ = target_buf_ + buf_size_ * 2;
	target_ = target_buf_;
	UErrorCode err = U_ZERO_ERROR;
	const char* source = source_buf_;
	ucnv_toUnicode(converter_,
				   &target_, target_limit_,
				   &source, source_limit,
				   NULL, flush, &err);
	if (err == U_BUFFER_OVERFLOW_ERROR) {
		std::cerr << "overflow\n";
	} else if (err != U_ZERO_ERROR) {
		std::cerr << "err!" << err << "\n";
	}
	out_ = target_buf_;
	return buf_read;
}

void UnicodeIstreamWrapper::ensure_more()
{
	//keep calling more() as long as there's nothing ready in the target
	//buffer and it reads something from the input stream.
	while (target_ <= out_ && more()) ;
}

} /* end namespace Toki */

