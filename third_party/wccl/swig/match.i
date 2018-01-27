#ifndef SWIG_LIBWCCL_MATCH_I
#define SWIG_LIBWCCL_MATCH_I

%module libcclbool
%{
  #include <libwccl/values/match.h>
%}

%include "value.i"
%include "matchdata.i"
%include "tokenmatch.i"
%include "annotationmatch.i"
%include "matchvector.i"
%include "libcorpus2/annotatedsentence.i"

%include "std_string.i"
// %include <libcorpus2/boost_shared_ptr.i>

%feature("notabstract") Wccl::Match;

%template(MatchPtr) boost::shared_ptr<Wccl::Match>;
%template(ConstMatchPtr) boost::shared_ptr<Wccl::Match const>;

namespace Wccl {
  class Match : public Value {
  public:
    const char* get_type_name() const { return type_name; }
    static std::string var_repr(const std::string &var_name);
    std::string make_var_repr(const std::string &var_name) const {
      return var_repr(var_name);
    }
    /* --------------------------------------------------------------------- */

    Match();
    /* --------------------------------------------------------------------- */

    Match(const boost::shared_ptr<MatchData>& data);
    Match(const boost::shared_ptr<TokenMatch>& data);
    Match(const boost::shared_ptr<AnnotationMatch>& data);
    Match(const boost::shared_ptr<MatchVector>& data);
    /* --------------------------------------------------------------------- */
  
    Match(const MatchData& data);
    Match(const Match& match);
    /* --------------------------------------------------------------------- */
  
    %rename(OperatorEqMatch) *::operator=(const Match& other);
    /* --------------------------------------------------------------------- */

    // const MatchData& get_value() const;
    MatchData& get_value();
    /* --------------------------------------------------------------------- */

    void set_value(const MatchData& m);
    /* --------------------------------------------------------------------- */

    bool empty() const;
    /* --------------------------------------------------------------------- */

    int first_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;
    int last_token(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s) const;
    /* --------------------------------------------------------------------- */

    std::string to_raw_string() const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_MATCH_I */
