#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This file is part of WCRFT
# Copyright (C) 2012-2013 Adam Radziszewski.
# WCRFT is free software; you can redistribute and/or modify it
# under the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# WCRFT is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the LICENCE and COPYING files for more details

import sys
import codecs # utf8
from optparse import OptionParser

import corpus2

from wcrft import tagger
from wcrft import corpio as tagger_io

descr = """%prog [options] INPUT_FILE OUTPUT_FILE

Performs lemmatisation of INPUT_FILE and writes results to OUTPUT_FILE.
Assumes both are text files encoded in UTF-8. Output will contain lemmas
separated with spaces. If the tagger recognises that input is divided
into paragraphs, will separate paragraphs by single newspace (in the
default config the tagger will recognise paragraph division when two
or more consecutive newline characters appear in the input).

NKJP tagset assumes non-traditional segmentation policy, which affects
lemmatisation. Use the -x switch to determine which parts of speech
(grammatical classes) should be ignored. By default tokens bearing the
aglt class are ignored. To ignore nothing, use -x none. You can ignore
multiple classes, e.g. -x aglt,qub,adv.
"""

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-w', '--tagger-config', type='string', action='store',
		dest='tagger_config', default='nkjp_s2.ini',
		help='use given tagger (wcrft) config (default: nkjp_s2.ini)')
	parser.add_option('-W', '--tagger-model', type='string', action='store',
		dest='tagger_dir', default='model_nkjp10_wcrft_s2',
		help='read tagger (wcrft) trained model from the given dir (default: model_nkjp10_wcrft_s2)')
	parser.add_option('-m', '--maca-config', type='string', action='store',
		dest='maca_config', default='',
		help='override maca config file specified in tagger config')
	parser.add_option('-x', '--exclude-classes', type='string', action='store',
		dest='unwanted_classes', default='aglt',
		help='ignore tokens with those parts of speech (comma-separated list)')
	(options, args) = parser.parse_args()
	
	
	if len(args) != 2:
		sys.stderr.write('Need two args: input and output file\n')
		sys.exit(1)
	
	in_path, out_path = args
	
	main(in_path, out_path,
		options.tagger_config, options.tagger_dir,
		options.maca_config,
		options.unwanted_classes)

def main(in_path, out_path,
		tagger_config, tagger_dir,
		maca_config,
		unwanted_classes):
	
	# initialise tagger (do it once)
	tagr = tagger.Tagger(tagger_config, tagger_dir)
	# Maca config is specified in tagger config
	# optionally let the user override Maca config
	# (this probably won't be needed)
	if maca_config != '':
		tagr.maca_config = maca_config
	
	tagr.load_model()
	
	# prepare mask for easier testing of unwanted parts of-speech
	# the mask is in fact a prepared corpus2.Tag object
	if unwanted_classes == 'none':
		unwanted_mask = corpus2.Tag() # empty mask
	else:
		unwanted_mask = tagr.tagset.parse_symbol_string(unwanted_classes)
	
	
	# now request handling part
	with codecs.open(out_path, 'wb', 'utf-8') as out_stream:
		reader = tagger_io.get_reader(in_path, tagr.tagset, 'text', tagr.maca_config)
		
		while True:
			par = reader.get_next_chunk()
			if not par:
				break # end of input
			
			for sent in par.sentences():
				# preserve_ambiguity = False
				tagr.tag_sentence(sent, False)
				
				# iterate over tokens, get lemmas
				for tok in sent.tokens():
					# get "lexeme" (tag + lemma)
					lex = tok.get_preferred_lexeme(tagr.tagset)
					# check if this tag's part of speech is wanted
					testing_pos_mask = lex.tag().get_masked(unwanted_mask)
					if testing_pos_mask.is_null():
						# the tag has empty intersection with unwanted classes
						# so we want this token
						out_stream.write(unicode(lex.lemma()))
						# separate lemmas with spaces
						out_stream.write(u' ')
					# else: the token should be ignored
				# end each paragraph with a newline
				out_stream.write(u'\n')
		del reader


if __name__ == '__main__':
	go()
