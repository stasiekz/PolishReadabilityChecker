#ifndef SWIG_LIBCORPUS2_LEXEME_I
#define SWIG_LIBCORPUS2_LEXEME_I

%module libcorpuslexeme
%{
  #include <unicode/unistr.h>
  #include <libcorpus2/lexeme.h>
/*   #include <boost/make_shared.hpp> */
%}

%include "tag.i"

%include "std_string.i"
%include "std_vector.i"

%feature("notabstract") Corpus2::Lexeme;

%template(LexemeVector) std::vector<Corpus2::Lexeme>;

namespace Corpus2 {
  class Lexeme {
  public:
    Lexeme();
    Lexeme(const UnicodeString& lemma, const Tag& tag);

    static Lexeme create(const UnicodeString& lemma, const Tag& tag);
    static Lexeme create_utf8(const std::string& lemma, const Tag& tag);

    const UnicodeString& lemma() const;
    const std::string lemma_utf8() const;

    void set_lemma(const UnicodeString& l);
    void set_lemma_utf8(const std::string& l);

/*     const Tag& tag() const; UNSAFE */
    void set_tag(const Tag& tag);

    bool is_null() const;
    void set_disamb(bool value);
    bool is_disamb() const;
    void lowercase_lemma();

    bool operator<(const Lexeme& other) const;
    bool operator==(const Lexeme& other) const;
  };

  %extend Lexeme {
    /* Override lex.tag() with a version that returns a copy. */
    Corpus2::Tag tag() {
      return Corpus2::Tag(self->tag().get_pos(), self->tag().get_values());
    }
    // otherwise x != y wont trigger operator==
    %pythoncode %{
      def __ne__(self, other):
        return not self.__eq__(other)
    %}
  }
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_LEXEME_I */
