/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBCORPUS2_IO_READER_H
#define LIBCORPUS2_IO_READER_H

#include <libcorpus2/chunk.h>
#include <libcorpus2/tokensource.h>
#include <deque>
#include <loki/Factory.h>
#include <loki/Singleton.h>

namespace Corpus2 {

/**
 * Base class for readers of Corpus2 tokens. Tokens are always read from a
 * source and always within a given tagset.
 *
 * Tokens can be read on a token-by-token basis, or in entire senteces, or
 * in entire chunks (paragraphs). Behavior is not defined if there are
 * mixed calls to differend forms of getting data.
 */
class TokenReader : public TokenSource
{
public:
	typedef boost::shared_ptr<TokenReader> TokenReaderPtr;

	/// Constructor --- only a Tagset is needed
	explicit TokenReader(const Tagset& tagset);

	/**
	 * Reader creation from a class identifier (possibly with comma-separated
	 * parameters / options that are passed to set_option), with a tagset and
	 * a path to a file or some other resource that the reader will open.
	 *
	 * Any files open will be closed by the reader.
	 */
	static TokenReaderPtr create_path_reader(
		const std::string& class_id,
		const Tagset& tagset,
		const std::string& path);

	/**
	 * Reader creation as in create_path_reader, only readng form a stream
	 * that is managed by the caller (so e.g. std::cin can be used). Generally
	 * all stream readers are path readers, but not all path readers are
	 * stream readers (a path reader might look and open more that one file,
	 * which is beyond what this interface allows). Attempting to create a
	 * reader that can not read a stream will result in an exception.
	 */
	static TokenReaderPtr create_stream_reader(
		const std::string& class_id,
		const Tagset& tagset,
		std::istream& stream);

	/// Destructor
	virtual ~TokenReader();

	/**
	 * Interface for getting a token from the reader. Note that the caller
	 * must dispose of the Token it receives. A null value returned indicates
	 * end of processing.
	 *
	 * There is no information about sentence boundaries in this mode.
	 */
	virtual Token* get_next_token() = 0;

	/**
	 * Interface for getting entire senteces from the reader.
	 *
	 * There is no information about chunk boundaries in that mode.
	 */
	virtual Sentence::Ptr get_next_sentence() = 0;

	/**
	 * Interface for getting entire chunks from the reader.
	 */
	virtual boost::shared_ptr<Chunk> get_next_chunk() = 0;

	/**
	 * General option setter.
	 */
	virtual void set_option(const std::string& option);

	/**
	 * Option inspector. Should echo the option if it is set, return
	 * an empty string otheriwse, and "unknown" if the option is invalid.
	 */
	virtual std::string get_option(const std::string& option) const;

	/**
	 * Check if the reader is valid, should throw if not. Called after
	 * all set_options during factory reader creation.
	 */
	virtual void validate() {}

	/**
	 * Set a new file to read (takes a filepath).
	 */
	virtual void setFile(const std::string&) {}

	/**
	 * Tagset accesor
	 */
	const Tagset& tagset() const {
		return tagset_;
	}

	/**
	 * Function to get a vector of available reader type strings.
	 */
	static std::vector<std::string> available_reader_types();

	/**
	 * Function to get the help string for a reader
	 */
	static std::string reader_help(const std::string& class_id);

	/**
	 * Function to get a vector of available reader type strings with help
	 * strings appended
	 */
	static std::vector<std::string> available_reader_types_help();

	/**
	 * Convenience template for registering TokenReader derived classes.
	 */
	template <typename T>
	static bool register_reader(const std::string& class_id,
			const std::string& help = "");

	/**
	 * Convenience template for registering TokenReader derived classes.
	 * Path-only verison.
	 */
	template <typename T>
	static bool register_path_reader(const std::string& class_id,
			const std::string& help = "");

	/// Convenience function to parse a tag string with options of this reader
	Tag parse_tag(const std::string& tag_string) const {
		return tagset().parse_simple_tag(tag_string, tag_parse_mode_);
	}

	/// tag parse mode getter
	Tagset::ParseMode tag_parse_mode() const {
		return tag_parse_mode_;
	}

	/// tag parse mode setter
	void set_tag_parse_mode(Tagset::ParseMode mode) {
		tag_parse_mode_ = mode;
	}

	boost::shared_ptr<Sentence> make_sentence() const;

protected:
	unsigned int sent_number_; /// Sentence number, automatically generated
	unsigned int chunk_number_; /// Chunk numer, automatically generated
	static const std::string SENT_ID_PREFFIX; /// Sent id prefix
	static const std::string CHUNK_ID_PREFFIX; /// Chunk id prefix

	/**
	 * If sentence has no name/id, will auto-generate one.
	 * Returns whether no name was set.
	 */
	bool name_sent(boost::shared_ptr<Sentence> sent);

	/**
	 * If chunk has no name/id, will auto-generate one.
	 * Returns whether no name was set.
	 */
	bool name_chunk(boost::shared_ptr<Chunk> chunk);

private:
	/// Tagset used by the Reader
	const Tagset& tagset_;

