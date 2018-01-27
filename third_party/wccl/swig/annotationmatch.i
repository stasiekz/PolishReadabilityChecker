#ifndef SWIG_LIBWCCL_ANNOTATIONMATCH_I
#define SWIG_LIBWCCL_ANNOTATIONMATCH_I

%module libcclannotationmatch
%{
  #include <libwccl/values/annotationmatch.h>
%}

%include "matchdata.i"
%include "libcorpus2/annotatedsentence.i"

%include "std_string.i"

%nodefaultctor Wccl::AnnotationMatch;

namespace Wccl {
  class AnnotationMatch : public MatchData {
  public:
    explicit AnnotationMatch(int pos, const std::string channel);

    /* --------------------------------------------------------------------- */
    bool empty() const;

    /* --------------------------------------------------------------------- */
    int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const;
    int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const;

    /* --------------------------------------------------------------------- */
    std::string to_raw_string() const;
  };
}

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_ANNOTATIONMATCH_I */
