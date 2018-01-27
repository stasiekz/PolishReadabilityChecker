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

#ifndef LIBMACA_CONV_JOINLAYER_H
#define LIBMACA_CONV_JOINLAYER_H

#include <libmaca/conv/layer.h>
#include <libmaca/conv/joinrule.h>
#include <libmaca/util/confignode.h>

namespace Maca {
namespace Conversion {

/**
 * A layer of JoinRules, each of which is tried on consecutive pairs of
 * tokens that appear from the source of the layer. If no rule joins the
 * tokens, the first one is output unchanged and the second one retained
 * for possible joining with the next token that will be processed from the
 * source. The rules are checked in order, the first matching one is used.
 *
 * @see JoinRule for details of how tokens are joined.
 */
class JoinLayer : public OneTagsetLayer, boost::noncopyable
{
public:
	/**
	 * Constructor for an empty JoinLayer working within a tagset.
	 *
	 * An empty JoinLayer has no rules so it simply will never join tokens.
	 */
	JoinLayer(const Corpus2::Tagset& tagset);

	/**
	 * Config node constructor. The config node is passed to the parent
	 * class, and then one JoinRule is created from it.
	 */
	JoinLayer(const Config::Node& cfg);

	/// Destructor
	~JoinLayer();

	/// Cloning
	JoinLayer* clone() const;

	/**
	 * Adds a rule to the list of join rules in this layer.
	 *
	 * The tagset of the JoinRule must match this layer's tagset.
	 */
	void append_rule(const JoinRule& rule);

	/**
	 * Creates a JoinRule from a config node and adds it to the list of
	 * join rules. Tagsets must match.
	 */
	void append_rule(const Config::Node& cfg);

	/// Layer override
	Corpus2::Token* get_next_token();

private:
	/// The rules
	std::vector<JoinRule> rules_;

	/// buffer for the token that might be joined with the next one
	Corpus2::Token* buf_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_JOINLAYER_H
