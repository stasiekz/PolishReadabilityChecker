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

#include <libtoki/layers/init.h>
#include <libtoki/layers/affixsplit.h>
#include <libtoki/layers/append.h>
#include <libtoki/layers/basicsplit.h>
#include <libtoki/layers/check.h>
#include <libtoki/layers/combine.h>
#include <libtoki/layers/groupsplit.h>
#include <libtoki/layers/groupaffixsplit.h>
#include <libtoki/layers/lexiconclassify.h>
#include <libtoki/layers/regexpclassify.h>
#include <libtoki/layers/regexmatchsplit.h>

namespace Toki {

bool init_token_layers()
{
	static bool init_done = false;
	if (init_done) return false;
	TokenLayer::register_layer<TokenLayer>("passthrough");
	TokenLayer::register_layer<AffixSplitLayer>("affix_split");
	TokenLayer::register_layer<AppendLayer>("append");
	TokenLayer::register_layer<BasicSplitLayer>("split");
	TokenLayer::register_layer<GroupSplitLayer>("group_split");
	TokenLayer::register_layer<GroupAffixSplitLayer>("group_affix_split");
	TokenLayer::register_layer<CheckLayer>("check");
	TokenLayer::register_layer<CombineLayer>("combine");
	TokenLayer::register_layer<RegexpClassifyLayer>("regexp");
	TokenLayer::register_layer<CaseLexiconClassifyLayer>("lexicon");
	TokenLayer::register_layer<CaselessLexiconClassifyLayer>(
			"lexicon_caseless");
	TokenLayer::register_layer<RegexMatchSplitLayer>("regex_match_split");
	init_done = true;
	return true;
}

} /* end namespace Toki */
