#ifndef SWIG_LIBCORPUS2_CORPUS_READER_I
#define SWIG_LIBCORPUS2_CORPUS_READER_I

%module libcorpusdocument
%{
  #include <libcorpus2_whole/io/corpusreader.h>
%}

%include "std_defs.i"
%include "tagset.i"
%include "corpus.i"

%template(CorpusReaderPtr) boost::shared_ptr<Corpus2::whole::CorpusReader>;
%template(ConstCorpusReaderPtr) boost::shared_ptr<const Corpus2::whole::CorpusReader>;

%template(CorpusReaderPtrVector) std::vector<boost::shared_ptr<Corpus2::whole::CorpusReader> >;

namespace Corpus2 {
namespace whole {
  class CorpusReader {
  public:
    CorpusReader(const Tagset& tagset, const std::string& corpus_type);
    boost::shared_ptr<Corpus> read(const std::string& corpus_file);
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_CORPUS_READER_I */
