#ifndef SWIG_LIBCORPUS2_SENTENCE_I
#define SWIG_LIBCORPUS2_SENTENCE_I

%module libcorpussentence
%{
  #include <libcorpus2/sentence.h>
%}

%include "std_string.i"
%include "token.i"
%include "boost_shared_ptr.i"

%template(SentencePtr) boost::shared_ptr<Corpus2::Sentence>;
%template(ConstSentencePtr) boost::shared_ptr<const Corpus2::Sentence>;

%template(SentencePtrVector) std::vector<boost::shared_ptr<Corpus2::Sentence> >;
%template(ConstSentencePtrVector) std::vector<boost::shared_ptr<const Corpus2::Sentence> >;

namespace Corpus2 {
  class Sentence {
  public:
    typedef boost::shared_ptr<Sentence> Ptr;
    typedef boost::shared_ptr<const Sentence> ConstPtr;
    
    /* --------------------------------------------------------------------- */

    Sentence(const std::string &id = "");
    ~Sentence();

    /* --------------------------------------------------------------------- */

    virtual Ptr clone_shared() const;
    void release_tokens();
    bool empty() const;
    size_t size() const;
    
    %rename(GetToken) operator[];
    Token* operator[](size_t idx);

    %rename(GetConstToken) operator[];
    const Token* operator[](size_t idx) const;

    const std::vector<Token*>& tokens() const;
    std::vector<Token*>& tokens();

    virtual void append(Token* t);
    const Token* first_token() const;

    const std::string id() const;

    void set_id(const std::string &id);

    %extend {
      static boost::shared_ptr<Corpus2::Sentence> create_sent(const std::string &id) {
        return boost::shared_ptr<Corpus2::Sentence>(new Corpus2::Sentence(id));
      }
    }
  };
}

using namespace std;
using namespace boost;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_SENTENCE_I */
