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
from copy import deepcopy
import itertools

descr = """%prog [options] infile outfile

Utility to remove duplicates and compact a morphological dictionary, possibly
converting forms and/or lemmas to lowercase. Useful for removing repetitions
from morphological dictionaries after some pre-processing, joining dictionaries
and converting data to lowercase.

Will read input tab-separated file, decompose tags, apply optional case
conversion, compact resulting entries and write back to outfile. Because of the
decompose-compact procedure, the output dictionary may look different even if
there are no repetitions.

NOTE: this script assumes that the input is sorted by the two first columns
(form and lemma). This is important as entries for a form-lemma pair may be
broken into several lines. If this doesn't hold for your input, either have it
sorted with the sort command or run the script with -u (will load all the
entries into memory before processing; this may take a horrible amount of
memory).

Maca -- copyright (C) 2010 Tomasz Sniatowski and Adam Radziszewski
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions (see LICENSE and COPYING).
"""

def entries(infname, options):
	"""Reads the given whitespace-separated file and generates
	(form, lemma, tagrepr) entries. Note: tagrepr is textual representation
	of tag or multiple tags, just as in input.
	"""
	num_lines = 0
	f = codecs.open(infname, 'rb', options.input_enc)
	for line in f:
		line = line.strip()
		if line:
			entry = line.split() # form, lemma, tagrepr
			assert len(entry) == 3, ('unexpected line format: %s' % line)
			num_lines += 1
			if options.verbose and num_lines % 10000 == 0:
				_print_now('%d lines read...       \r' % num_lines)
			yield entry
	f.close()
	if options.verbose:
		_print_now('\n')

def xform(entry, options):
	"""Returns an entry subjected to lowercase transformation as specd in
	options."""
	form, lemma, tagrepr = entry
	if options.lower_forms:
		form = form.lower()
	if options.lower_lemmas:
		lemma = lemma.lower()
	return (form, lemma, tagrepr)

def decomp(tagrepr):
	"""Returns a list of possibly duplicated tags corresponding to the given
	tagrepr. Tagrepr is first split on + and | characters and then values are
	split on dot character.
	NOTE: underscore is treated as any other value.
	
	>>> decomp('kl:aa1:aa2:aa3:_:aa4')
	['kl:aa1:aa2:aa3:_:aa4']
	>>> decomp('kl:a1.a2.a3:b')
	['kl:a1:b', 'kl:a2:b', 'kl:a3:b']
	>>> decomp('kl:a1.a2.a3:_')
	['kl:a1:_', 'kl:a2:_', 'kl:a3:_']
	>>> sorted(set(decomp('kl:a1.a2:b1.b2')))
	['kl:a1:b1', 'kl:a1:b2', 'kl:a2:b1', 'kl:a2:b2']
	>>> sorted(set(decomp('w:a1.a2:b1.b2.b3:c:d1.d2'))) #doctest: +NORMALIZE_WHITESPACE
	['w:a1:b1:c:d1', 'w:a1:b1:c:d2', 'w:a1:b2:c:d1', 'w:a1:b2:c:d2',
	'w:a1:b3:c:d1', 'w:a1:b3:c:d2', 'w:a2:b1:c:d1', 'w:a2:b1:c:d2',
	'w:a2:b2:c:d1', 'w:a2:b2:c:d2', 'w:a2:b3:c:d1', 'w:a2:b3:c:d2']
	>>> decomp('aa:bb1:cc+aa:bb2:cc')
	['aa:bb1:cc', 'aa:bb2:cc']
	>>> decomp('aa:bb1.bb2:cc|aa:bb3:cc')
	['aa:bb1:cc', 'aa:bb2:cc', 'aa:bb3:cc']
	>>> sorted(set(decomp('kl:a1.a2:b1.b2+kl:a2:b2.b3')))
	['kl:a1:b1', 'kl:a1:b2', 'kl:a2:b1', 'kl:a2:b2', 'kl:a2:b3']
	"""
	def app_to_tagv(tagv, to_add):
		# add first elems first
		for t in tagv:
			t.append(to_add[0])
		tlen = len(tagv)
		for e in to_add[1:]:
			for i in xrange(tlen):
				tagv.append(deepcopy(tagv[i])) # copy last one
				tagv[-1][-1] = e # replace last value with this one
	alltags = [] # a list of lists; [ tag1:[v1,v2,...], tag2:...]
	for prepart in tagrepr.split('+'):
		for part in prepart.split('|'):
			tags = [[]]
			fields = part.split(':')
			assert fields
			for field in fields:
				alters = field.split('.')
				app_to_tagv(tags, alters)
			alltags.extend(tags)
	return [':'.join(t) for t in alltags]

