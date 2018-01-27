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

#ifndef LIBMACA_TOKEN_LAYER_H
#define LIBMACA_TOKEN_LAYER_H

#include <libcorpus2/tokensource.h>
#include <libcorpus2/tagset.h>
#include <boost/function.hpp>
#include <vector>

namespace Maca {
namespace Conversion {

/**
 * Base class for conversion layers.
 *
 * Layers accept tokens in one tagset, perform some changes, and output
 * tokens in a possibly different tagset via the TokenSource interface.
 */
class Layer : public Corpus2::TokenSource
{
public:
	/// Constructor
	Layer();

	/// Destructor
	virtual ~Layer();

	/// Cloning
	virtual Layer* clone() const = 0;

	/// Source setter
	void set_source(Corpus2::TokenSource* src);

	/// Source getter
	Corpus2::TokenSource* source();

	/**
	 * The tagset of tokens coming into the layer.
	 */
	virtual const Corpus2::Tagset& tagset_from() const = 0;

	/**
	 * The tagset of tokens coming out of the layer.
	 */
	virtual const Corpus2::Tagset& tagset_to() const = 0;

private:
	/// The current token source
	Corpus2::TokenSource* source_;
};

/**
 * Base class for conversion layers that do not change a token's tagset.
 *
 * (i.e. the 'from' and 'to' tagsets are identical).
 */
class OneTagsetLayer : public Layer
{
public:
	/**
	 * Constructor.
	 * @param tagset the tagset used for tokens coming in and out of the
	 *               layer.
	 */
	OneTagsetLayer(const Corpus2::Tagset& tagset);

	/// Layer override
	const Corpus2::Tagset& tagset_from() const;

	/// Layer override
	const Corpus2::Tagset& tagset_to() const;

	/**
	 * Redundant tagset accessor for consistency and to avoid having to
	 * choose between _from and _to in derived classes.
	 */
	const Corpus2::Tagset& tagset() const {
		return tagset_;
	}

private:
	/// The tagset
	const Corpus2::Tagset& tagset_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_TOKEN_LAYER_H
