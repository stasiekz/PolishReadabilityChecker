#ifndef SWIG_LIBWCCL_MATCHRULE_I
#define SWIG_LIBWCCL_MATCHRULE_I

%module libcclmatchrule
%{
  #include <libwccl/ops/matchrule.h>
%}

%include "variables.i"
%include "parsedexpression.i"
%include "libcorpus2/annotatedsentence.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>

%template(MatchRulePtr) boost::shared_ptr<Wccl::Matching::MatchRule>;
%template(ConstMatchRulePtr) boost::shared_ptr<const Wccl::Matching::MatchRule>;

%template(MatchRuleVector) std::vector<Wccl::Matching::MatchRule>;

namespace Wccl {
namespace Matching {
  class MatchRule : public ParsedExpression {
  public:
    MatchRule(
      const Variables& variables,
      const boost::shared_ptr<ApplyOperator>& apply
    );

    MatchRule(const MatchRule& other, bool clean = false);
    MatchRule();

    /* --------------------------------------------------------------------- */

    %rename(OpFunMatchRule) operator()(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s);
    void operator()(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s);

    void apply(const boost::shared_ptr<Corpus2::AnnotatedSentence>& s);

    /* --------------------------------------------------------------------- */

    MatchRule clone() const;
    MatchRule clone_clean() const;

    boost::shared_ptr<MatchRule> clone_ptr() const;
    boost::shared_ptr<MatchRule> clone_clean_ptr() const;

    /* --------------------------------------------------------------------- */

    %rename(OpEqMatchRule) *::operator=(const MatchRule& other);

    /* --------------------------------------------------------------------- */

    std::string to_string(const Corpus2::Tagset& tagset) const;
  };
}
}

using namespace std;
using namespace Wccl;
using namespace Wccl::Matching;

#endif /* SWIG_LIBWCCL_MATCHRULE_I */