	/// Tag parse mode
	Tagset::ParseMode tag_parse_mode_;

	/// Flag to force creation of sentences as AnnotatedSentences
	bool use_annotated_sentences_;
};

namespace detail {

typedef Loki::Factory<
	TokenReader, // The base class for objects created in the factory
	std::string, // Identifier type
	Loki::TL::MakeTypelist<
		const Tagset& /*tagset*/,
		std::istream& /*input*/,
		const string_range_vector& /*params*/
	>::Result
> StreamTokenReaderFactoryType;

typedef Loki::Factory<
	TokenReader, // The base class for objects created in the factory
	std::string, // Identifier type
	Loki::TL::MakeTypelist<
		const Tagset& /*tagset*/,
		const std::string& /*path*/,
		const string_range_vector& /*params*/
	>::Result
> PathTokenReaderFactoryType;

struct TokenReaderFactory
{
	StreamTokenReaderFactoryType stream_factory;
	PathTokenReaderFactoryType path_factory;
	std::map<std::string, std::string> help;
};

/**
 * Factory singleton accesor
 */
TokenReaderFactory& token_reader_factory();

/**
 * Templated TokenReader creation function, stream variant
 */
template <typename T>
inline
T* stream_reader_creator(const Tagset& tagset, std::istream& is,
	const string_range_vector& params)
{
	T* reader = new T(tagset, is);
	BOOST_FOREACH(const string_range& sr, params) {
		reader->set_option(boost::copy_range<std::string>(sr));
	}
	reader->validate();
	return reader;
}

/**
 * Templated TokenReader creation function, stream variant
 */
template <typename T>
inline
T* path_reader_creator(const Tagset& tagset, const std::string& path,
	const string_range_vector& params)
{
	T* reader = new T(tagset, path);
	BOOST_FOREACH(const string_range& sr, params) {
		reader->set_option(boost::copy_range<std::string>(sr));
	}
	reader->validate();
	return reader;
}

/**
 * Convenience typedef for the exception type the factory throws
 */
typedef Loki::DefaultFactoryError<
	std::string, TokenReader
>::Exception
TokenReaderFactoryException;

} /* end ns detail */


template <typename T>
bool TokenReader::register_reader(const std::string& class_id,
		const std::string& help)
{
	bool ret = detail::token_reader_factory().path_factory.Register(
			class_id, detail::path_reader_creator<T>);
	bool ret2 = detail::token_reader_factory().stream_factory.Register(
			class_id, detail::stream_reader_creator<T>);
	if (ret || ret2) {
		detail::token_reader_factory().help[class_id] = help;
	}
	return ret;
}

template <typename T>
bool TokenReader::register_path_reader(const std::string& class_id,
		const std::string& help)
{
	bool ret = detail::token_reader_factory().path_factory.Register(
			class_id, detail::path_reader_creator<T>);
	if (ret) {
		detail::token_reader_factory().help[class_id] = help;
	}
	return ret;
}


/**
 * Convenience class for readers that keep a buffer of chunks. Sentence
 * and token accessors are based upon the chunk buffer.
 *
 * A dervied class only needs to override ensure_more with a function that
 * fills the chunk buffer.
 */
class BufferedChunkReader : public TokenReader
{
public:
	BufferedChunkReader(const Tagset& tagset);

	~BufferedChunkReader();

	Token* get_next_token();

	Sentence::Ptr get_next_sentence();

	boost::shared_ptr<Chunk> get_next_chunk();

	void set_option(const std::string& option) {
		TokenReader::set_option(option);
	}

	std::string get_option(const std::string& option) const {
		return TokenReader::get_option(option);
	}

protected:
	virtual void ensure_more() = 0;

	std::deque< boost::shared_ptr<Chunk> > chunk_buf_;
	std::deque< Sentence::Ptr > sentence_buf_;
	std::deque<Token*> token_buf_;
};

/**
 * Convenience class for readers that internally read sentences. The token
 * accessor is based on the sentence buffer, so is the chunk accessor.
 *
 * A dervied class only neds to override actual_next_sentence with a
 * function that returns a new sentence.
 *
 * Note that the chunk accessor might well read the entire input and return
 * one huge chunk with all the sentences.
 *
 * By default the reader will split sequences of sentences into chunks
 * (i.e. paragraphs) before each token that has the preceding whitespace
 * amount set to PwrNlp::Whitespace::ManyNewlines. This behaviour is
 * controlled by the chunkify_ flag.
 */
class BufferedSentenceReader : public TokenReader
{
public:
	BufferedSentenceReader(const Tagset& tagset);

	Token* get_next_token();

	Sentence::Ptr get_next_sentence();

	boost::shared_ptr<Chunk> get_next_chunk();

	void set_option(const std::string& option) {
		TokenReader::set_option(option);
	}

	std::string get_option(const std::string& option) const {
		return TokenReader::get_option(option);
	}

protected:
	virtual Sentence::Ptr actual_next_sentence() = 0;

	bool chunkify_;

	Sentence::Ptr sentence_buf_;

	std::deque<Token*> token_buf_;
};

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_READER_H
