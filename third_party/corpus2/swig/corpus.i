#ifndef SWIG_LIBCORPUS2_CORPUS_I
#define SWIG_LIBCORPUS2_CORPUS_I

%module libcorpuscorpus
%{
  #include <libcorpus2_whole/corpus.h>
%}

%include "std_defs.i"
%include "document.i"

%template(CorpusPtr) boost::shared_ptr<Corpus2::whole::Corpus>;
%template(ConstCorpusPtr) boost::shared_ptr<const Corpus2::whole::Corpus>;

namespace Corpus2 {
namespace whole {
  class Corpus {
  public:
    Corpus(const std::string name = "");

    void add_document(boost::shared_ptr<Document> document);

    boost::shared_ptr<Document> next_document();
    const std::vector<boost::shared_ptr<Document> > documents() const;
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_CORPUS_I */
