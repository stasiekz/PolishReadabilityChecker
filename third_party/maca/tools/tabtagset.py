#!/usr/bin/env python
# -*- coding: utf-8 -*-

#    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
#    Part of the Maca project
#
#    This program is free software; you can redistribute it and/or modify it
#    under the terms of the GNU Lesser General Public License as published by the Free
#    Software Foundation; either version 3 of the License, or (at your option)
#    any later version.
#
#    This program is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE.
#
#    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.

import sys, codecs
from optparse import OptionParser
from collections import defaultdict as dd
import tabclean

descr = """%prog [options] infile outfile

Utility to analyse tagset of morphological dictionaries. Reads input file
(by default, a tab-separated dictionary in Maca/Morfologik format or, when
using -t, plain list of tags) and outputs two files: outfile being a list of
all encountered tags and outfile.tagset being a list of tag usage patterns.
Each line of the output file corresponds to a subset of one grammatical class
with tags having a fixed number of attributes.

Maca -- copyright (C) 2010 Tomasz Sniatowski and Adam Radziszewski
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions (see LICENSE and COPYING).
"""

def tagreprs(infname, options):
	"""Reads the given whitespace-separated or just tag file and generates
	subsequent tag representations."""
	num_lines = 0
	with codecs.open(infname, 'rb', options.input_enc) as f:
		for line in f:
			line = line.strip()
			if line:
				entry = line.split() # form, lemma, tagrepr
				if options.tags_only:
					assert len(entry) == 1, ('unexpected whitespace: %s' % line)
					tagrepr = entry[0]
				else:
					assert len(entry) == 3, ('unexpected line format: %s' % line)
					tagrepr = entry[2]
				num_lines += 1
				if options.verbose and num_lines % 10000 == 0:
					tabclean._print_now('%d lines read...       \r' % num_lines)
				yield tagrepr
	if options.verbose:
		tabclean._print_now('Read all %d lines\n' % num_lines)

def process(infname, outfname, options):
	total = 0
	tagcount = dd(int) # (group name, tag) -> value
	classattrs = dd(lambda: dd(set)) # group name -> attr pos -> value set
	for tagrepr in tagreprs(infname, options):
		taglist = tabclean.decomp(tagrepr)
		for disc, taggroup in tabclean._group_disc(taglist):
			# gram class + number of attrs
			groupname = '%s/%d' % (disc[0], disc[1] - 1)
			for tag in taggroup:
				tagcount[(groupname, tag)] += 1
				for a_pos, a_val in enumerate(tag.split(':')):
					if a_pos > 0: # skipping grammatical class
						classattrs[groupname][a_pos].add(a_val)
					else:
						classattrs[groupname] # touch it so that we know this class exists
				total += 1
	if options.verbose:
		tabclean._print_now('Total: %d tag occurrences\n' % total)
	with codecs.open(outfname, 'wb', options.output_enc) as out:
		for groupname, tag in sorted(tagcount):
			out.write(u'%s\t%s\t%d\n' % (groupname, tag, tagcount[(groupname, tag)]))
	with codecs.open(outfname + '.tagset', 'wb', options.output_enc) as out:
		for groupname in sorted(classattrs):
			out.write(groupname)
			attrs = classattrs[groupname]
			out.write(u':\t')
			out.write(u'\t'.join(u'.'.join(sorted(attrs[pos])) for pos in sorted(attrs)))
			out.write(u'\n')

if __name__ == '__main__':
	def_enc = 'utf-8'
	parser = OptionParser(usage=descr)
	parser.add_option('-t', '--tags', action='store_true', dest='tags_only', help='read files with tags only')
	parser.add_option('--input-encoding', type='string', action='store', default=def_enc, dest='input_enc', help='set character encoding of input (default: %s)' % def_enc)
	parser.add_option('--output-encoding', type='string', action='store', default=def_enc, dest='output_enc', help='set character encoding of output (default: %s)' % def_enc)
	parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='show progress information')
	
	(options, args) = parser.parse_args()
	
	if len(args) != 2:
		print
		print 'You need to provide input and output filenames'
		parser.print_help()
		sys.exit(1)
	
	infname = args[0]
	outfname = args[1]
	
	process(infname, outfname, options)
