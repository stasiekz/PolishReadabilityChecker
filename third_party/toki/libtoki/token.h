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

#ifndef LIBTOKI_TOKEN_H
#define LIBTOKI_TOKEN_H

#include <libpwrutils/whitespace.h>

#include <unicode/unistr.h>

#include <string>


namespace Toki {

/**
 * A single token being processed.
 *
 * Token is described by:
 * - orth, token string as encountered in running text (stored internally
 *   as an ICU UnicodeString),
 * - the type and amount of whitespace that came before this token,
 * - whether this token starts a new sentence,
 * - token type mnemonic (its interpretation is up to the user).
 */
class Token
{
public:
	/**
	 * Constructor.
	 * @param orth      the token orth
	 * @param type      the token type
	 * @param wa_before preceeding whitespace amount indicator
	 */
	Token(const UnicodeString& orth, const std::string& type,
			PwrNlp::Whitespace::Enum wa_before);

	/**
	 * Constructor.
	 * @param orth_utf8 the token orth, treated as an UTF8
	 * @param type      the token type
	 * @param wa_before preceeding whitespace amount indicator
	 */
	Token(const char* orth_utf8, const std::string& type,
			PwrNlp::Whitespace::Enum wa_before);

	/**
	 * Clone this token.
	 *
	 * The caller is responsible for releasing the cloned object.
	 */
	Token* clone() const;

	/**
	 * Clone this token, changing it's orth.
	 *
	 * The caller is responsible for releasing the cloned object.
	 */
	Token* clone_changed(const UnicodeString& new_orth) const;

	/**
	 * Clone this token, changing it's orth and type.
	 *
	 * The caller is responsible for releasing the cloned object.
	 */
	Token* clone_changed(const UnicodeString& new_orth,
			const std::string new_type) const;

	/**
	 * Convenience function, sets the flags as if this token came after no
	 * spaces (was cut out from another token).
	 */
	void mark_as_cut();

#ifndef LIBTOKI_TRACK_TOKEN_CREATION
	/// Copy ctor
	Token(const Token& other);

	/// Destructor
	~Token();

#endif
	/**
	 * Instance counter accessor, always returns -1 if the
	 * LIBTOKI_TRACK_TOKEN_CREATION symbol was not defined during
	 * compilation
	 */
	static int instance_count() {
#ifndef LIBTOKI_NO_TRACK_TOKEN_CREATION
		return instance_count_;
#else
		return -1;
#endif
	}

	/**
	 * Creation counter accessor, always returns -1 if the
	 * LIBTOKI_TRACK_TOKEN_CREATION symbol was not defined during
	 * compilation
	 */
	static int creation_count() {
#ifndef LIBTOKI_NO_TRACK_TOKEN_CREATION
		return creation_count_;
#else
		return -1;
#endif
	}

	/// orth getter
	const UnicodeString& orth() const {
		return orth_;
	}

	/// orth setter
	void set_orth(const UnicodeString& new_orth) {
		orth_ = new_orth;
	}

	/// type getter
	const std::string& type() const {
		return type_;
	}

	/// type setter
	void set_type(const std::string& new_type) {
		type_ = new_type;
	}

	/// wa getter
	PwrNlp::Whitespace::Enum preceeding_whitespace() const {
		return preceeding_whitespace_;
	}

	/// wa setter
	void set_preceeding_whitespace(PwrNlp::Whitespace::Enum new_wa) {
		preceeding_whitespace_ = new_wa;
	}

	/// orth as-utf8 convenience getter
	std::string orth_utf8() const;

	/// begins_sentence getter
	bool begins_sentence() const {
		return begins_sentence_;
	}

	/// begins_sentence setter
	void set_begins_sentence(bool v) {
		begins_sentence_ = v;
	}

protected:
	/// 'orth', the body of the token
	UnicodeString orth_;

	/// type of the token
	std::string type_;

	/// whitespace preceeding the token
	PwrNlp::Whitespace::Enum preceeding_whitespace_;

	/// token-begins-sentence flag
	bool begins_sentence_;

#ifndef LIBTOKI_NO_TRACK_TOKEN_CREATION
private:
	static int instance_count_;
	static int creation_count_;
#endif
};

} /* end ns Toki */

#endif // LIBTOKI_TOKEN_H
