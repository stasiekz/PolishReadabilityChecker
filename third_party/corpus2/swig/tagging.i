#ifndef SWIG_LIBCORPUS2_TAGGING_I
#define SWIG_LIBCORPUS2_TAGGING_I

%module libcorpustagging
%{
  #include <libcorpus2/tagging.h>
%}

%include "std_string.i"

%include "tag.i"
%include "tagset.i"
%include "token.i"

namespace Corpus2 {

Tag get_attribute_mask(const Tagset& tagset,
                       const std::string attr_name);

Tag mask_token(const Token& token, const Tag& mask, bool disamb_only);

int mask_card(const Tag& mask);

Tag with_values_masked(Tag input, Tag attr_value, Tag attr_mask);

bool select_preferred_disamb(const Tagset& tagset, Token* token);

void select_preferred_lexeme(const Tagset& tagset, Token* token);

bool select_preferred_disamb_tag(const Tagset& tagset, Token* token);

void select_preferred_tag(const Tagset& tagset, Token* token);

void expand_optional_attrs(const Tagset& tagset, Token* token);

void select_singular_tags(const Tagset& tagset, Token* token);

bool disambiguate_equal(Token* token, const Tag& mask_where, const Tag& mask_wanted);

bool disambiguate_subset(Token* token, const Tag& mask_where, const Tag& mask_wanted);

bool disambiguate_lemma(Token* token, const std::string& lemma_utf8);

void overwrite_lemmas(Token* token, const std::string& lemma_utf8);

void lowercase_lemmas(Token* token);

void set_disambs(Token *token, const Tag& wanted_tag);
}

using namespace std;
using namespace Corpus2;


#endif /* SWIG_LIBCORPUS2_TAGGING_I */
