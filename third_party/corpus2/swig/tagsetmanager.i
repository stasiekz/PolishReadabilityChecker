#ifndef SWIG_LIBCORPUS2_TAGSETMANAGER_I
#define SWIG_LIBCORPUS2_TAGSETMANAGER_I

%module libcorpustagsetmanager
%{
  #include <libcorpus2/tagsetmanager.h>
%}

%include "tagset.i"
%include "exception.i"

%include "std_string.i"
%include "std_vector.i"
%include "boost_shared_ptr.i"

%feature("notabstract") Corpus2::TagsetManager;

namespace Corpus2 {
  class TagsetNotFound : public Corpus2Error {
  public:
    // explicit TagsetNotFound(const tagset_idx_t id);
    ~TagsetNotFound() throw() {}

    /* --------------------------------------------------------------------- */
    std::string info() const;

    /* --------------------------------------------------------------------- */
    // tagset_idx_t id;
  };

  class TagsetManager {
  public:
    TagsetManager();
    ~TagsetManager();

    /* --------------------------------------------------------------------- */
    const Tagset& get_tagset(const std::string& name);
    boost::shared_ptr<Tagset> get_cache_entry(const std::string& name);

    /* --------------------------------------------------------------------- */
  };


  %exception {
    try {
      $action
    } catch (Corpus2::TagsetNotFound &e) {
      PyErr_SetString(PyExc_IndexError, e.info().c_str());
      return NULL;
    } catch (PwrNlp::PwrNlpError &e) {
      PyErr_SetString(PyExc_IndexError, e.info().c_str());
      return NULL;
    }
  }
  inline const Tagset& get_named_tagset(const std::string& name);
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_TAGSETMANAGER_I */
