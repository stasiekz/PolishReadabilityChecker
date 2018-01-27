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

#ifndef LIBCORPUS2_ANN_ANNOTATEDSENTENCE_H
#define LIBCORPUS2_ANN_ANNOTATEDSENTENCE_H

#include <libcorpus2/sentence.h>
#include <libcorpus2/exception.h>
#include <libcorpus2/ann/channel.h>
#include <libcorpus2/ann/view.h>

namespace Corpus2 {

/**
 * Exception class for use when a requested annotation channel does not exist
 */
class MissingAnnotationChannel : public Corpus2Error
{
public:
	MissingAnnotationChannel(const std::string& name)
		: Corpus2Error("Annotation channel missing: " + name)
	{
	}

	~MissingAnnotationChannel() throw()
	{
	}
};

/**
 * A class describing Sentences with additional information in the form of
 * annotation channels.
 *
 * Note: channels are not automatiaclly resized. The sentence should not have
 * tokens added or removed after annotation channels are created.
 */
class AnnotatedSentence : public Corpus2::Sentence
{
public:
	/**
	 * Create an empty AnnotatedSentece with no tokens and no channels
	 */
	AnnotatedSentence(const std::string &id = "");

	~AnnotatedSentence();

	Sentence::Ptr clone_shared() const;

	/// typedef for the channels
	typedef std::map<std::string, AnnotationChannel> chan_map_t;

	/**
	 * Create an AnnotatedSentence from a Sentence, grabing all the tokens
	 * directly (afterwards the source Sentence has no tokens).
	 */
	static boost::shared_ptr<AnnotatedSentence> wrap_sentence(
		const boost::shared_ptr<Sentence> s);

	/**
	 * Create an AnnotatedSentence from a Sentence, cloning all the tokens.
	 * The source Sentence is not modified.
	 */
	static boost::shared_ptr<AnnotatedSentence> wrap_sentence_clone(
		const boost::shared_ptr<Sentence> s);

	/**
	 * Create an annotation channel named name in this annotated sentence.
	 * If the channel already exists, nothing happens.
	 */
	void create_channel(const std::string& name);

	/**
	 * @return true iif this sentence has an annotation channel named name
	 */
	bool has_channel(const std::string& name) const {
		return channels_.find(name) != channels_.end();
	}

	/**
	 * Return the annotation channel by name or throw MissingAnnotationChannel
	 * if there is no such channel
	 */
	AnnotationChannel& get_channel(const std::string& name) {
		chan_map_t::iterator i = channels_.find(name);
		if (i == channels_.end()) {
			throw MissingAnnotationChannel(name);
		}
		return i->second;
	}

	/**
	 * Const version of get_channel
	 */
	const AnnotationChannel& get_channel(const std::string& name) const {
		chan_map_t::const_iterator i = channels_.find(name);
		if (i == channels_.end()) {
			throw MissingAnnotationChannel(name);
		}
		return i->second;
	}

	/**
	 * Add a copy of the given annotation channel.
	 * If a channel of the given name already exists, will do nothing and return false.
	 * Otherwise, will add the channel and return true.
	 */
	bool add_channel(const std::string& name, const AnnotationChannel& chan) {
		if (has_channel(name)) {
			return false;
		}
		channels_.insert(std::make_pair(name, chan));
		return true;
	}

    /**
     * Remove channel having a given name. Returns whether removed
     * (will return false if no channel of the given name exists).
     */
    bool remove_channel(const std::string& name) {
        return (channels_.erase(name) > 0);
    }

	const chan_map_t& all_channels() const {
		return channels_;
	}

	/// Sentence override, extends annotation objects
	void append(Token *t);

	std::string annotation_info() const;

private:
	/// the actual channels
	chan_map_t channels_;
};

/**
 * Create an AnnotationView pseudo-sentence from an AnnotatedSentence that
 * behaves like a sentence viewed through an annotation.
 *
 * This is a free function, not a member of AnnotatedSentence, because it is
 * mandatory that the AnnotatedSentence be passed via a shared_ptr.
 */
boost::shared_ptr<AnnotationView> create_view(
	const boost::shared_ptr<AnnotatedSentence>& s,
	const std::string& ann_name);


} /* end ns Corpus2 */

#endif // LIBCORPUS2_ANN_ANNOTATEDSENTENCE_H
