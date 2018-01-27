#ifndef SWIG_LIBPWRNLP_PWRNLPERROR_I
#define SWIG_LIBPWRNLP_PWRNLPERROR_I

%module libpwrnlperror
%{
  #include <libpwrutils/exception.h>
%} 

%include "std_string.i"
%include "std_except.i"

%nodefaultctor PwrNlp::PwrNlpError;

namespace PwrNlp {
  class PwrNlpError : public std::runtime_error {
  public:
    PwrNlpError(const std::string &what);

    /* --------------------------------------------------------------------- */
    ~PwrNlpError() throw();

    virtual std::string info() const;
    virtual std::string scope() const;
  };
}

using namespace std;
using namespace PwrNlp;

#endif /* SWIG_LIBPWRNLP_PWRNLPERROR_I */
