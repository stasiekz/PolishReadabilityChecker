#ifndef SWIG_LIBCORPUS2_TOKEN_I
#define SWIG_LIBCORPUS2_TOKEN_I

%module libcorpustoken
%{
  #include <unicode/unistr.h>
  #include <libcorpus2/token.h>
%}

%include "lexeme.i"
%include "tagset.i"
%include "tokenmetadata.i"

%include "std_string.i"
%include "std_vector.i"
%include "boost_shared_ptr.i"


%feature("notabstract") Corpus2::Token;

%template(TokenPtr) boost::shared_ptr<Corpus2::Token>;
%template(ConstTokenPtr) boost::shared_ptr<const Corpus2::Token>;

%template(TokenPtrVector) std::vector<Corpus2::Token*>;
%template(ConstTokenPtrVector) std::vector<const Corpus2::Token*>;

namespace Corpus2 {
  class Token {
  public:
    Token();
    Token(const UnicodeString& orth, PwrNlp::Whitespace::Enum wa);
    Token* clone() const;

    static Token* create_utf8(const std::string& orth_utf8, PwrNlp::Whitespace::Enum wa = PwrNlp::Whitespace::Space);
    
    const UnicodeString& orth() const;
    std::string orth_utf8() const;

    void set_orth(const UnicodeString& orth);
    void set_orth_utf8(const std::string& orth);

    const PwrNlp::Whitespace::Enum& wa() const;
    void set_wa(const PwrNlp::Whitespace::Enum& wa);
    bool after_space() const;

    const std::vector<Lexeme>& lexemes() const;
    std::vector<Lexeme>& lexemes();

    void add_lexeme(const Lexeme& lex);

    void remove_all_lexemes();

    const Lexeme& get_preferred_lexeme(const Tagset& tagset) const;
    size_t get_preferred_lexeme_index(const Tagset& tagset) const;

    void make_ign(const Tagset& tagset);
    void add_ign(const Tagset& tagset);

    bool operator==(const Token& other) const;

    bool check_duplicate_lexemes() const;
    bool remove_duplicate_lexemes();

    bool has_disamb_lexeme() const;
    int count_disamb_lexemes() const;

    boost::shared_ptr<TokenMetaData> get_metadata() const;
    void set_metadata(TokenMetaData& md);
    void set_metadata_ptr(boost::shared_ptr<TokenMetaData> md);
    bool has_metadata();

    void create_metadata();
  };

  %extend Token {
    // otherwise x != y wont trigger operator==
    %pythoncode %{
      def __ne__(self, other):
        return not self.__eq__(other)
    %}
  }
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_TOKEN_I */
