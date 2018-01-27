#ifndef SWIG_LIBWCCL_SENTENCECONTEXT_I
#define SWIG_LIBWCCL_SENTENCECONTEXT_I

%module libcclsentencecontext
%{
  #include <libwccl/sentencecontext.h>
%}

%include "libcorpus2/token.i"
%include "libcorpus2/lexeme.i"
%include "libcorpus2/sentence.i"

%include "position.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

// %feature("notabstract") Wccl::SentenceContext;

%template(SentenceContextPtr) boost::shared_ptr<Wccl::SentenceContext>;

namespace Wccl {
  class SentenceContext {
  public:
    explicit SentenceContext(const boost::shared_ptr<Corpus2::Sentence>& s);

    SentenceContext duplicate() const;
    SentenceContext* clone() const;
    
    Corpus2::Sentence& get_sentence();
    const Corpus2::Sentence& get_sentence() const;

    int size() const;
    int get_position() const;

    bool is_current_inside() const;
    bool is_inside(int abs_pos) const;
    bool is_inside(const Position& pos) const;

    bool is_outside(int abs_pos) const;
    bool is_outside(const Position& pos) const;

    bool validate_range(
      const Position& left,
      const Position& right,
      int& abs_left,
      int& abs_right) const;

    void set_position(int new_position);
    void advance();
    void goto_start();
    
    const Corpus2::Token* at(int abs_pos) const;
    const Corpus2::Token* at(const Position& position) const;
    Corpus2::Token* at(int abs_pos);
    Corpus2::Token* at(const Position& position);
    const Corpus2::Token* current() const;
    Corpus2::Token* current();

    int get_abs_position(const Position& position) const;
    int get_rel_position(const Position& position) const;
    int translate_special_position(int pos) const;
  };
}

using namespace boost;
using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_SENTENCECONTEXT_I */
