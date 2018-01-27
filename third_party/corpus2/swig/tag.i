#ifndef SWIG_LIBCORPUS2_TAG_I
#define SWIG_LIBCORPUS2_TAG_I

%module libcorpustag
%{
  #include <libcorpus2/tag.h>
  #include <boost/cstdint.hpp>
%}

%include "std_string.i"
%include "std_vector.i"

/* Typedefs */
%inline %{
  typedef std::bitset<64> mask_t;
%}

%feature("notabstract") Corpus2::Tag;

%template(TagVector) std::vector<Corpus2::Tag>;

namespace Corpus2 {
  class Tag {
  public:
    Tag();
    explicit Tag(mask_t pos);
    Tag(mask_t pos, mask_t values);

    /* --------------------------------------------------------------------- */
    ~Tag();

    /* --------------------------------------------------------------------- */
    bool is_null() const;
    int pos_count() const;
    int get_pos_index() const;

    mask_t get_values() const;
    mask_t get_pos() const;

    Tag& combine_with(const Tag& other);
    Tag get_combined(const Tag& other) const;
    Tag& mask_with(const Tag& other);
    Tag get_masked(const Tag& other);

    std::string raw_dump() const;
    bool operator<(const Tag& other) const;
    bool operator==(const Tag& other) const;
  };

  size_t hash_value(const Tag &tag);

  %extend Tag {  
    long __hash__() {
      return (long) hash_value(*self);
    }
    // otherwise x != y wont trigger operator==
    %pythoncode %{
      def __ne__(self, other):
        return not self.__eq__(other)
    %}
  }

}

using namespace std;
using namespace Corpus2;


#endif /* SWIG_LIBCORPUS2_TAG_I */
