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

#ifndef LIBCORPUS2_IO_WRITER_H
#define LIBCORPUS2_IO_WRITER_H

#include <libcorpus2/chunk.h>

#include <loki/Factory.h>
#include <loki/Singleton.h>
#include "compressor.h"
namespace Corpus2 {

/**
 * Base class for writers of Corpus2 tokens
 */
class TokenWriter
{
public:
	typedef boost::shared_ptr<TokenWriter> TokenWriterPtr;
	/**
	 * Constructor. A TokenWriter operates within a tagset, outputs data
	 * to a stream and may have some options passed.
	 */
	TokenWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params);

	/// Destructor
	virtual ~TokenWriter();

	virtual void write_token(const Token& t) = 0;

	/// Convenience wrapper for a write-and-delete action
	void write_token_dispose(Token* t);

	virtual void write_sentence(const Sentence& s) = 0;

	virtual void write_chunk(const Chunk& p) = 0;

    virtual void finish();


    //ktagowski added for compression
    std::stringstream & sstream(){
        return sstream_;
    }
    std::stringstream & ssi();

	std::ostream& os() {
		return os_;
	}

	const Tagset& tagset() const {
		return tagset_;
	}

	/**
	 * Factory interface for creating writers from string identifiers
	 *
	 * Mostly a convenience function to avoid having client code refer
	 * directly to the factory instance.
	 *
	 * @param class_id the unique class identifier
	 * @param os the output stream to pass to the writer's constructor
	 * @param tagset the tagset to pass to the writer's constructor
	 */
	static TokenWriterPtr create_stream_writer(const std::string& class_id,
			std::ostream& os,
			const Tagset& tagset,
			const string_range_vector& params);

	/**
	 * Factory interface for creating writers from string identifiers.
	 *
	 * Params are split from the class id and then the more general version
	 * is called. Parameters are expected to be comma-separated from the
	 * class id.
	 */
	static TokenWriterPtr create_stream_writer(const std::string& class_id_params,
			std::ostream& os,
			const Tagset& tagset);

	/**
	 * Factory interface for creating writers from string identifiers
	 *
	 * Mostly a convenience function to avoid having client code refer
	 * directly to the factory instance.
	 *
     * This is the file path (as opposed to output stream) version.
	 *
	 * @param class_id the unique class identifier
	 * @param path file to write to
	 * @param tagset the tagset to pass to the writer's constructor
	 */
	static TokenWriterPtr create_path_writer(const std::string& class_id,
			const std::string& path,
			const Tagset& tagset,
			const string_range_vector& params);

	/**
	 * Factory interface for creating writers from string identifiers.
	 *
	 * Params are split from the class id and then the more general version
	 * is called. Parameters are expected to be comma-separated from the
	 * class id.
	 *
	 * This is the file path (as opposed to output stream) version.
	 */
	static TokenWriterPtr create_path_writer(const std::string& class_id_params,
			const std::string& path,
			const Tagset& tagset);

	/**
	 * Function to get a vector of available writer type strings.
	 */
	static std::vector<std::string> available_writer_types();

	/**
	 * Function to get the help string for a writer
	 */
	static std::string writer_help(const std::string& class_id);

	/**
	 * Function to get a vector of available writer type strings with help
	 * strings appended
	 */
	static std::vector<std::string> available_writer_types_help();

	/**
	 * Convenience template for registering TokenWriter derived classes.
	 */
	template <typename T>
	static bool register_writer(const std::string& class_id,
			const std::string& help = "");


    std::ostream& osi();


protected:
    virtual void do_footer() { }

	void indent_more(int n = 1);

	void indent_less(int n = 1);

	int indent_level() const {
		return indent_;
	}

	/**
	 * Write indentation spaces to the output stream and return the stream
	 * for writing. Convenience function useful when starting new lines
	 * in the output. */
    //KTagowski: Added for Compression
    Compressor * pc_compressor;

protected:
    bool needs_footer_;

private:
	std::ostream& os_;

    std::stringstream sstream_;

	const Tagset& tagset_;


	int indent_;
};

namespace detail {

typedef Loki::Factory<
	TokenWriter, // The base class for objects created in the factory
	std::string, // Identifier type
	Loki::TL::MakeTypelist<
		std::ostream&, const Tagset&, const string_range_vector&
	>::Result
	// TokenLayer constructor arguments' types specification
> TokenWriterFactoryType;

struct TokenWriterFactory
{
	TokenWriterFactoryType factory;
	std::map<std::string, std::string> help;
};

/**
 * Declaration of the TokenWriter factory as a singleton Loki object
 * factory. The factory instance can be accessed as
 * TokenLayerFactory::Instance(). It is assumed that all derived classes
 * have the same constructor signature.
 */
typedef Loki::SingletonHolder<
	TokenWriterFactory,
	Loki::CreateUsingNew, // default, needed to change the item below
	Loki::LongevityLifetime::DieAsSmallObjectChild // per libloki docs
>
TokenWriterFactorySingleton;

/**
 * Convenience typedef for the exception type the factory throws
 */
typedef Loki::DefaultFactoryError<
	std::string, TokenWriter
>::Exception
TokenWriterFactoryException;

/**
 * Convenience template TokenWriter creation function
 */
template <typename T>
inline
T* writer_creator(std::ostream& os, const Tagset& tagset,
		const string_range_vector& params)
{
	return new T(os, tagset, params);
}

} /* end ns detail */


template <typename T>
bool TokenWriter::register_writer(const std::string& class_id,
		const std::string& help)
{
	bool ret = detail::TokenWriterFactorySingleton::Instance().factory.Register(
			class_id, detail::writer_creator<T>);
	if (ret) {
		detail::TokenWriterFactorySingleton::Instance().help[class_id] = help;
	}
	return ret;
}

} /* end ns Corpus2 */

#endif // LIBCORPUS2_IO_WRITER_H
