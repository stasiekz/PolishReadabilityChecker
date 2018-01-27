#ifndef SWIG_LIBWCCL_FUNCTIONALOPERATOR_I
#define SWIG_LIBWCCL_FUNCTIONALOPERATOR_I

%module libcclfunctionaloperator
%{
  #include <libwccl/ops/operator.h>
%}

%include "value.i"
%include "variables.i"
%include "sentencecontext.i"
%include "parsedexpression.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%template(FunctionalOperatorPtr) boost::shared_ptr<Wccl::FunctionalOperator>;
%template(ConstFunctionalOperatorPtr) boost::shared_ptr<const Wccl::FunctionalOperator>;

namespace Wccl {
  class FunctionalOperator : public ParsedExpression {
  public:
    virtual boost::shared_ptr<const Value> base_apply(const SentenceContext& sc) = 0;

    /* --------------------------------------------------------------------- */
    boost::shared_ptr<FunctionalOperator> clone_ptr() const;
    boost::shared_ptr<FunctionalOperator> clone_clean_ptr() const;

  protected:
    explicit FunctionalOperator(const Variables& variables);
    FunctionalOperator* clone_internal() const = 0;
  };
}

using namespace boost;
using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_FUNCTIONALOPERATOR_I */
