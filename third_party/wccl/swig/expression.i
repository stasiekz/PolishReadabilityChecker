#ifndef SWIG_LIBWCCL_EXPRESSION_I
#define SWIG_LIBWCCL_EXPRESSION_I

%module libcclexpression
%{
  #include <libwccl/ops/expression.h>
  #include <boost/noncopyable.hpp>
%}

%include "libcorpus2/tagset.i"
%include "std_string.i"

namespace Wccl {
  class Expression {
  public:
    virtual std::string to_string(const Corpus2::Tagset& tagset) const = 0;
    std::string to_raw_string() const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_EXPRESSION_I */
