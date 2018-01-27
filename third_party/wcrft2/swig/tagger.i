#ifndef SWIG_WCRFT2_TAGGER_I
#define SWIG_WCRFT2_TAGGER_I

%module tagger
%{
	#include <libwcrft/tagger.h>
%}

%include "std_map.i"
%include "std_string.i"

%include "libcorpus2/chunk.i"
%include "libcorpus2/sentence.i"
%include "libcorpus2/tagset.i"
%include "libcorpus2/tokenreader.i"
%include "libcorpus2/tokenwriter.i"

%nodefaultctor Wcrft::Tagger;

namespace Wcrft {

class Statistics {
public:
	Statistics();

	void clear();

	void dump(std::ostream& output_stream = std::cerr);
	void report(std::ostream& output_stream = std::cerr, int sents = 100);

	std::map<std::string, int> layer_gets, layer_fails;
	int num_tokens, num_sentences, num_evals;
};

class Tagger {
public:
	Tagger(const std::string& config_name, const std::string& model_dir = "");
	~Tagger();

	/* --------------------------------------------------------------------- */
	void load_model();

	void train_and_save(const std::string& input_file, const std::string& input_format);

	/* --------------------------------------------------------------------- */
	void tag_input(const std::string& input_file, const std::string& input_format,
				   const std::string& output_file, const std::string& output_format);
	void tag_input(std::istream& input_stream, const std::string& input_format,
				   std::ostream& output_stream, const std::string& output_format);
	void tag_sentence(boost::shared_ptr<Corpus2::Sentence> sentence,
					  bool preserve_ambiguity = false,
					  bool guess_unknown = true);
	void tag_paragraph(boost::shared_ptr<Corpus2::Chunk> paragraph,
					   bool preserve_ambiguity = false,
					   bool guess_unknown = true);

	/* --------------------------------------------------------------------- */
	void switch_verbose(bool verbose);
	void switch_preserve_ambiguity(bool ambiguity);
	void switch_paragraph_processing(bool paragraphs);

	template<typename T>
	inline void set_configuration_option(const std::string& option_name, T value)
	{
		tagger_conf_.set_config_option<T>(option_name, value);
	}

	const Corpus2::Tagset &get_tagset() const
	{
		return tagset_;
	}

	void set_maca_config(const std::string &maca_cfg);
	const std::string get_maca_config() const;
};

}

using namespace std;
using namespace boost;
using namespace Wcrft;
using namespace Corpus2;

#endif // SWIG_WCRFT2_I
