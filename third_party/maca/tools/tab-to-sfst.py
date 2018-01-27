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

import sys
import codecs
import timeit

if __name__ == '__main__':
	if len(sys.argv) < 3:
		print """
	tab-to-sfst.py: convert whitespace-delimited data file to SFST format.
	Lines in the input file are expected to be in the following format:
	orth	lemma	tag

	The output file is usable by fst_compile -c, the output of which
	can be used as a morphology analyser module.

	Usage: tab-to-sfst.py infile outfile

	Progress info is output to stdout every 1000 lines processed.
	"""
		exit()


	def work():
		file = codecs.open(sys.argv[1], 'r', 'utf8')
		ofile = codecs.open(sys.argv[2], 'w', 'utf8')
		escapes = []
		for e in ('\\', ':', '-', '+', '.'):
			escapes.append((e, '\\' + e))
			
		def ewrite(olist, c):
			if c == '\\':
				c = '\\\\'
			elif c == ':':
				c = '\\:'
			elif c == '-':
				c = '\\-'
			elif c == '+':
				c = '\\+'
			elif c == '.':
				c = '\\.'
			#for e1, e2 in escapes:
			#	if c == e1:
			#		c = e2
			olist.append(c)
		zzlist = []
		li = 0
		for line in file:
			if line.strip():
				olist = []
				orth, lemma, tag = line.split()
				for e1, e2 in escapes:
					tag = tag.replace(e1, e2)
				i = 0
				m = min(len(orth), len(lemma))
				while (i < m):
					co = orth[i]
					cl = lemma[i]
					if co == cl:
						ewrite(olist, co)
					else:
						ewrite(olist, cl)
						olist.append(':')
						ewrite(olist, co)
					i = i + 1
				while (i < len(orth)):
					co = orth[i]
					olist.append('<>:')
					ewrite(olist, co)
					i = i + 1
				while (i < len(lemma)):
					cl = lemma[i]
					ewrite(olist, cl)
					olist.append(':<>')
					i = i + 1
				olist.append('<')
				olist.append(tag)
				olist.append('>:<>\n')
				li = li + 1
				if li % 1000 == 0:
					print "\r", li,
				zzlist.append(''.join(olist))
				olist = []
		ofile.write(''.join(zzlist))

	t = timeit.Timer(work)

	print t.timeit(1), "seconds elapsed"

