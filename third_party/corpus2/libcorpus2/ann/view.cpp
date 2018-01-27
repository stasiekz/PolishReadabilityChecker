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

#include <libcorpus2/ann/view.h>
#include <libcorpus2/ann/annotatedsentence.h>
#include <boost/make_shared.hpp>

namespace Corpus2 {

AnnotationView::AnnotationView(
	const boost::shared_ptr<AnnotatedSentence> &original,
	const std::string &ann_name)
	: Sentence(), original_(original), ann_name_(ann_name)
{
}

AnnotationView::~AnnotationView()
{
}

Sentence::Ptr AnnotationView::clone_shared() const
{
	boost::shared_ptr<AnnotationView> copy;
	copy = boost::make_shared<AnnotationView>(original_, ann_name_);
	BOOST_FOREACH(const Token* t, tokens_) {
		copy->append(t->clone());
	}
	return copy;
}

void AnnotationView::release_original()
{
	original_.reset();
}

void AnnotationView::commit()
{
	commit_to(original_, ann_name_);
}

void AnnotationView::commit_to(
	const boost::shared_ptr<AnnotatedSentence> &original,
	const std::string &ann_name)
{
	if (!original) {
		throw AnnotationViewOutOfSync("null-pointer-to-original");
	}
	if (!original->has_channel(ann_name)) {
		throw MissingAnnotationChannel(ann_name);
	}
	// ...
}

} /* end ns Corpus2 */
