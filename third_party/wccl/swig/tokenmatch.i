#ifndef SWIG_LIBWCCL_TOKENMATCH_I
#define SWIG_LIBWCCL_TOKENMATCH_I

%module libccltokenmatch
%{
  #include <libwccl/values/tokenmatch.h>
%}

%include "matchdata.i"
%include "libcorpus2/annotatedsentence.i"

%nodefaultctor Wccl::TokenMatch;

namespace Wccl {
  class TokenMatch : public MatchData {
  public:
    explicit TokenMatch(int pos);

    /* --------------------------------------------------------------------- */
    bool empty() const;

    /* --------------------------------------------------------------------- */
    int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const;
    int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const;

    /* --------------------------------------------------------------------- */
    std::string to_raw_string() const;
  };
}

using namespace std;
using namespace Wccl;


#endif /* SWIG_LIBWCCL_TOKENMATCH_I */
