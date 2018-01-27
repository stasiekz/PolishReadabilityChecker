#ifndef SWIG_LIBWCCL_LEXICON_I
#define SWIG_LIBWCCL_LEXICON_I

%module libccllexicon
%{
  #include <libwccl/lexicon/lexicon.h>

  #include <libcorpus2/lexeme.h> 
  #include <boost/unordered_map.hpp>
%}

%include "strset.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%rename(map_t) boost::unordered_map<UnicodeString, UnicodeString>;

%nodefaultctor Wccl::Lexicon;

%template(LexiconPtr) boost::shared_ptr<Wccl::Lexicon>;
%template(ConstLexiconPtr) boost::shared_ptr<const Wccl::Lexicon>;

namespace Wccl {
  class Lexicon {
  public:
    typedef boost::unordered_map<UnicodeString, UnicodeString> map_t;

    /* --------------------------------------------------------------------- */

    Lexicon(const std::string& name, const std::string& file_name);

    /* --------------------------------------------------------------------- */

    boost::shared_ptr<StrSet> translate(const UnicodeString& key) const;
    // boost::shared_ptr<StrSet> translate(const StrSet& set) const;

    // TODO
    // std::string translate_utf8(const std::string&);

    /* --------------------------------------------------------------------- */

    std::string name() const;
    std::string file_name() const;

    /* --------------------------------------------------------------------- */

    bool has_key(const UnicodeString& key) const;

    // TODO
    // bool has_key_utf8(const std::string& key) const 
    
    /* --------------------------------------------------------------------- */

    void insert(const UnicodeString& key, const UnicodeString& value);
    void insert(const UnicodeString& key);
    
    // TODO
    // void insert_utf8(const std::string& key, const std::string& value);
    // void insert_utf8(const std::string& key);

    /* --------------------------------------------------------------------- */

    const map_t& map() const;
  };

}

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_LEXICON_I */
