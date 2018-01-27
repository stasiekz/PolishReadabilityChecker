#ifndef SWIG_LIBCORPUS2_CORPUS_READER_I
#define SWIG_LIBCORPUS2_CORPUS_READER_I

%module libcorpusdocument
%{
  #include <libcorpus2_whole/io/documentreader.h>
%}

%include "std_defs.i"
%include "tagset.i"
%include "corpus.i"

%template(DocumentReaderPtr) boost::shared_ptr<Corpus2::whole::DocumentReader>;
%template(ConstDocumentReaderPtr) boost::shared_ptr<const Corpus2::whole::DocumentReader>;

%template(DocumentReaderPtrVector) std::vector<boost::shared_ptr<Corpus2::whole::DocumentReader> >;

namespace Corpus2 {
namespace whole {
  class DocumentReader {
  public:
    DocumentReader(const Tagset& tagset, const std::string& corpus_type,const std::string& corpus_file_path, const std::string& corpus_reader);
    DocumentReader(const Tagset& tagset, const std::string& corpus_type,const std::string& corpus_file_path);
    boost::shared_ptr<Document> read();
    boost::shared_ptr<Document> read_with_auto_id();
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_CORPUS_READER_I */
