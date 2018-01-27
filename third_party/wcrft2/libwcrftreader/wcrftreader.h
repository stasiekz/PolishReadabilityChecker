#ifndef LIBWCRFTREADER_WCRFTREADER_H
#define LIBWCRFTREADER_WCRFTREADER_H

#include <libcorpus2/io/reader.h>

#include <libwcrft/tagger.h>

namespace Corpus2 {

/**
 * @brief @c Wrapper that allows to use tagger as a @c Corpus2::TokenReader.
 *
 * Gives @c Corpus2::TokenReader interface to WCRFT tagger. The reader may be
 * created using @c create_path_reader and using "wcrft" as reader id.
 * WcrftReader may read various input formats. By default it tags plain text.
 * You can override this by setting "format:formatname" option.
 * Default WCRFT configuration may be overridden using "config:configname"
 * option. Trained tagger model may be overridden using "model:modeldir".
 */
class WcrftReader: public BufferedChunkReader
{
public:
	/**
	 * Create WcrftReader ready to process the given file.
	 * @param tagset @c tagset that will match tagger config used
	 * @param filename file to be tagged or empty string for stdin
	 */
	WcrftReader(const Tagset& tagset, const std::string &filename);

	~WcrftReader();

	/**
	  * Allows to set options specific to WCRFT. These include:
	  * * "format:formatname" - assumed input format (default: txt),
	  * * "config:configname" - tagger config (default: nkjp_e2),
	  * * "model:modeldir" - override model dir (default: empty string).
	 **/
	void set_option(const std::string& option);

	/**
	 * Option inspector. If option is set, will return the whole string
	 * than contains its name and its value (e.g. config:nkjp_e2). If unset,
	 * will return an empty string and "unknown" if the option is invalid.
	 */
	std::string get_option(const std::string& option) const;

	/**
	 * Check if the reader is valid, should throw if not. Called after
	 * all set_options during factory reader creation.
	 */
	virtual void validate();

	static bool registered;

	static const std::string DEFAULT_CONFIG, DEFAULT_FORMAT, DEFAULT_MODEL;

private:
	/// create tagger instance and load its data if tagger is NULL
	void ensure_tagger_loaded();

	/// actual implementation of obtaining tagged data
	void ensure_more();

	/// name of tagset in use to assert it matches tagger config
	const std::string tagset_name;
	/// pointer to tagger object
	boost::shared_ptr<Wcrft::Tagger> tagger_;
	/// format of input data as understood by tagger
	std::string input_format_;
	/// tagger configuration name
	std::string config_name_;
	/// optional location for tagger model (may be empty string)
	std::string model_dir_;

	/// path for inner reader
	std::string inner_filename_;
	/// token reader object as used by the tagger
	boost::shared_ptr<Corpus2::TokenReader> inner_reader_;

	/// contains last processed sentence
	Corpus2::Sentence::Ptr currentSentence;
	/// contains last processed chunk
	boost::shared_ptr<Corpus2::Chunk> currentChunk;
};

} // ns Corpus2

#endif // LIBWCRFTREADER_WCRFTREADER_H
