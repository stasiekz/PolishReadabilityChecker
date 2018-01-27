#ifndef SWIG_LIBCORPUS2_ANNOTATIONVIEW_I
#define SWIG_LIBCORPUS2_ANNOTATIONVIEW_I

%module libcorpusannotationview
%{
  #include <libcorpus2/ann/view.h>
%}

%include "sentence.i"
%include "annotationchannel.i"
%include "annotatedsentence.i"

%include "std_string.i"
%include "boost_shared_ptr.i"

%template(AnnotationViewPtr) boost::shared_ptr<Corpus2::AnnotationView>;
%template(ConstAnnotationViewPtr) boost::shared_ptr<const Corpus2::AnnotationView>;

%nodefaultctor Corpus2::TokenWriter;

namespace Corpus2 {
  class AnnotationView : public Corpus2::Sentence {
  public:
    AnnotationView(const boost::shared_ptr<AnnotatedSentence>& original, const std::string& ann_name_);
    ~AnnotationView();

    /* --------------------------------------------------------------------- */
    boost::shared_ptr<Sentence> clone_shared() const;
    
    /* --------------------------------------------------------------------- */
    void commit();
    void commit_to(
      const boost::shared_ptr<AnnotatedSentence>& original, 
      const std::string& ann_name);

    /* --------------------------------------------------------------------- */
    void release_original();
  };
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_ANNOTATIONVIEW_I */
