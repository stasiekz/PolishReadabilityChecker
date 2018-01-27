#ifndef SWIG_LIBCORPUS2_TOKENMETADATA_I
#define SWIG_LIBCORPUS2_TOKENMETADATA_I

%module libcorpustokenmetadata
%{
  #include <libcorpus2/tokenmetadata.h>
%}

%include "std_map.i"
%include "std_string.i"

%include "std_defs.i"
%include "boost_shared_ptr.i"

%feature("notabstract") Corpus2::TokenMetaData;

%template(TokenMetaDataPtr) boost::shared_ptr<Corpus2::TokenMetaData>;
%template(ConstTokenMetaDataPtr) boost::shared_ptr<const Corpus2::TokenMetaData>;

namespace Corpus2 {
  class TokenMetaData {
  public:
    TokenMetaData();
    ~TokenMetaData();
    
    boost::shared_ptr<TokenMetaData> clone() const;
    bool has_attribute(const std::string& name) const;
    std::string get_attribute(const std::string& name) const;
    void set_attribute(const std::string& name, const std::string& value);

    typedef std::map<std::string, std::string> attr_map_t;
    const attr_map_t& attributes() const;
  };
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_TOKENMETADATA_I */
