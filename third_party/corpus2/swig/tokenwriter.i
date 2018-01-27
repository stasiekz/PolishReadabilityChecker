#ifndef SWIG_LIBCORPUS2_TOKENWRITER_I
#define SWIG_LIBCORPUS2_TOKENWRITER_I

%module libcorpustokenwriter
%{
  #include <libcorpus2/io/writer.h>
%}

%include "tag.i"
%include "token.i"
%include "chunk.i"
%include "tagset.i"
%include "sentence.i"
%include "std_string.i"
%include "boost_shared_ptr.i"

%nodefaultctor Corpus2::TokenWriter;

%template(TokenWriterPtr) boost::shared_ptr<Corpus2::TokenWriter>;
%template(ConstTokenWriterPtr) boost::shared_ptr<const Corpus2::TokenWriter>;

namespace Corpus2 {
  class TokenWriter {
  public:
    typedef boost::shared_ptr<TokenWriter> TokenWriterPtr;
    /* --------------------------------------------------------------------- */

    TokenWriter();
    // TokenWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params);
    ~TokenWriter();
    
    /* --------------------------------------------------------------------- */
    virtual void write_token(const Token& t) = 0;
    void write_token_dispose(Token* t);
    virtual void write_sentence(const Sentence& s) = 0;
    virtual void write_chunk(const Chunk& p) = 0;

    /* --------------------------------------------------------------------- */
    // std::ostream& os();
    const Tagset& tagset();
    %feature("autodoc", "Instead of using this function you must delete TokenWriter object by calling: 'del TokenWriterObject' to finish writing to file.");
    void finish();

    /* --------------------------------------------------------------------- */
    const Tagset& tagset() const;
    static std::vector<std::string> available_writer_types();

    /* --------------------------------------------------------------------- */
    static std::string writer_help(const std::string& class_id);
    static std::vector<std::string> available_writer_types_help();

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
    static TokenWriterPtr create_stream_writer(
      const std::string& class_id_params, 
      std::ostream& os, const Tagset& 
      tagset);

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    %feature("autodoc", "1");
    static TokenWriterPtr create_path_writer(
      const std::string& class_id_params,
      const std::string& path,
      const Tagset& tagset);
  };

%extend TokenWriter {
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
	%feature("autodoc", "1");
	static TokenWriterPtr create_stdout_writer(
			const std::string& class_id_params, const Tagset& tagset) {
		return Corpus2::TokenWriter::create_stream_writer(class_id_params, std::cout, tagset);
	}

	void write_token(boost::shared_ptr<Corpus2::Token> t) {
		self->write_token(*t);
	}
	void write_sentence(boost::shared_ptr<Corpus2::Sentence> s) {
		self->write_sentence(*s);
	}
	void write_chunk(boost::shared_ptr<Corpus2::Chunk> c) {
		self->write_chunk(*c);
	}
}
}


using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_TOKENWRITER_I */
