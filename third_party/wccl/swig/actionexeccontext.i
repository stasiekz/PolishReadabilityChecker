#ifndef SWIG_LIBWCCL_ACTIONEXECCONTEXT_I
#define SWIG_LIBWCCL_ACTIONEXECCONTEXT_I

%module libcclactionexeccontext
%{
  #include <libwccl/ops/tagaction.h>
%}

%include "variables.i"
%include "sentencecontext.i"

namespace Wccl {
  class ActionExecContext {
  public:
    ActionExecContext(
      SentenceContext& sentence_context, 
      const boost::shared_ptr<Variables>& vars);
    /* --------------------------------------------------------------------- */

    SentenceContext& sentence_context() const;
    /* --------------------------------------------------------------------- */

    const boost::shared_ptr<Variables>& variables() const;
  };
}

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_ACTIONEXECCONTEXT_I */
