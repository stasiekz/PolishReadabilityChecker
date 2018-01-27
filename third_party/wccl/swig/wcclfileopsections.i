#ifndef SWIG_LIBWCCL_WCCLFILEOPSECTIONS_I
#define SWIG_LIBWCCL_WCCLFILEOPSECTIONS_I

%module libcclwcclfileopsections
%{
  #include <libwccl/wcclfileopsections.h>
%}

%include "bool.i"
%include "match.i"
%include "position.i"
%include "strset.i"
%include "tset.i"
%include "value.i"
%include "functionalopsequence.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>

namespace Wccl {
  template<class T> class WcclFileOpSections {
  public:
    typedef typename T::op_t op_t;
    typedef typename boost::shared_ptr<op_t> op_ptr_t;
    typedef typename boost::shared_ptr<const op_t> op_ptr_c_t;
    typedef typename boost::shared_ptr<T> ptr_t;
    typedef typename boost::shared_ptr<const T> ptr_c_t;
    typedef typename std::vector<ptr_t> ptr_v_t;
    typedef typename boost::unordered_map<std::string, ptr_t> map_t;
    typedef typename T::name_op_v_t name_op_v_t;
    typedef typename T::name_op_v_c_t name_op_v_c_t;
    /* --------------------------------------------------------------------- */

  protected:  
    bool has_section(const std::string& name) const;
    const ptr_v_t& sections();
    /* --------------------------------------------------------------------- */
  
    size_t size() const;
    bool empty() const;
    /* --------------------------------------------------------------------- */

    std::vector<std::string> section_names() const;
    /* --------------------------------------------------------------------- */

    T& get_section(const std::string& name);
    const T& get_section(const std::string& name) const;
    /* --------------------------------------------------------------------- */

    ptr_t get_section_ptr(const std::string& name);
    ptr_c_t get_section_ptr(const std::string& name) const;
    /* --------------------------------------------------------------------- */

    op_t& get_op(const std::string& name, size_t idx = 0);
    const op_t& get_op(const std::string& name, size_t idx = 0) const;
    /* --------------------------------------------------------------------- */

    op_ptr_t get_op_ptr(const std::string& name, size_t idx = 0);
    op_ptr_c_t get_op_ptr(const std::string& name, size_t idx = 0) const;
    /* --------------------------------------------------------------------- */

    name_op_v_t& add_name_op_pairs(name_op_v_t& pairs);
    name_op_v_c_t& add_name_op_pairs(name_op_v_c_t& pairs) const;
    /* --------------------------------------------------------------------- */

    name_op_v_t gen_name_op_pairs();
    name_op_v_c_t gen_name_op_pairs() const;
    /* --------------------------------------------------------------------- */
  
    WcclFileOpSections();

    void append(const ptr_t& section);
    /* --------------------------------------------------------------------- */
  };
}

%template(UntypedOpSections) Wccl::WcclFileOpSections<Wccl::UntypedOpSequence>;
%template(StrSetOpSections) Wccl::WcclFileOpSections<Wccl::OpSequence<Wccl::StrSet> >;
%template(TSetOpSections) Wccl::WcclFileOpSections<Wccl::OpSequence<Wccl::TSet> >;
%template(BoolOpSections) Wccl::WcclFileOpSections<Wccl::OpSequence<Wccl::Bool> >;
%template(PositionOpSections) Wccl::WcclFileOpSections<Wccl::OpSequence<Wccl::Position> >;
%template(MatchOpSections) Wccl::WcclFileOpSections<Wccl::OpSequence<Wccl::Match> >;


using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_WCCLFILEOPSECTIONS_I */
