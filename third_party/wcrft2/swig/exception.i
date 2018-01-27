#ifndef SWIG_WCRFT2_EXCEPTION_I
#define SWIG_WCRFT2_EXCEPTION_I

%module wcrft2exception
%{
  #include <libwcrft/exception.h>
%}

%include "std_string.i"

%include "libcorpus2/libpwrnlperror.i"

namespace Wcrft {
  class WcrftError : public PwrNlp::PwrNlpError {
  public:
    WcrftError(const std::string &what);
    ~WcrftError() throw();

    /* --------------------------------------------------------------------- */
    std::string scope() const;
  };

%rename(WcrftFileNotFound) FileNotFound;
  class FileNotFound : public WcrftError {
  public:
    FileNotFound(const std::string& filename, const std::string& paths, 
        const std::string& where);

    ~FileNotFound() throw();

    /* --------------------------------------------------------------------- */
    std::string info() const;
    std::string filename, paths, where;
  };
}

using namespace std;
using namespace Wcrft;

#endif
