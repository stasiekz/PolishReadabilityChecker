/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBTOKI_WHITESPACETOKENIZER_H
#define LIBTOKI_WHITESPACETOKENIZER_H

#include <libtoki/token.h>
#include <libtoki/tokenizer/tokenizer.h>
#include <libtoki/unicode/source.h>

namespace Toki {

	namespace Srx {
		class SourceWrapper;
	}

	/**
	 * A simple tokenizer that takes a UnicodeSource (i.e. continous text) and
	 * and outputs (via get_next_token) tokens split according to whitespace in
	 * the text.
	 */
	class WhitespaceTokenizer : public Tokenizer
	{
	public:
		/**
		 * Constructor
		 *
		 * Keys recognized in the configuration:
		 * - token_type - Token type to set in created tokens, defaults to "t".
		 * - initial_whitespace - Whitespace to set in the first extracted
		 *                        token, can be a descriptor or a string with
		 *                        the actual whitespace, use \\u0020 for a
		 *                        space.
		 * - srx - SRX file to load and use as a sentence break detector. The
		 *         segmenter is preserved even if the source of this tokenizer
		 *         is changed. BY default SRX is not used.
		 * - srx_language - language to use with the SRX, determines which
		 *                  rules will be used. The default empty value is
		 *                  likely not going to work well.
		 * - srx_window - SRX performance tuning parameter, segmentation
		 *                wrapper "window" parameter, see Srx::SourceWrapper
		 * - srx_margin - SRX performance tuning parameter, segmentation
		 *                wrapper "margin" parameter, see Srx::SourceWrapper
		 */
		WhitespaceTokenizer(const Config::Node& cfg = default_config());

		/**
		 * Constructor shorthand
		 * @see Tokenizer::Tokenizer
		 */
		WhitespaceTokenizer(UnicodeSource* us,
				const Config::Node& cfg = default_config());

		/// TokenSource override
		Token* get_next_token();

		/// Tokenizer override
		void restart();

		/// SRX state accessor
		bool has_srx() const;

		/// Initial whitespace setter
		void set_initial_whitespace(PwrNlp::Whitespace::Enum wa);

	protected:
		/// UnicodeSink override
		void new_input_source();

		/// process configuration helper for the constructors
		void process_config(const Config::Node& cfg);

	private:
		/**
		 * Read characters from the input source until a non-white character is
		 * found, storing the amount/type of whitespace that was consumed.
		 */
		void eat_whitespace();

		/**
		 * Stored whitespace amount that preceeds the next token
		 */
		PwrNlp::Whitespace::Enum wa_;

		/**
		 * The type of tokens to return
		 */
		std::string token_type_;

		/**
		 * Initial whitespace for the first token
		 */
		PwrNlp::Whitespace::Enum initial_wa_;


		/**
		 * First token flag
		 */
		bool first_;

		/**
		 * Token begins_sentence flag
		 */
		bool begins_sentence_;

		boost::shared_ptr<Srx::SourceWrapper> srx_;
	};

} //end namespace Toki

#endif // LIBTOKI_WHITESPACETOKENIZER_H
