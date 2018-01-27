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

#ifndef LIBCORPUS2_TAGGING_H
#define LIBCORPUS2_TAGGING_H

#include <libcorpus2/tag.h>
#include <libcorpus2/token.h>
#include <libcorpus2/tagset.h>

namespace Corpus2 {

/**
  * Creates a mask having all values of the given attribute set.
  * Pass an empty string to get a mask for all POS values.
  * The resulting object should only be used for masking as it won't be
  * a valid tag.
  *
  * @arg tagset_name tagset to use
  * @arg attr_name attrinbute as defined in tagset or empty string for POS
  * @return mask for given attribute
  */
Tag get_attribute_mask(const Tagset& tagset,
					   const std::string attr_name);

/**
  * Projects the token onto the mask. Depending on disamb_only, will consider
  * only disamb or all lexemes.
  */
Tag mask_token(const Token& token, const Tag& mask, bool disamb_only);

/** Returns the number of set elements belonging to the mask given. */
int mask_card(const Tag& mask);

/**
  * Returns a copy of the given input tag with the attribute referred
  * to by attr_mask value set to attr_value (possibly empty).
  * NOTE: only attribute part of the masks are considered.
  */
Tag with_values_masked(Tag input, Tag attr_value, Tag attr_mask);

/** Forces one disamb lexeme per token. The selection is based on tagset
  * definition order. Returns if any disamb found.
  */
bool select_preferred_disamb(const Tagset& tagset, Token* token);

/** Forces one lexeme per token. The selection is based on tagset
  * definition order, disamb markers are not respected.
  * The selected lexeme will be set to disamb=True.
  */
void select_preferred_lexeme(const Tagset& tagset, Token* token);

/** Forces one DISAMB TAG per token. Works as select_preferred_disamb,
  * but multiple disamb lexemes may be left, as long as they differ only
  * in base forms. Returns if any disamb found.
  */
bool select_preferred_disamb_tag(const Tagset& tagset, Token* token);

/** Forces one TAG per token. Works as select_preferred_lexeme, but multiple
  * lexemes may be left, as long as they differ only in base forms.
  */
void select_preferred_tag(const Tagset& tagset, Token* token);

/** Encodes optional attributes with unspecified values as each value set.
  * This is to facilitate safe masking when the value in question is not to be
  * skipped.
  */
void expand_optional_attrs(const Tagset& tagset, Token* token);

/** Repairs multivalue tags. Optional attributes will be cleared if
  * multi-value. Regular attributes will be set to lowest value given.
  */
void select_singular_tags(const Tagset& tagset, Token* token);

/** Tries to select only those lexemes whose tags projected onto mask_where
  * have exactly the value as given in mask_wanted. E.g. pass whole attribute
  * as mask_where and a particular desired value as mask_wanted. If no lexeme
  * satisfies the constraint, will leave the token intact.
  * @return if succeeded
  */
bool disambiguate_equal(Token* token, const Tag& mask_where,
						const Tag& mask_wanted);

/** Tries to select only those lexemes whose tags projected onto mask_where
  * have a subset of the value as given in mask_wanted. E.g. pass noun + gerund
  * mask and have both left. NOTE: this may be inconvenient for dealing with
  * optional attributes. If no lexeme satisfies the constraint, will leave the
  * token intact.
  * @return if succeeded
  */
bool disambiguate_subset(Token* token, const Tag& mask_where,
						const Tag& mask_wanted);

/** Tries to select only those lexemes whose lemmas are equal to the given
  * string. If no lexeme satisfies the constraint, will leave the token intact.
  * @return if succeeded
  */
bool disambiguate_lemma(Token* token, const std::string& lemma_utf8);

/** Overwrites each lexeme's lemma with the given string.
  * NOTE that it may result in generation of duplicate lexemes.
  */
void overwrite_lemmas(Token* token, const std::string& lemma_utf8);

/** Overwrites each lexeme's lemma with the lower-case version
  * of the original lemma.
  * NOTE that it may result in generation of duplicate lexemes.
  */
void lowercase_lemmas(Token* token);

/** Sets lexemes' disamb markers iff lexeme.tag is wanted_tag. */
void set_disambs(Token *token, const Tag& wanted_tag);

} /* end ns Corpus2 */

#endif // LIBCORPUS2_TAGGING_H
