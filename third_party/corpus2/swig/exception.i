#ifndef SWIG_LIBCORPUS2_EXCEPTION_I
#define SWIG_LIBCORPUS2_EXCEPTION_I

%module libcorpusexception
%{
  #include <libcorpus2/exception.h>
%}

%include "std_string.i"

%include "libpwrnlperror.i"

namespace Corpus2 {
  class Corpus2Error : public PwrNlp::PwrNlpError {
  public:
    Corpus2Error(const std::string &what);
    ~Corpus2Error() throw();

    /* --------------------------------------------------------------------- */
    std::string scope() const;
  };

  class FileNotFound : public Corpus2Error {
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
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_EXCEPTION_I */
