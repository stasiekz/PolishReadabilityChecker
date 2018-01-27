#ifndef LIBWCCL_EXCEPTION_H
#define LIBWCCL_EXCEPTION_H

%module libwcclexception
%{
  #include <libpwrutils/exception.h>
%}

%include "std_string.i"

%include "libpwrnlperror.i"

namespace Wccl {
  class WcclError : public PwrNlp::PwrNlpError {
  public:
    WcclError(const std::string &what);
    ~WcclError() throw();

    /* --------------------------------------------------------------------- */
    std::string scope() const;
  };

  class InvalidArgument : public WcclError {
  public:
    InvalidArgument(const std::string& arg_name, const std::string& reason);

    ~InvalidArgument() throw();

    /* --------------------------------------------------------------------- */
    std::string info() const;
    std::string arg_name;
    std::string reason;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_EXCEPTION_I */