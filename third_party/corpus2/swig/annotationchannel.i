#ifndef SWIG_LIBCORPUS2_ANNOTATIONCHANNEL_I
#define SWIG_LIBCORPUS2_ANNOTATIONCHANNEL_I

%module libcorpusannotationchannel
%{
  #include <libcorpus2/ann/channel.h>
%}

%include "std_string.i"
%include "std_vector.i"
%include "boost_shared_ptr.i"

%include "iob.i"
%include "std_defs.i"

%template(chan_map_t) std::map<std::string, Corpus2::AnnotationChannel>;

%template(AnnotationoPtr) boost::shared_ptr<Corpus2::Annotation>;
%template(ConstAnnotationoPtr) boost::shared_ptr<const Corpus2::Annotation>;

%template(AnnotationVector) std::vector<Corpus2::Annotation>;

%template(AnnotationChannelPtr) boost::shared_ptr<Corpus2::AnnotationChannel>;
%template(ConstAnnotationChannelPtr) boost::shared_ptr<const Corpus2::AnnotationChannel>;

namespace Corpus2 {
  struct Annotation {
    Annotation() : indices(), head_index(-1), seg_number(0) {}
    std::vector<int> indices;
    int head_index;
    int seg_number;
    bool empty() const;
    bool sane() const;
  };

  class AnnotationChannel {
  public:
    AnnotationChannel();
    explicit AnnotationChannel(int size);
    ~AnnotationChannel();
    
    /* --------------------------------------------------------------------- */

    int size() const;
    void resize(int size);

    void make_iob_from_segments();
    void make_segments_from_iob();

    int renumber_segments();
    int get_new_segment_index() const;

    /* --------------------------------------------------------------------- */
    enum AnnotationVectorMode {
      O_DISJOINT_EXCLUSIVE   = 0,
      O_CONTINUOUS           = 1,
      O_INCLUSIVE            = 2,
      O_CONTINUOUS_INCLUSIVE = 3
    };

    std::vector<Annotation> make_annotation_vector(
      AnnotationVectorMode mode = O_DISJOINT_EXCLUSIVE) const;

    /* --------------------------------------------------------------------- */
    std::vector<int>& segments();
    const std::vector<int>& segments() const;

    int get_segment_at(int idx) const;
    void set_segment_at(int token_idx, int segment_idx);
    
    /* --------------------------------------------------------------------- */
    const std::vector<IOB::Enum>& iobs() const;
    IOB::Enum get_iob_at(int idx);

    /* --------------------------------------------------------------------- */
    void set_iob_at(int idx, IOB::Enum iob);
    bool is_head_at(int idx) const;
    void set_head_at(int idx, bool v);

    /* --------------------------------------------------------------------- */
    std::string dump_iob() const;
    std::string dump_segments() const;
    std::string dump_heads() const;
    std::string dump_alpha() const;

    /* --------------------------------------------------------------------- */
    void do_counts(int& annotations, int& disjoint, int& unannotated) const;
  };
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_ANNOTATIONCHANNEL_I */
