#ifndef SWIG_LIBWCCL_VARIABLES_I
#define SWIG_LIBWCCL_VARIABLES_I

%module libcclvariables
%{
  #include <libwccl/variables.h>
%}

%include "value.i"
%include "match.i"
%include "bool.i"
%include "position.i"
%include "strset.i"
%include "tset.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>


%template(VariablesPtr) boost::shared_ptr<Wccl::Variables>;
%template(ConstVariablesPtr) boost::shared_ptr<Wccl::Variables const>;

namespace Wccl {
  /* ----------------------------------------------------------------------- */
  /* Helper detail class */
  namespace detail {
    template<class T> class Vmap {
      public:
        typedef std::map< std::string, boost::shared_ptr<T> > map_t;
    };

    // %template(VmapBool) Vmap<Bool>;

  } // detail ns

  /* ----------------------------------------------------------------------- */
  /* Base Accessor */
  class BaseAccessor {
  public:
    const std::string get_name();

  protected:
    BaseAccessor(const std::string& varname);
  };

  /* ----------------------------------------------------------------------- */
  /* Variable Accessor */
  template<typename T>
  class VariableAccessor : public BaseAccessor {
    public:
  };

  /* ----------------------------------------------------------------------- */
  /* Variables */
  class Variables /*: 
    detail::Vmap<Value>, 
    detail::Vmap<Bool>, 
    detail::Vmap<Position>, 
    detail::Vmap<StrSet>, 
    detail::Vmap<TSet>, 
    detail::Vmap<Match> */
  {
  public:
    Variables();

    template<typename T> int size() const;

    void reset_values();

    template<typename T> const typename detail::Vmap<T>::map_t get_all() const;

    Variables* clone() const;

    template<typename T> boost::shared_ptr<T> get(const std::string& s) const;

    template<typename T> boost::shared_ptr<T> get_or_throw(const std::string& s) const;

    template<typename T> const typename T::value_type& get_value(const std::string& s) const;

    template<typename T> VariableAccessor<T> create_accessor(const std::string& s);

    template<typename T> boost::shared_ptr<T> get_fast(const VariableAccessor<T>& a) const;

    template<typename T> boost::shared_ptr<T> get_put(const std::string& s); 

    template<typename T> void put(const std::string& s, const boost::shared_ptr<T>& v);

    void put_any(const std::string& s, const boost::shared_ptr<Value>& v);

    template<typename T> bool del_variable(const std::string& s);

    bool del_any(const std::string& s);

    template<typename T> void put(const std::string& s, T* v);

    template<typename T> void set(const std::string& s, const T& v);
  };

  %extend Variables {
    %template(size_bool) size<Bool>;
    %template(size_tset) size<TSet>;
    %template(size_strset) size<StrSet>;
    %template(size_position) size<Position>;
    %template(size_match) size<Match>;

    %template(get_all_bool) get_all<Bool>;
    %template(get_all_tset) get_all<TSet>;
    %template(get_all_strset) get_all<StrSet>;
    %template(get_all_position) get_all<Position>;
    %template(get_all_match) get_all<Match>;

    %template(get_bool) get<Bool>;
    %template(get_tset) get<TSet>;
    %template(get_strset) get<StrSet>;
    %template(get_position) get<Position>;
    %template(get_match) get<Match>;

    %template(get_or_throw_bool) get_or_throw<Bool>;
    %template(get_or_throw_tset) get_or_throw<TSet>;
    %template(get_or_throw_strset) get_or_throw<StrSet>;
    %template(get_or_throw_position) get_or_throw<Position>;
    %template(get_or_throw_match) get_or_throw<Match>;

    %template(get_value_bool) get_value<Bool>;
    %template(get_value_tset) get_value<TSet>;
    %template(get_value_strset) get_value<StrSet>;
    %template(get_value_position) get_value<Position>;
    // %template(get_value_match) get_value<Match>;

    %template(create_accessor_bool) create_accessor<Bool>;
    %template(create_accessor_tset) create_accessor<TSet>;
    %template(create_accessor_strset) create_accessor<StrSet>;
    %template(create_accessor_position) create_accessor<Position>;
    %template(create_accessor_match) create_accessor<Match>;

    %template(get_fast_bool) get_fast<Bool>;
    %template(get_fast_tset) get_fast<TSet>;
    %template(get_fast_strset) get_fast<StrSet>;
    %template(get_fast_position) get_fast<Position>;
    %template(get_fast_match) get_fast<Match>;

    %template(get_put_bool) get_put<Bool>;
    %template(get_put_tset) get_put<TSet>;
    %template(get_put_strset) get_put<StrSet>;
    %template(get_put_position) get_put<Position>;
    %template(get_put_match) get_put<Match>;

    %template(put_bool_2) put<Bool>;
    %template(put_tset_2) put<TSet>;
    %template(put_strset_2) put<StrSet>;
    %template(put_position_2) put<Position>;
    %template(put_match_2) put<Match>;

    %template(del_bool) del_variable<Bool>;
    %template(del_tset) del_variable<TSet>;
    %template(del_strset) del_variable<StrSet>;
    %template(del_position) del_variable<Position>;
    %template(del_match) del_variable<Match>;

    %template(put_bool) put<Bool>;
    %template(put_tset) put<TSet>;
    %template(put_strset) put<StrSet>;
    %template(put_position) put<Position>;
    %template(put_match) put<Match>;

    %template(set_bool) set<Bool>;
    %template(set_tset) set<TSet>;
    %template(set_strset) set<StrSet>;
    %template(set_position) set<Position>;
    %template(set_match) set<Match>;
  }
} // namespace Wccl;

using namespace boost;
using namespace std;
using namespace Wccl;
using namespace detail;

#endif /* SWIG_LIBWCCL_VARIABLES_I */
