#ifndef SWIG_LIBWCCL_MATCHRULESEQUENCE_I
#define SWIG_LIBWCCL_MATCHRULESEQUENCE_I

%module libcclmatchrulesequence
%{
  #include <libwccl/ops/matchrulesequence.h>
%}

%include "matchrule.i"
%include "expression.i"
%include "libcorpus2/annotatedsentence.i"
%include "libcorpus2/sentence.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>

%template(ConstMatchRuleSequencePtr) boost::shared_ptr<const Wccl::Matching::MatchRuleSequence>;
%template(MatchRuleSequencePtr) boost::shared_ptr<Wccl::Matching::MatchRuleSequence>;


namespace Wccl {
namespace Matching {
  class MatchRuleSequence : public std::vector<MatchRule>, public Expression {
  public:
    MatchRuleSequence();
    MatchRuleSequence(const std::vector<MatchRule>& rules);

    /* --------------------------------------------------------------------- */

    %rename(OpFunMatchRuleSequence) operator()(const shared_ptr<Corpus2::AnnotatedSentence>& sentence);
    void operator()(const shared_ptr<Corpus2::AnnotatedSentence>& sentence);

    /* --------------------------------------------------------------------- */
    %exception {
      try {
        $action
      } catch (Wccl::WcclError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    void apply_all(const shared_ptr<Corpus2::AnnotatedSentence>& sentence);
    
    %exception {
      try {
        $action
      } catch (Wccl::WcclError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    void apply_all_sentence_wrapper(const shared_ptr<Corpus2::Sentence>& sentence);

    /* --------------------------------------------------------------------- */

    std::string to_string(const Corpus2::Tagset& tagset) const;
  };

  %exception {
      try {
        $action
      } catch (Wccl::WcclError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
}
}

using namespace boost;
using namespace Wccl;
using namespace std;
using namespace Wccl::Matching;

#endif /* SWIG_LIBWCCL_MATCHRULESEQUENCE_I */
