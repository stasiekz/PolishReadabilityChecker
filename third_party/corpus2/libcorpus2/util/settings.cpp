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

#include <libcorpus2/util/settings.h>

#ifdef HAVE_CONFIG_D_H
#include <libcorpus2/config_d.h>
#endif

#ifndef LIBCORPUS2_PATH_SEPARATOR
#define LIBCORPUS2_PATH_SEPARATOR ":"
#endif

#include <sstream>

namespace Corpus2 {

Corpus2PathSearcher::Corpus2PathSearcher()
	: PwrNlp::PathSearcher<FileNotFound>(LIBCORPUS2_PATH_SEPARATOR)
{
#ifdef LIBCORPUS2_DATA_DIR
	set_search_path(LIBCORPUS2_DATA_DIR);
#else
	set_search_path(".");
#endif
}

} /* end namespace Corpus2 */
