#ifndef SWIG_LIBCORPUS2_TAGSET_I
#define SWIG_LIBCORPUS2_TAGSET_I

%module libcorpustagset
%{
  #include <libcorpus2/tagset.h>
%}

%include "tag.i"
%include "exception.i"

%include "std_defs.i"
%include "std_string.i"
%include "std_vector.i"

%feature("notabstract") Corpus2::Tagset;

%template(TagsetPtr) boost::shared_ptr<Corpus2::Tagset>;
%template(ConstTagsetPtr) boost::shared_ptr<const Corpus2::Tagset>;

namespace Corpus2 {
  class Tagset;
  class TagParseError : public Corpus2Error {
  public:
    TagParseError(const std::string &what, const std::string& val,
        const std::string& tag, const std::string& tagset);

    ~TagParseError() throw() {}

    /* --------------------------------------------------------------------- */
    std::string info() const;
    std::string val, tag, tagset;
  }; // TagParseError

  class TagsetMismatch : public Corpus2Error {
  public:
    TagsetMismatch(const std::string& where, const Tagset& expected,
        const Tagset& actual);

    // TagsetMismatch(const std::string& where, tagset_idx_t expected,
    //    tagset_idx_t actual);

    ~TagsetMismatch() throw() {}

    /* --------------------------------------------------------------------- */
    std::string info() const;

    /* --------------------------------------------------------------------- */
    // tagset_idx_t expected_id, actual_id;
  }; // TagsetMismatch

  class Tagset {
  public:

    enum ParseMode {
      ParseCheckRequired = 1,
      ParseAllowExtra = 2,
      ParseCheckSingular = 4,

      ParseDefault = ParseCheckRequired, 
      ParseRequiredWithExtra = ParseCheckRequired | ParseAllowExtra,
      ParseStrict = ParseCheckRequired | ParseCheckSingular,
      ParseLoose = ParseAllowExtra
    };

    Tagset();
    static Tagset from_data(const char*);

    /* --------------------------------------------------------------------- */
    Tag parse_symbol(const std::string& s) const;
    Tag parse_symbol_string(const std::string& s) const;

    /* --------------------------------------------------------------------- */
    std::vector<Tag> parse_tag(const char* c, ParseMode m = ParseDefault) const;
    Tag parse_simple_tag(const char* c, ParseMode mode = ParseDefault) const;
    Tag make_ign_tag() const;

    /* --------------------------------------------------------------------- */
    std::string tag_to_string(const Tag& tag) const;
    std::string tag_to_no_opt_string(const Tag &tag) const;

    /* --------------------------------------------------------------------- */
    std::vector<std::string> tag_to_symbol_string_vector(const Tag& tag, bool 
      compress_attribites = true) const;

    std::string tag_to_symbol_string(const Tag& tag,
      bool compress_attribites = true) const;

    /* --------------------------------------------------------------------- */
    size_t tag_size(const Tag& tag) const;
    bool tag_is_singular(const Tag& tag) const;

    /* --------------------------------------------------------------------- */
    std::vector<Tag> split_tag(const Tag& tag) const;
    Tag select_singular(const Tag& tag) const;
    Tag expand_optional_attrs(const Tag& tag) const;
    /* --------------------------------------------------------------------- */
    int pos_count() const;
    int attribute_count() const;
    int value_count() const;
    size_t size() const;
    
    double size_extra() const;

    /* --------------------------------------------------------------------- */
    std::string name() const;
    void set_name(const std::string& name);


    std::string id_string() const;
    std::string id_string(const Tag& tag) const;

    const std::string& get_value_name(int index) const;
    const std::string& get_attribute_name(int index) const;
    const std::string& get_pos_name(int index) const;

  };
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_TAGSET_I */
