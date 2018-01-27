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

__doc__ = """A simple script for testing validity of tabclean output."""

descr = """usage: %s infile outfile

Will read infile (in tab-separated format) and join multiple lines with the
same form and lemma. Such cases will be represented as single lines where tag
representations will be joined with +. This is a preprocessing step for
comparing two tab-separated files. The next step is to decompose each tag
representation and compare sets of tags assigned to each (form, lemma) pair.
"""

import codecs, sys
from collections import defaultdict as dd

def entries(infname):
	"""Reads the given whitespace-separated file and generates
	(form, lemma, tagrepr) entries. Note: tagrepr is textual representation
	of tag or multiple tags, just as in input.
	"""
	f = codecs.open(infname, 'rb', 'utf-8')
	for line in f:
		line = line.strip()
		if line:
			entry = line.split() # form, lemma, tagrepr
			assert len(entry) == 3, ('unexpected line format: %s' % line)
			yield entry
	f.close()

def process(infname, outfname):
	"""Reads tab file and writes a tabfile but with multiple entries for
	(lemma, tag) joined into single ones (with +)."""
	data = dd(list)
	print 'Reading data...'
	for form, lemma, tagrepr in entries(infname):
		data[(form, lemma)].append(tagrepr)
	print 'Writing data one line per (form, lemma)'
	with codecs.open(outfname, 'wb', 'utf-8') as out:
		for key in sorted(data):
			if data[key]:
				out.write(u'%s\t%s\t%s\n' % (key[0], key[1], '+'.join(sorted(data[key]))))
	print 'Done'

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print descr % sys.argv[0]
		sys.exit(1)
	
	infname = sys.argv[1]
	outfname = sys.argv[2]
	process(infname, outfname)
