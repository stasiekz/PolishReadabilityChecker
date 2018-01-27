#ifndef SWIG_LIBWCCL_TAGACTION_I
#define SWIG_LIBWCCL_TAGACTION_I

%module libccltagaction
%{
  #include <libwccl/ops/tagaction.h>
%}

%include "bool.i"
%include "expression.i"
%include "actionexeccontext.i"

%include "std_string.i"

%nodefaultctor Wccl::TagAction;

namespace Wccl {
  class TagAction : public Expression {
  public:
    virtual std::string name() const = 0;
    virtual Bool execute(const ActionExecContext& context) const = 0;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_TAGACTION_I */
