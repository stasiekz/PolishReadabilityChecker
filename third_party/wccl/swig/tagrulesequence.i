#ifndef SWIG_LIBWCCL_TAGRULESEQUENCE_I
#define SWIG_LIBWCCL_TAGRULESEQUENCE_I

%module libccltagrulesequence
%{
  #include <libwccl/ops/tagrulesequence.h>
%}

%include "bool.i"
%include "tagrule.i"
%include "expression.i"
%include "libcorpus2/sentence.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>


%template(TagRuleSequencPtr) boost::shared_ptr<Wccl::TagRuleSequence>;
%template(ConstTagRuleSequencPtr) boost::shared_ptr<const Wccl::TagRuleSequence>;

namespace Wccl {
  class TagRuleSequence : public std::vector<TagRule>, public Expression {
  public:
    TagRuleSequence(const std::vector<TagRule>& rules);
    TagRuleSequence();

    /* --------------------------------------------------------------------- */

    %rename(OpFunTagRuleSequence) operator()(
      const shared_ptr<Corpus2::Sentence>& sentence
    );
    Bool operator()(const shared_ptr<Corpus2::Sentence>& sentence);

    /* --------------------------------------------------------------------- */

    Bool execute_once(const shared_ptr<Corpus2::Sentence>& sentence);
    int execute_until_done(const shared_ptr<Corpus2::Sentence>& sentence, int max_iter = 1000);

    std::string to_string(const Corpus2::Tagset& tagset) const;
  };
}

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_TAGRULESEQUENCE_I */
