#ifndef SWIG_LIBWCCL_OPERATOR_I
#define SWIG_LIBWCCL_OPERATOR_I

%module libccloperator
%{
  #include <libwccl/ops/operator.h>
%}


%include "value.i"
%include "sentencecontext.i"
%include "functionaloperator.i"

/*
%include "variables.i"
%include "parsedexpression.i"
*/

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%feature("notabstract") Wccl::Operator;

namespace Wccl {
  template <class T> class Operator : public FunctionalOperator {
  public:
    Operator();
    Operator(const Operator& other, bool clean = false);

    /* --------------------------------------------------------------------- */
    %rename(OperatorEq) *::operator=(const Operator& other);

    /* --------------------------------------------------------------------- */
    %rename(OpFun) operator()(const SentenceContext& sentence_context);
    boost::shared_ptr<const T> operator()(const SentenceContext& sentence_context);

    /* --------------------------------------------------------------------- */
    boost::shared_ptr<const T> apply(const SentenceContext& sentence_context);
    boost::shared_ptr<T> copy_apply(const SentenceContext& sentence_context);
    boost::shared_ptr<const Value> base_apply(const SentenceContext& sc);
    
    /* --------------------------------------------------------------------- */
    // Operator clone() const;
    // Operator clone_clean() const;
    
    /* --------------------------------------------------------------------- */
    boost::shared_ptr<Operator<T> > clone_ptr() const;
    boost::shared_ptr<Operator<T> > clone_clean_ptr() const;

    /* --------------------------------------------------------------------- */
    std::string to_string(const Corpus2::Tagset& tagset) const;

  protected:
    Operator* clone_internal() const;
    std::ostream& write_to(std::ostream& ostream) const;
  };
}

%template (BoolOperator) Wccl::Operator<Wccl::Bool>;
%template (TSetOperator) Wccl::Operator<Wccl::TSet>;
%template (StrSetOperator) Wccl::Operator<Wccl::StrSet>;
%template (PositionOperator) Wccl::Operator<Wccl::Position>;

%template (BoolOperatorPtr) boost::shared_ptr<Wccl::Operator<Wccl::Bool> >;
%template (TSetOperatorPtr) boost::shared_ptr<Wccl::Operator<Wccl::TSet> >;
%template (StrSetOperatorPtr) boost::shared_ptr<Wccl::Operator<Wccl::StrSet> >;
%template (PositionOperatorPtr) boost::shared_ptr<Wccl::Operator<Wccl::Position> >;

using namespace boost;
using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_OPERATOR_I */
