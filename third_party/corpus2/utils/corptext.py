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

descr = """%prog [options] INPUT OUTPUT

Reads input and saves as plain text. By default, paragraphs are separated with
two newlines, sentence division is not marked."""

from optparse import OptionParser
import sys, codecs
import corpus2

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='xces',
		help='set the input format; default: xces')
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='nkjp',
		help='set the tagset used in input; default: nkjp')
	parser.add_option('-p', '--par-sep', type='string', action='store',
		dest='par_sep', default='\n\n',
		help='set the paragraph separator; default: (two newlines)')
	parser.add_option('--ignore-ns-sent', action='store_true', default=False,
		dest='ignore_ns_sent', help='ignore no-space markers on sent boundaries')
	(options, args) = parser.parse_args()
	if len(args) != 2:
		print 'Need to provide input and output.'
		print 'See --help for details.'
		print
		sys.exit(1)
	
	fn_input, fn_output = args
	
	with codecs.open(fn_output, 'wb', 'utf-8') as out:
		tagset = corpus2.get_named_tagset(options.tagset)
		rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, fn_input)
		first = True
		while True:
			par = rdr.get_next_chunk()
			parfirst = True
			if not par:
				break
			for sent in par.sentences():
				sentfirst = True # if non-empty sent sep, skip pre-spaces
				for tok in sent.tokens():
					if not parfirst and ((sentfirst and options.ignore_ns_sent) or tok.after_space()):
						out.write(' ')
					out.write(unicode(tok.orth()))
					sentfirst = False
					parfirst = False
			out.write(options.par_sep)

if __name__ == '__main__':
	go()
