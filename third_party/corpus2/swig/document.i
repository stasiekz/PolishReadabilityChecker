#ifndef SWIG_LIBCORPUS2_DOCUMENT_I
#define SWIG_LIBCORPUS2_DOCUMENT_I

%module libcorpusdocument
%{
  #include <libcorpus2_whole/document.h>
%}

%include "std_defs.i"
%include "chunk.i"
%include "relation.i"

%template(DocumentPtr) boost::shared_ptr<Corpus2::whole::Document>;
%template(ConstDocumentPtr) boost::shared_ptr<const Corpus2::whole::Document>;

%template(DocumentPtrVector) std::vector<boost::shared_ptr<Corpus2::whole::Document> >;

namespace Corpus2 {
namespace whole {
  class Document {
  public:
    Document(const std::string& path = "");
    ~Document();

    void add_paragraph(const boost::shared_ptr<Chunk> para);
    void add_relation(const boost::shared_ptr<Relation> relation);

    const std::vector< boost::shared_ptr<Chunk> >& paragraphs() const;
    const std::vector< boost::shared_ptr<Relation> >& relations() const;

    const std::string& path() const;
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_DOCUMENT_I */
