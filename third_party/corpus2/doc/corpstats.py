#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from optparse import OptionParser
from collections import defaultdict as dd
import corpus2

descr = """%prog [options] CORPUSFILE
Reads a corpus file and reports some statistics.
This script is a demo of the Python API.
"""

def tokens(rdr):
	"""Yields subsequent tokens from a reader.
	Declared here for demonstration."""
	while True:
		tok = rdr.get_next_token()
		if not tok:
			break
		yield tok

def sentences(rdr):
	"""Yields subsequent sentences from a reader.
	Declared here for demonstration."""
	while True:
		sent = rdr.get_next_sentence()
		if not sent:
			break
		yield sent

def chunks(rdr):
	"""Yields subsequent paragraphs from a reader."""
	while True:
		chunk = rdr.get_next_chunk()
		if not chunk:
			break
		yield chunk

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='xces',
		help='set the input format; default: xces')
	#parser.add_option('-o', '--output-format', type='string', action='store',
		#dest='output_format', default='xces',
		#help='set the output format; default: xces')
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='kipi',
		help='set the tagset used in input; default: kipi')
	parser.add_option('-v', '--verbose', action='store_true',
		dest='verbose', default=False,
		help='report each token')
	parser.add_option('-n', '--number-of-tags', type='int', action='store',
		dest='num_tags', default=10,
		help='set the max number of tags to report')
	(options, args) = parser.parse_args()
	
	if len(args) != 1:
		print 'You need to provide an input corpus.'
		print 'See %s --help' % sys.argv[0]
		sys.exit(1)
	
	inpath = args[0]
	# load a tagset, create a reader
	tagset = corpus2.get_named_tagset(options.tagset)
	rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, inpath)
	# init stats (for this example)
	num_toks, num_sents, num_chunks = 0, 0, 0
	tag_count = dd(int) 
	
	for chunk in chunks(rdr):
		for sent in chunk.sentences():
			for tok in sent.tokens():
				if options.verbose:
					print tok.orth_utf8()
				
				for lex in tok.lexemes():
					tag_str = tagset.tag_to_string(lex.tag())
					tag_count[tag_str] += 1
					
					if options.verbose:
						lemma = lex.lemma_utf8()
						print ('+' if lex.is_disamb() else ' '), lemma, tag_str
						# if you want a unicode object, orth_utf8().decode('utf-8')
				num_toks += 1
			num_sents += 1
		num_chunks += 1
		
	
	print 'Tokens:', num_toks
	print 'Sents: ', num_sents
	print 'Chunks:', num_chunks
	print
	print 'Most frequent tags:'
	for tc in sorted(tag_count.items(), key=lambda tc: (-tc[1], tc[0]))[:options.num_tags]:
		print '\t%s\t%d' % tc
		


if __name__ == '__main__':
	go()
