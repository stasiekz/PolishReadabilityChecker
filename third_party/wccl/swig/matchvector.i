#ifndef SWIG_LIBWCCL_MATCHVECTOR_I
#define SWIG_LIBWCCL_MATCHVECTOR_I

%module libcclmatchvector
%{
  #include <libwccl/values/matchvector.h>
%}

%include "match.i"
%include "matchdata.i"
%include "tokenmatch.i"
%include "annotationmatch.i"
%include "libcorpus2/annotatedsentence.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%template(MatchVectorPtr) boost::shared_ptr<Wccl::MatchVector>;

namespace Wccl {
  class MatchVector {
  public:
    MatchVector();
    ~MatchVector();
    /* --------------------------------------------------------------------- */

    bool empty() const;
    /* --------------------------------------------------------------------- */
    
    int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;
    int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;
    /* --------------------------------------------------------------------- */

    std::string to_raw_string() const;
    /* --------------------------------------------------------------------- */

    void append(const boost::shared_ptr<Match>& m);
    void append(const boost::shared_ptr<MatchData>& m);
    void append(const boost::shared_ptr<MatchVector>& m);
    void append(const boost::shared_ptr<TokenMatch>& m);
    void append(const boost::shared_ptr<AnnotationMatch>& m);
    /* --------------------------------------------------------------------- */

    size_t size() const;
    void clear();
    /* --------------------------------------------------------------------- */

    const boost::shared_ptr<Match>& submatch(size_t idx);
    // const boost::shared_ptr<const Match> submatch(size_t idx) const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_MATCHVECTOR_I */
