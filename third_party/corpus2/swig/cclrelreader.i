#ifndef SWIG_LIBCORPUS2_DOCUMENTREADER_I
#define SWIG_LIBCORPUS2_DOCUMENTREADER_I

%module libcorpuscclrelreader
%{
  #include <libcorpus2_whole/io/cclrelreader.h>
%}

%include "exception.i"
%include "document.i"
%include "boost_shared_ptr.i"

namespace Corpus2 {
namespace whole {
  class CclRelReader {
  public:
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    CclRelReader(const Tagset&, const std::string&, const std::string &);

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    boost::shared_ptr<Document> read();

    void set_option(const std::string& option);
    std::string get_option(const std::string& option) const;

    /* --------------------------------------------------------------------- */
    ~CclRelReader();
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_DOCUMENTREADER_I */
