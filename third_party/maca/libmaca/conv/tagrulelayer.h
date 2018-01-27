/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBMACA_CONV_TAGRULELAYER_H
#define LIBMACA_CONV_TAGRULELAYER_H

#include <libmaca/conv/layer.h>
#include <libmaca/conv/tagrule.h>
#include <libmaca/util/confignode.h>
#include <unicode/regex.h>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace Maca {
namespace Conversion {

/**
 * A layer of TagRules which are applied in order
 */
class TagRuleLayer : public OneTagsetLayer
{
public:
	/**
	 * Empty layer constructor
	 */
	TagRuleLayer(const Corpus2::Tagset& tagset);

	/**
	 * Constructor from a config node. Creates a layer and uses the config
	 * to create one tag rule.
	 */
	TagRuleLayer(const Config::Node& cfg);

	/// Cloning
	TagRuleLayer* clone() const;

	/**
	 * Rule adder
	 */
	void append_rule(const TagRule& tr);

	/**
	 * Rule adder, the rule is constructed from the config node
	 * Recognized keys are:
	 * - pre - a precondition predicate string, can appear multiple times
	 * - post - a postcondition predicate string, can appear multiple times
	 */
	void append_rule(const Config::Node& cfg);

	/**
	 * Layer override -- get tokens from source, pass them through the
	 * tag rules and return.
	 */
	Corpus2::Token* get_next_token();

protected:
	/// Pass a token through the rules
	void process(Corpus2::Token*);

private:
	/// The TagRules used by this layer
	std::vector<TagRule> rules_;
};

class RegexTagRuleLayer : public TagRuleLayer
{
public:
	/**
	 * Empty layer constructor
	 */
	RegexTagRuleLayer(const Corpus2::Tagset& tagset);

	/**
	 * Constructor from a config node. Creates a layer and uses the config
	 * to create one tag rule with a regex.
	 */
	RegexTagRuleLayer(const Config::Node& cfg);

	/// Cloning
	RegexTagRuleLayer* clone() const;

	/// Destructor
	~RegexTagRuleLayer();

	/**
	 * Layer override -- get tokens from source, pass them through the
	 * tag rules and return.
	 */
	Corpus2::Token* get_next_token();

private:
	boost::scoped_ptr<RegexMatcher> matcher_;

	boost::shared_ptr<RegexPattern> pattern_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_TAGRULELAYER_H
