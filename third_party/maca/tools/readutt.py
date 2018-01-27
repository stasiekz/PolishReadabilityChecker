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

import sys, codecs, re
from optparse import OptionParser
from collections import defaultdict as dd
import tabclean

p_lemma = re.compile(u'(([0-9]+)([^-0-9]*)-)?(([0-9]+)([^-0-9]*))$', re.U)

descr = """%prog [options] infile outfile

Utility to read UTT/lem format. Outputs Maca-compliant tab-separated file
(outfile) and tagset definition file inferred from the input (outfile.tagset).

By default attribute names are prefixed with 'a', value names are prefixed with
original attribute name. Use -m to pass external remapping file (two-column
plain text file) where the names (prefixed) are mapped to desired ones.

Maca -- copyright (C) 2010 Tomasz Sniatowski and Adam Radziszewski
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions (see LICENSE and COPYING).
"""

class Renamer:
	def __init__(self, fname):
		self.r = dict()
		if fname:
			with open(fname, 'r') as f:
				for line in f:
					if line.strip():
						k, v = line.split()
						self.r[k] = v
	
	def out_wclass(self, value):
		return self.r.get(value, value)
	
	def out_attr(self, value):
		o_attr = 'a' + value
		return self.r.get(o_attr, o_attr)
	
	def out_val(self, i_attr, value):
		o_val = i_attr + value
		return self.r.get(o_val, o_val)
	
class TagRepr:
	def __init__(self, mortagrepr, renamer):
		self.renamer = renamer
		self.i_attrs = [] # list of attr names as in input
		self.o_attrs = [] # list of attr names for output (prefixed)
		self.i_av = {} # attr -> val set
		self.o_av = {} # attr -> val set
		items = mortagrepr.split('/')
		if len(items) != 1:
			assert len(items) == 2
			i_attr = None
			o_attr = None
			new_attr = False
			for char in items[1]:
				if char.isupper(): # we've got attr name
					if new_attr: # attr with no value given
						self._add_val(i_attr, o_attr, 'X')
					i_attr = char
					o_attr = self.renamer.out_attr(char)
					self.i_attrs.append(i_attr)
					self.o_attrs.append(o_attr)
					new_attr = True
				else:
					new_attr = False
					self._add_val(i_attr, o_attr, char)
			# last attr with no value given?
			if new_attr:
				self._add_val(i_attr, o_attr, 'X')
		self.wclass = self.renamer.out_wclass(items[0])
	
	def _add_val(self, i_attr, o_attr, value):
		assert value
		assert i_attr, value # attr name for value char must be defined
		assert o_attr, value # attr name for value char must be defined
		# in: add value to i_attr as it is
		self.i_av[i_attr] = self.i_av.get(i_attr, set()).union([value])
		# out: prefix value with input attr name
		self.o_av[o_attr] = self.o_av.get(o_attr, set()).union([self.renamer.out_val(i_attr, value)])
	
	def out_repr(self):
		"""Returns KIPI-like representation using o_av and o_attrs (prefixed
		versions of attr names and values)."""
		return ':'.join([self.wclass] + ['.'.join(sorted(self.o_av[attr])) for attr in self.o_attrs])

def get_lemma(form, lemmarepr):
	"""Restores lemma from form and lemmarepr (rule).
	
	Doctest fails at unicode, hence these crippled forms.
	
	>>> get_lemma(u'kocie', u'3t')
	u'kot'
	>>> get_lemma(u'najbielsi', u'3-4aly')
	u'bialy'
	>>> get_lemma(u'najbogobojniejszymi', u'10bogoboj-8y')
	u'bogobojny'
	"""
	m = p_lemma.match(lemmarepr)
	assert m, lemmarepr # die if unmatched
	part1, cut1, add1, part2, cut2, add2 = m.groups()
	if cut1:
		add1 = '' if add1 is None else add1
		form = add1 + form[int(cut1):]
	assert cut2 is not None
	cut2 = int(cut2)
	if cut2:
		form = form[:-cut2]
	if add2:
		form = form + add2
	assert form
	return form

def entries(infname, options):
	"""Reads UTT/lem file and generates (form, lemma, tagrepr) tuples.
	NOTE that tagreprs are kept in UTT/lem input format."""
	num_lines = 0
	f = codecs.open(infname, 'rb', options.input_enc)
	for line in f:
		line = line.strip()
		if line:
			entry = line.split(';') # form, (lemmarepr, tagrepr) or form, (lemmarepr, tagrepr), (lemmarepr2, tagrepr2)
			form = entry[0]
			for alter in entry[1:]:
				lemmarepr, tagrepr = alter.split(',')
				lemma = get_lemma(form, lemmarepr)
				yield (form, lemma, tagrepr)
			num_lines += 1
			if options.verbose and num_lines % 10000 == 0:
				tabclean._print_now('%d lines read...       \r' % num_lines)
	f.close()
	if options.verbose:
		tabclean._print_now('\n')

def process(infname, outfname, options):
	total = 0
	attrval = dd(set) # attr -> values
	classattr = dd(set) # class -> attrs
	renamer = Renamer(options.remap)
	with codecs.open(outfname, 'wb', options.output_enc) as out:
		for form, lemma, tagrepr in entries(infname, options):
			# wclass, av = get_tag_parts(tagrepr)
			tag = TagRepr(tagrepr, renamer)
			classattr[tag.wclass].update(tag.o_attrs) # add the attrs to wclass attrs
			for attr in tag.o_attrs:
				attrval[attr].update(tag.o_av[attr]) # add attrval[attr] to the domain of attr
			out.write(u'%s\t%s\t%s\n' % (form, lemma, tag.out_repr())) # TODO: write converted
			total += 1
	if options.verbose:
		tabclean._print_now('Total: %d tag occurrences\n' % total)
	
	with codecs.open(outfname + '.tagset', 'wb', options.output_enc) as out:
		out.write('[ATTR]\n')
		for attr in sorted(attrval):
			out.write(attr)
			if attrval[attr]:
				out.write('\t')
				out.write(' '.join(sorted(attrval[attr])))
			out.write('\n')
		out.write('\n')
		out.write('[POS]\n')
		for wclass in sorted(classattr):
			out.write(wclass)
			if classattr[wclass]:
				out.write('\t')
				out.write(' '.join(sorted(classattr[wclass])))
			out.write('\n')
		out.write('\n')

if __name__ == '__main__':
	def_enc = 'utf-8'
	parser = OptionParser(usage=descr)
	parser.add_option('-m', '--remap', type='string', action='store', default=None, dest='remap', help='set character encoding of input (default: %s)' % def_enc)
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