def _update(data, key, taglist):
	"""Updates data[key] with set(taglist) whether key in data or not.
	Runs slightly faster than defaultdict(set)."""
	if key not in data:
		data[key] = set(taglist)
	else:
		data[key].update(taglist)

def get_decomp_data(infname, options):
	"""Reads the given whitespace-separated file and generates dictionaries
	(form, lemma) -> set of simple tags (decomposed).
	If options.memorise is set, will yield one dict will all the entries;
	otherwise will keep yielding one-key dicts with sequences of entries
	having the same key."""
	data = {}
	last_key = None
	for entry in entries(infname, options):
		form, lemma, tagrepr = xform(entry, options)
		if not options.memorise and last_key != (form, lemma) and last_key is not None:
			yield data
			data = {}
		last_key = (form, lemma)
		taglist = decomp(tagrepr)
		_update(data, (form, lemma), taglist)
	yield data

def _groups(taglist):
	"""Divides the given taglist into groups, according to their wordclass
	(first attribute) and the number of elements. The groups are sorted
	respectively by these two criteria.
	
	>>> _groups(['aa:bb1:cc', 'aa:bb2:cc', 'zz:qq1', 'aa:bb3:cc', 'zz:qq2:optional'])
	[['aa:bb1:cc', 'aa:bb2:cc', 'aa:bb3:cc'], ['zz:qq1'], ['zz:qq2:optional']]
	"""
	
	def discr(t):
		return (t[0], len(t))
	
	# get a vector of list representations
	tagv = [tag.split(':') for tag in taglist]
	# get group discriminators: (wordclass, length) pairs
	gr_discrs = sorted(set(discr(t) for t in tagv))
	# now return a list of taglists, each corresponding to a disciminator
	return [[':'.join(t) for t in tagv if discr(t) == disc] for disc in gr_discrs]

def _group_disc(taglist):
	"""Returns a list of pairs (group_discriminator, group elements).
	This corresponds to _groups but also returns discriminators."""
	def discr(t):
		return (t[0], len(t))
	
	# get a vector of list representations
	tagv = [tag.split(':') for tag in taglist]
	# get group discriminators: (wordclass, length) pairs
	gr_discrs = sorted(set(discr(t) for t in tagv))
	# now return a list of taglists, each corresponding to a disciminator
	return [(disc, [':'.join(t) for t in tagv if discr(t) == disc]) for disc in gr_discrs]

def _compact_attr(tagv, a_id):
	"""Transforms the given list reprs of tags by compacting on the given attr
	index.
	
	>>> _compact_attr([['a1', 'b1'], ['a1', 'b2'], ['a2', 'b1'], ['a2', 'b2'], ['a2', 'b3']], 1)
	[['a1', 'b1.b2'], ['a2', 'b1.b2.b3']]
	"""
	
	def overridden(t, new_v): # v at a_id, the rest intact
		return [new_v if idx == a_id else v for idx, v in enumerate(t)]
	def masked(t):
		return tuple(overridden(t, None))
	
	# group tag reprs according to the values of other attrs (except a_id)
	other_vals = sorted(set(masked(t) for t in tagv))
	by_otherval = [sorted(set(t[a_id] for t in tagv if masked(t) == mask)) for mask in other_vals]
	# by_othervals is now a list of val sets, each val set corresponding to subsequent element of other_vals
	# now get a string representation of each val_set, accounting for _ (repr of all possible values)
	v_reprs = ['_' if '_' in vset else '.'.join(vset) for vset in by_otherval]
	return [overridden(t, v) for (t, v) in itertools.izip(other_vals, v_reprs)]
	
def _compact_group(taglist):
	"""Takes a list of tags representing one group and returns a compact
	representation (a string)."""
	tagv = [tag.split(':') for tag in taglist]
	assert tagv
	length = len(tagv[0]) # guaranteed that each has the same length
	# go over all the attrs, returning subsequent versions of tagv
	for a_id in reversed(range(length)):
		tagv = _compact_attr(tagv, a_id)
	# have them as strings
	return '+'.join([':'.join(t) for t in tagv])

