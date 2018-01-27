#include "wcrftreader.h"

#include <libwcrft/corpusio.h>

#include <boost/algorithm/string.hpp>
#include <libcorpus2/exception.h>
#include <libcorpus2/chunk.h>

namespace Corpus2 {

bool WcrftReader::registered = TokenReader::register_path_reader<WcrftReader>(
			"wcrft","Tag input file with WCRFT2 and pretend we're just reading. "
			"Options: format (default: txt), config (default: nkjp_e2) and model"
			"(default: empty string)");

const std::string WcrftReader::DEFAULT_CONFIG = "nkjp_e2";
const std::string WcrftReader::DEFAULT_FORMAT = "txt";
// empty string means it will be inferred from config
const std::string WcrftReader::DEFAULT_MODEL = "";

WcrftReader::WcrftReader(const Tagset& tagset, const std::string &filename)
	: BufferedChunkReader(tagset),
	  tagset_name(tagset.name()),
	  input_format_(DEFAULT_FORMAT),
	  config_name_(DEFAULT_CONFIG),
	  model_dir_(DEFAULT_MODEL),
	  inner_filename_(filename)
{
}

WcrftReader::~WcrftReader()
{
}

void WcrftReader::ensure_tagger_loaded()
{
	if(!tagger_) {
		tagger_ = boost::make_shared<Wcrft::Tagger>(config_name_, model_dir_);
		// assert the config assumes the same tagset as we do
		if(tagger_->get_tagset().name().compare(tagset_name) != 0) {
			// TODO: replace with TagsetMismatch from Corpus2
			std::string message =
					std::string("WcrftReader was told to use tagset ")
					+ tagset_name + " while tagger config "
					+ config_name_
					+ " operates on tagset " + tagger_->get_tagset().name();
			throw Corpus2Error(message);
		}
		tagger_->load_model();
		// create a reader that will be able to feed the tagger
		// with paragraphs or sentences to tag
		const std::string maca_cfg = tagger_->get_maca_config();

		inner_reader_ = Wcrft::get_reader(
					inner_filename_, input_format_, tagset(), maca_cfg);
	}
}

void WcrftReader::validate()
{
	ensure_tagger_loaded();
}

void WcrftReader::set_option(const std::string& option)
{
	if(boost::algorithm::starts_with(option, "format:")) {
		input_format_ = option.substr(7);
	} else if(boost::algorithm::starts_with(option, "config:")) {
		config_name_ = option.substr(7);
	} else if(boost::algorithm::starts_with(option, "model:")) {
		model_dir_ = option.substr(6);
	} else {
		TokenReader::set_option(option);
	}
}

std::string WcrftReader::get_option(const std::string &option) const
{
	const static std::string PREF_FORMAT("format:");
	const static std::string PREF_CONFIG("config:");
	const static std::string PREF_MODEL("model:");
	if(boost::algorithm::starts_with(option, PREF_FORMAT)) {
		return PREF_FORMAT + input_format_;
	}
	if(boost::algorithm::starts_with(option, PREF_CONFIG)) {
		return PREF_CONFIG + config_name_;
	}
	if(boost::algorithm::starts_with(option, PREF_MODEL)) {
		return PREF_MODEL + model_dir_;
	}
	return BufferedChunkReader::get_option(option);
}

void WcrftReader::ensure_more()
{
	if(chunk_buf_.empty()) {
		ensure_tagger_loaded();
		boost::shared_ptr<Corpus2::Chunk> paragraph =
				inner_reader_->get_next_chunk();
		if(paragraph) {
			// using default options:
			// - don't preserve ambiguity (it may damage unknown word handling)
			// - do use tag guesser (it's useful)
			tagger_->tag_paragraph(paragraph);
		}
		chunk_buf_.push_back(paragraph);
	}
}

} // end ns Corpus2
