#ifndef SWIG_LIBWCCL_FUNCTIONALOPSEQUENCE_I
#define SWIG_LIBWCCL_FUNCTIONALOPSEQUENCE_I

%module libcclfunctionalopsequence
%{
  #include <libwccl/ops/opsequence.h>
%}

%include "operator.i"
%include "expression.i"
%include "functionaloperator.i"

%include "std_pair.i"
%include "std_string.i"
%include "std_vector.i"

%include <libcorpus2/boost_shared_ptr.i>

%nodefaultctor Wccl::FunctionalOpSequence;

%template(name_op_pair_t) std::pair<std::string, boost::shared_ptr<Wccl::FunctionalOperator> > ;
%template(name_op_pair_c_t) std::pair<std::string, boost::shared_ptr<const Wccl::FunctionalOperator> >;
%template(name_op_v_t) std::vector<std::pair<std::string, boost::shared_ptr<Wccl::FunctionalOperator> > >;
%template(name_op_v_c_t) std::vector<std::pair<std::string, boost::shared_ptr<const Wccl::FunctionalOperator> > >;

namespace Wccl {
  class FunctionalOpSequence : public Expression {
  public:
    typedef shared_ptr<FunctionalOperator> fun_op_ptr_t;
    typedef shared_ptr<const FunctionalOperator> fun_op_ptr_c_t;
    typedef std::pair<std::string, fun_op_ptr_t> name_op_pair_t;
    typedef std::pair<std::string, fun_op_ptr_c_t> name_op_pair_c_t;
    typedef std::vector<name_op_pair_t> name_op_v_t;
    typedef std::vector<name_op_pair_c_t> name_op_v_c_t;
    /* --------------------------------------------------------------------- */

    std::string name() const;
    /* --------------------------------------------------------------------- */

    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    /* --------------------------------------------------------------------- */

    //virtual FunctionalOperator& get(size_t idx) = 0;
    //virtual const FunctionalOperator& get(size_t idx) const = 0;
    /* --------------------------------------------------------------------- */

    virtual fun_op_ptr_t get_untyped_ptr(size_t idx) = 0;
    /* --------------------------------------------------------------------- */

    name_op_v_t& add_name_op_pairs_untyped(name_op_v_t& pairs);
    /* --------------------------------------------------------------------- */

    name_op_v_t gen_name_op_pairs_untyped();
    /* --------------------------------------------------------------------- */

    std::string gen_op_name(size_t idx) const;
    /* --------------------------------------------------------------------- */

    name_op_pair_t gen_name_op_pair_untyped(size_t idx);
    /* --------------------------------------------------------------------- */
  
    shared_ptr<FunctionalOpSequence> clone() const;
    /* --------------------------------------------------------------------- */

    virtual ~FunctionalOpSequence();
    /* --------------------------------------------------------------------- */

  protected:
    FunctionalOpSequence(const std::string& name);
    virtual FunctionalOpSequence* clone_internal() const = 0;
  }; // FunctionalOpSequence

  /* ----------------------------------------------------------------------- */


  class UntypedOpSequence : public FunctionalOpSequence {
  public:
    typedef FunctionalOperator op_t;

    UntypedOpSequence(const std::string& name);
    /* --------------------------------------------------------------------- */

    bool empty() const;
    size_t size() const;
    /* --------------------------------------------------------------------- */

    void append(const fun_op_ptr_t& op);
    /* --------------------------------------------------------------------- */

    //op_t& get(size_t idx);
    //const op_t& get(size_t idx) const;
    /* --------------------------------------------------------------------- */

    fun_op_ptr_t get_untyped_ptr(size_t idx);
    /* --------------------------------------------------------------------- */

    fun_op_ptr_t get_ptr(size_t idx);
    /* --------------------------------------------------------------------- */

    name_op_v_t& add_name_op_pairs(name_op_v_t& pairs);
    /* --------------------------------------------------------------------- */

    name_op_v_t gen_name_op_pairs();
    /* --------------------------------------------------------------------- */

    name_op_pair_t gen_name_op_pair(size_t idx);
    /* --------------------------------------------------------------------- */

    std::string to_string(const Corpus2::Tagset& tagset) const;
    /* --------------------------------------------------------------------- */

    UntypedOpSequence(const UntypedOpSequence& seq);
    /* --------------------------------------------------------------------- */

    shared_ptr<UntypedOpSequence> clone() const;
    /* --------------------------------------------------------------------- */

