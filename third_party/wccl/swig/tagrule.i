#ifndef SWIG_LIBWCCL_TAGRULE_I
#define SWIG_LIBWCCL_TAGRULE_I

%module libccltagrule
%{
  #include <libwccl/ops/tagrule.h>
%}

%include "bool.i"
%include "tagaction.i"
%include "variables.i"
%include "sentencecontext.i"
%include "parsedexpression.i"

%include "libcorpus2/tagset.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>

%template(TagRulePtr) boost::shared_ptr<Wccl::TagRule>;

%template(TagRuleVector) std::vector<Wccl::TagRule>;

namespace Wccl {
  class TagRule : public ParsedExpression {
  public:
    /*
    TagRule(
      const std::string& name,
      const Variables& variables,
      const boost::shared_ptr<const std::vector<boost::shared_ptr<TagAction> > >& actions,
      const boost::shared_ptr<const Function<Bool> >& condition = TrueCondition()
    );
    */
  
    TagRule();
    TagRule(const TagRule& other, bool clean = false);

    /* --------------------------------------------------------------------- */

    %rename(OpFunTagRule) operator()(const SentenceContext& sentence_context);
    Bool operator()(SentenceContext& sentence_context);
    /* --------------------------------------------------------------------- */

    Bool execute(SentenceContext& sentence_context);

    std::string name() const;
    /* --------------------------------------------------------------------- */

    TagRule clone() const;
    TagRule clone_clean() const;
    /* --------------------------------------------------------------------- */

    shared_ptr<TagRule> clone_ptr() const;
    shared_ptr<TagRule> clone_clean_ptr() const;
    /* --------------------------------------------------------------------- */

    %rename(OperatorEqTagRule) *::operator=(const TagRule& other);
    /* --------------------------------------------------------------------- */

    std::string to_string(const Corpus2::Tagset& tagset) const;
    /* --------------------------------------------------------------------- */

  protected:
    TagRule* clone_internal() const;

  private:
    // static const boost::shared_ptr<const Function<Bool> > TrueCondition();
  };
}

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_TAGRULE_I */
