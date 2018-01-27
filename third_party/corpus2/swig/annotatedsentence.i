#ifndef SWIG_LIBCORPUS2_ANNOTATEDSENTENCE_I
#define SWIG_LIBCORPUS2_ANNOTATEDSENTENCE_I

%module libcorpusannotatedsentence
%{
  #include <libcorpus2/ann/annotatedsentence.h>
%}

%include "token.i"
%include "sentence.i"
%include "exception.i"
%include "annotationchannel.i"

%include "boost_shared_ptr.i"
%include "std_map.i"
%include "std_string.i"

%feature("notabstract") Corpus2::AnnotatedSentence;

%template(AnnotatedSentencePtr) boost::shared_ptr<Corpus2::AnnotatedSentence>;
%template(ConstAnnotatedSentencePtr) boost::shared_ptr<const Corpus2::AnnotatedSentence>;

namespace Corpus2 {
  class MissingAnnotationChannel : public Corpus2Error {
  public:
    MissingAnnotationChannel(const std::string& name);
    ~MissingAnnotationChannel() throw();
  }; // MissingAnnotationChannel

  class AnnotatedSentence : public Corpus2::Sentence {
  public:
    typedef std::map<std::string, AnnotationChannel> chan_map_t;

    /* --------------------------------------------------------------------- */
    AnnotatedSentence(const std::string &id = "");
    ~AnnotatedSentence();
    
    /* --------------------------------------------------------------------- */
    Sentence::Ptr clone_shared() const;

    static boost::shared_ptr<AnnotatedSentence> wrap_sentence(
      const boost::shared_ptr<Sentence>& s);
    static boost::shared_ptr<AnnotatedSentence> wrap_sentence_clone(
      const boost::shared_ptr<Sentence>& s);

    /* --------------------------------------------------------------------- */
    void create_channel(const std::string& name);
    bool has_channel(const std::string& name) const;
    
    AnnotationChannel& get_channel(const std::string& name);
    const AnnotationChannel& get_channel(const std::string& name) const;
    bool add_channel(const std::string& name, const AnnotationChannel& chan);

    /* Use with care. Note that get_channel returns a reference to the original
       channel, hence it is strongly recommended to del the channel ref object
       before attempting to call remove_channel. */
    bool remove_channel(const std::string& name);

    const chan_map_t& all_channels() const;

    /* --------------------------------------------------------------------- */
    void append(Token *t);

    /* --------------------------------------------------------------------- */
    std::string annotation_info() const;
  };

  %extend AnnotatedSentence {
    static boost::shared_ptr<Sentence> cast_as_sentence(
      const boost::shared_ptr<AnnotatedSentence>& a)
    {
      return a;
    }
  }

boost::shared_ptr<Corpus2::AnnotationView> create_view(
        const boost::shared_ptr<AnnotatedSentence>& s,
        const std::string& ann_name);
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_ANNOTATEDSENTENCE_I */
