#ifndef SWIG_LIBCORPUS2_RELATION_I
#define SWIG_LIBCORPUS2_RELATION_I

%module libcorpusrelation
%{
  #include <libcorpus2_whole/relation.h>
%}

%include "std_defs.i"
%include "std_string.i"
%include "boost_shared_ptr.i"

%template(DirectionPointPtr) boost::shared_ptr<Corpus2::whole::DirectionPoint>;
%template(ConstDirectionPointPtr) boost::shared_ptr<const Corpus2::whole::DirectionPoint>;

%template(RelationPtr) boost::shared_ptr<Corpus2::whole::Relation>;
%template(ConstRelationPtr) boost::shared_ptr<const Corpus2::whole::Relation>;
%template(RelationPtrVector) std::vector< boost::shared_ptr<Corpus2::whole::Relation> >;

using std::string;

namespace Corpus2 {
namespace whole {
  class DirectionPoint {
  public:
    DirectionPoint(const std::string, const std::string, const int);

    const std::string sentence_id() const;
    const std::string channel_name() const;
    int annotation_number() const;
  };

  class Relation {
  public:
    /*
    Relation(const std::string& name,
       const std::string& set,
       const boost::shared_ptr<const DirectionPoint> from,
       const boost::shared_ptr<const DirectionPoint> to);
    */
    Relation(const std::string& name,
       const std::string& set,
       const DirectionPoint& from,
       const DirectionPoint& to);

    /*
    Relation(const std::string& name,
       const boost::shared_ptr<const DirectionPoint> from,
       const boost::shared_ptr<const DirectionPoint> to);
    */
    Relation(const std::string& name,
       const DirectionPoint& from,
       const DirectionPoint& to);
    ~Relation();

    void set_from(const DirectionPoint& dp);
    void set_to(const DirectionPoint& dp);
    void set_name(const std::string& s);
    void set_set(const std::string& s);

    boost::shared_ptr<Relation> clone_shared();
    /* It must be renamed because "from" is python keyword */
    %rename(rel_from) from() const;
    const boost::shared_ptr<const DirectionPoint>& from() const;
    /* Also "to" for consistency should be renamed to rel_to */
    %rename(rel_to) to() const;
    const boost::shared_ptr<const DirectionPoint>& to() const;
    /* ...and "name" to rel_name */
    %rename(rel_name) name() const;
    const std::string name() const;
    /* ...and "set" to rel_set */
    %rename(rel_set) set() const;
    const std::string set() const;
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_RELATION_I */
