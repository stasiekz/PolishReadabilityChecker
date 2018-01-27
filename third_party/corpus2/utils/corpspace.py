#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2012 Adam Radziszewski.
# This program is free software; you can redistribute and/or modify it
# under the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the LICENCE and COPYING files for more details

from optparse import OptionParser
import sys
import corpus2
from StringIO import StringIO
from collections import defaultdict as dd
descr = """%prog [options] TAGOUT MORPHO OUT

Util to synchronise no-space markers between tagger output (TAGOUT) that
contains the wanted disamb lexemes but may be devoid of no-space markers
with the tagger input containing proper no-space markers but no disambs.
"""



def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='xces',
		help='set the input format; default: xces')
	parser.add_option('-o', '--output-format', type='string', action='store',
		dest='output_format', default='xces',
		help='set the output format; default: xces')
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='nkjp',
		help='set the tagset used in input; default: nkjp')
	parser.add_option('-q', '--quiet', action='store_false', default=True, dest='verbose')
	parser.add_option('-d', '--debug', action='store_true', dest='debug_mode')
	(options, args) = parser.parse_args()
	
	if len(args) != 3:
		print 'You need to provide a TAGOUT, MORPHO and OUTPUT files.'
		print 'See --help for details.'
		print
		sys.exit(1)
	
	tag_fn, mor_fn, out_fn = args
	tagset = corpus2.get_named_tagset(options.tagset)
	
	tag_rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, tag_fn)
	mor_rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, mor_fn)
	
	writer = corpus2.TokenWriter.create_path_writer(options.output_format, out_fn, tagset)
	
	while True:
		mor_sent = mor_rdr.get_next_sentence()
		tag_sent = tag_rdr.get_next_sentence()
		assert (not mor_sent) == (not tag_sent)
		if not mor_sent:
			break
		for mor_tok, tag_tok in zip(mor_sent.tokens(), tag_sent.tokens()):
			assert unicode(mor_tok.orth()) == unicode(tag_tok.orth()), unicode(tag_tok.orth())
			tag_tok.set_wa(mor_tok.wa())
		writer.write_sentence(tag_sent)
		
	writer.finish()
	
if __name__ == '__main__':
	go()
