#ifndef SWIG_LIBCORPUS2_TOKENREADER_I
#define SWIG_LIBCORPUS2_TOKENREADER_I

%module libcorpustokenreader
%{
  #include <libcorpus2/io/reader.h>
  #include <libcorpus2/io/helpers.h>
%}

%include "tag.i"
%include "token.i"
%include "chunk.i"
%include "tagset.i"
%include "sentence.i"
%include "std_string.i"
%include "std_vector.i"
%include "boost_shared_ptr.i"

%nodefaultctor Corpus2::TokenReader;

%template(TokenReaderPtr) boost::shared_ptr<Corpus2::TokenReader>;
%template(ConstTokenReaderPtr) boost::shared_ptr<const Corpus2::TokenReader>;

// typedef boost::shared_ptr<Corpus2::Token> TokenPtr;

namespace Corpus2 {
  class TokenReader {
  public:
    typedef boost::shared_ptr<TokenReader> TokenReaderPtr;
    //typedef boost::shared_ptr<Token> TokenPtr;

    /* --------------------------------------------------------------------- */
    explicit TokenReader(const Tagset& tagset);
    virtual ~TokenReader();
    
    /* --------------------------------------------------------------------- */
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    %feature("autodoc", "1");
    static TokenReaderPtr create_path_reader(
      const std::string& class_id,
      const Tagset& tagset,
      const std::string& path);

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    %feature("autodoc", "1");
    static TokenReaderPtr create_stream_reader(
      const std::string& class_id,
      const Tagset& tagset,
      std::istream& stream);

    /* --------------------------------------------------------------------- */
    /* virtual Token* get_next_token() = 0; */
    virtual Sentence::Ptr get_next_sentence() = 0;
    virtual boost::shared_ptr<Chunk> get_next_chunk() = 0;

    /* --------------------------------------------------------------------- */
    virtual void set_option(const std::string& option);
    virtual std::string get_option(const std::string& option) const;

    /* --------------------------------------------------------------------- */
    const Tagset& tagset() const;
    static std::vector<std::string> available_reader_types();

    /* --------------------------------------------------------------------- */
    static std::string reader_help(const std::string& class_id);
    static std::vector<std::string> available_reader_types_help();
  };

  %extend TokenReader {
    /* modfify the native get_next_token to wrap the tokens into shared_ptr */
    boost::shared_ptr<Corpus2::Token> get_next_token() {
      return boost::shared_ptr<Corpus2::Token>(self->get_next_token());
    }

    %feature("autodoc", "1");
    static TokenReaderPtr create_stdin_reader(
        const std::string& class_id_params, const Tagset& tagset) {
        return Corpus2::TokenReader::create_stream_reader(class_id_params, tagset, std::cin);
    }
  }

%feature("autodoc", "1");
  std::vector<boost::shared_ptr<Chunk> > read_chunks_from_utf8_string(
    const std::string& data, const Tagset& tagset, const std::string& format);
 
}

using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_TOKENREADER_I */