  protected:
    std::ostream& write_to(std::ostream& ostream) const;
    UntypedOpSequence* clone_internal() const;
  }; // UntypedOpSequence
    
  /* ----------------------------------------------------------------------- */

  %rename(op_t) Wccl::Operator<T>;
  %rename(op_ptr_t) boost::shared_ptr<Wccl::Operator<T> >;
  %rename(op_ptr_c_t) boost::shared_ptr<const Wccl::Operator<T> >;

  template<class T> class OpSequence : public FunctionalOpSequence {
  public:
    typedef Operator<T> op_t;
    typedef typename shared_ptr<Operator<T> > op_ptr_t;
    typedef typename shared_ptr<const Operator<T> > op_ptr_c_t;
    typedef typename std::pair<std::string, op_ptr_t> name_op_pair_t;
    typedef typename std::pair<std::string, op_ptr_c_t> name_op_pair_c_t;
    typedef typename std::vector<name_op_pair_t> name_op_v_t;
    typedef typename std::vector<name_op_pair_c_t> name_op_v_c_t;
    /* --------------------------------------------------------------------- */

    OpSequence(const std::string& name);
    /* --------------------------------------------------------------------- */

    bool empty() const;
    size_t size() const;
    /* --------------------------------------------------------------------- */

    void append(const op_ptr_t& op);
    /* --------------------------------------------------------------------- */

    //op_t& get(size_t idx);
    //const op_t& get(size_t idx) const;
    /* --------------------------------------------------------------------- */

    op_ptr_t get_ptr(size_t idx);
    /* --------------------------------------------------------------------- */

    FunctionalOpSequence::fun_op_ptr_t get_untyped_ptr(size_t idx);
    /* --------------------------------------------------------------------- */

    name_op_v_t gen_name_op_pairs();
/*     name_op_v_c_t gen_name_op_pairs() const; */
    /* --------------------------------------------------------------------- */

    name_op_v_t& add_name_op_pairs(name_op_v_t& pairs);
/*     name_op_v_c_t& add_name_op_pairs(name_op_v_c_t& pairs) const; */
    /* --------------------------------------------------------------------- */

    name_op_pair_t gen_name_op_pair(size_t idx);
    /* --------------------------------------------------------------------- */

    std::string to_string(const Corpus2::Tagset& tagset) const;
    /* --------------------------------------------------------------------- */

    OpSequence(const OpSequence<T>& seq);
    /* --------------------------------------------------------------------- */

    shared_ptr<OpSequence<T> > clone() const;
    /* --------------------------------------------------------------------- */

  protected:
    std::ostream& write_to(std::ostream& os) const;
    virtual OpSequence<T>* clone_internal() const;
  }; // OpSequence
}

%template(BoolOpSequence) Wccl::OpSequence<Wccl::Bool>;
%template(StrSetOpSequence) Wccl::OpSequence<Wccl::StrSet>;
%template(TSetOpSequence) Wccl::OpSequence<Wccl::TSet>;
%template(PositionOpSequence) Wccl::OpSequence<Wccl::Position>;

%template(UntypedOpSequencePtr) boost::shared_ptr<Wccl::UntypedOpSequence>;
%template(BoolOpSequencePtr) boost::shared_ptr<Wccl::OpSequence<Wccl::Bool> >;
%template(StrSetOpSequencePtr) boost::shared_ptr< Wccl::OpSequence<Wccl::StrSet> >;
%template(TSetOpSequencePtr) boost::shared_ptr<Wccl::OpSequence<Wccl::TSet> >;
%template(PositionOpSequencePtr) boost::shared_ptr<Wccl::OpSequence<Wccl::Position> >;

%template(UntypedOpSequencePtrVector) std::vector<boost::shared_ptr<Wccl::UntypedOpSequence> >;
%template(BoolOpSequencePtrVector) std::vector<boost::shared_ptr<Wccl::OpSequence<Wccl::Bool> > >;
%template(StrSetOpSequencePtrVector) std::vector<boost::shared_ptr<Wccl::OpSequence<Wccl::StrSet> > >;
%template(TSetOpSequencePtrVector) std::vector<boost::shared_ptr<Wccl::OpSequence<Wccl::TSet> > >;
%template(PositionOpSequencePtrVector) std::vector<boost::shared_ptr<Wccl::OpSequence<Wccl::Position> > >;


using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_FUNCTIONALOPSEQUENCE_I */