def compact(taglist):
	"""Takes a list of tags and produces a list of compact tag representations.
	Tags are first grouped according to their wordlcass (i.e. first attribute)
	and each such group is then divided into subgroups according to tag lengths
	(i.e. the number tag of parts). Each subgroup is treated separately and
	finally output as a separate compact string representation.
	NOTE: not always an optimal representation is generated. The attributes are
	checked sequentially (in reverse order) and for each of them the tags are
	compacted by seeking duplicated projections. For some cases a different
	order of checking would result in more compact representation.
	
	>>> compact(['aa:bb1:cc', 'aa:bb2:cc', 'aa:bb3:cc'])
	['aa:bb1.bb2.bb3:cc']
	
	Order is irrelevant:
	>>> compact(['aa:bb2:cc', 'aa:bb1:cc', 'aa:bb3:cc'])
	['aa:bb1.bb2.bb3:cc']
	>>> compact(['aa:bb1:cc', 'aa:bb2:cc', 'zz:qq', 'aa:bb3:cc'])
	['aa:bb1.bb2.bb3:cc', 'zz:qq']
	>>> compact(['k:a1:b1:c:d1', 'k:a1:b1:c:d2', 'k:a1:b2:c:d1', 'k:a1:b2:c:d2',
	... 'k:a1:b3:c:d1', 'k:a1:b3:c:d2', 'k:a2:b1:c:d1', 'k:a2:b1:c:d2',
	... 'k:a2:b2:c:d1', 'k:a2:b2:c:d2', 'k:a2:b3:c:d1', 'k:a2:b3:c:d2'])
	['k:a1.a2:b1.b2.b3:c:d1.d2']
	"""
	return [_compact_group(gr) for gr in _groups(taglist)]

def _print_now(what):
	sys.stderr.write(what)
	sys.stderr.flush()

def convert(infname, outfname, options):
	with codecs.open(outfname, 'wb', options.output_enc) as out:
		for data in get_decomp_data(infname, options):
			# now have it saved
			num_lines = 0
			for key in sorted(data):
				set_of_tags = data[key]
				if set_of_tags: # just in case
					form, lemma = key
					# now generate a list of compact tag representations
					# (each beaing a string to store under (form, lemma))
					if options.write_groups:
						for disc, taggroup in _group_disc(set_of_tags):
							groupname = '%s/%d' % (disc[0], disc[1] - 1)
							tagrepr = _compact_group(taggroup)
							num_lines += 1
							if options.verbose and num_lines % 10000 == 0:
								_print_now('%d lines written...       \r' % num_lines)
							out.write(u'%s\t%s\t%s\t%s\n' % (groupname, form, lemma, tagrepr))
					else:
						tagreprs = compact(set_of_tags)
						for tagrepr in tagreprs:
							num_lines += 1
							if options.verbose and num_lines % 10000 == 0:
								_print_now('%d lines written...       \r' % num_lines)
							out.write(u'%s\t%s\t%s\n' % (form, lemma, tagrepr))
		if options.verbose:
			_print_now('\nDone.\n')

if __name__ == '__main__':
	def_enc = 'utf-8'
	parser = OptionParser(usage=descr)
	parser.add_option('-f', '--lower-forms', action='store_true', dest='lower_forms', default=False, help='convert forms to lowercase')
	parser.add_option('-l', '--lower-lemmas', action='store_true', dest='lower_lemmas', default=False, help='convert forms to lowercase')
	parser.add_option('--input-encoding', type='string', action='store', default=def_enc, dest='input_enc', help='set character encoding of input (default: %s)' % def_enc)
	parser.add_option('--output-encoding', type='string', action='store', default=def_enc, dest='output_enc', help='set character encoding of output (default: %s)' % def_enc)
	parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='show progress information')
	parser.add_option('-u', '--unsorted', action='store_true', dest='memorise', default=False, help='treat input as unsorted and read whole into memory before processing')
	parser.add_option('-g', '--write-groups', action='store_true', dest='write_groups', default=False, help='enrich output with groupname column')
	
	(options, args) = parser.parse_args()
	
	if len(args) != 2:
		print
		print 'You need to provide input and output filenames'
		parser.print_help()
		sys.exit(1)
	
	infname = args[0]
	outfname = args[1]
	
	convert(infname, outfname, options)
