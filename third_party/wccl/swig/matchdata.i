#ifndef SWIG_LIBWCCL_MATCHDATA_I
#define SWIG_LIBWCCL_MATCHDATA_I

%module libcclmatchdata
%{
  #include <libwccl/values/matchdata.h>
%}

%include "match.i"
%include "libcorpus2/annotatedsentence.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%nodefaultctor Wccl::MatchData;

%template(MatchDataPtr) boost::shared_ptr<Wccl::MatchData>;

namespace Wccl {
  class MatchData {
  public:
    virtual bool empty() const = 0;

    /* --------------------------------------------------------------------- */

    virtual int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const = 0;
    virtual int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>&) const = 0;

    /* --------------------------------------------------------------------- */

    // virtual const boost::shared_ptr<const Match> submatch(size_t) const;
    virtual const boost::shared_ptr<Match>& submatch(size_t);

    /* --------------------------------------------------------------------- */

    boost::shared_ptr<MatchData> clone() const;
    
    /* --------------------------------------------------------------------- */
    
    virtual std::string to_raw_string() const = 0;

    /* --------------------------------------------------------------------- */

    virtual ~MatchData() {}
  };
}

using namespace std;
using namespace Wccl;
using namespace boost;

#endif /* SWIG_LIBWCCL_MATCHDATA_I */
