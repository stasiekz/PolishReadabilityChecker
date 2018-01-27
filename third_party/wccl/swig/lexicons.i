#ifndef SWIG_LIBWCCL_LEXICONS_I
#define SWIG_LIBWCCL_LEXICONS_I

%module libccllexicons
%{
  #include <libwccl/lexicon/lexicons.h>
%}

%include "lexicon.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%rename(map_t) boost::unordered_map<std::string, boost::shared_ptr<Lexicon> >;

%template(LexiconsPtr) boost::shared_ptr<Wccl::Lexicons>;
%template(ConstLexiconsPtr) boost::shared_ptr<const Wccl::Lexicons>;

namespace Wccl {
  class Lexicons {
  public:
    typedef unordered_map<std::string, shared_ptr<Lexicon> > map_t;

    /* --------------------------------------------------------------------- */

    Lexicons();

    /* --------------------------------------------------------------------- */

    bool has_lexicon(const std::string& name) const;

    /* --------------------------------------------------------------------- */


    %rename(get_lexicon) get(const std::string&) const;
    const Lexicon& get(const std::string& name) const;
    shared_ptr<const Lexicon> get_ptr(const std::string& name) const;

    /* --------------------------------------------------------------------- */

    void insert(const shared_ptr<Lexicon>& lexicon);

    /* --------------------------------------------------------------------- */

    const map_t& get_lexicons() const;
  };


}

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_LEXICONS_I */
