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

descr = """%prog [options] INPUT OUTDIR

Generates paragraph-wise folds by reading given input file (one possibly large
file containing the whole corpus). The folds are written into OUTDIR.
You may override the default seedword with any string you like using -s option.
Seedword's hash will be used as seed for pseudo-random number generator, which
in turn controlls the way input corpus paragraphs are shuffled before being
dispatched into folds. If you want to be able to reproduce your experiments
later, you may want to write down the seedword and include it in a log/readme
file where you store your folds and experimental results obtained
(along with corpus version etc.).
"""

from optparse import OptionParser
import sys, codecs, os, random
import corpus2

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
	parser.add_option('-f', '--num-folds', type='int', action='store',
		dest='num_folds', default='10',
		help='set the number of folds (default: 10)')
	parser.add_option('-s', '--seed-word', type='string', action='store',
		dest='seedword', default='korpus',
		help='set the seedword; default: korpus')
	parser.add_option('-v', '--verbose', action='store_true',
		dest='verbose', default=False,
		help='verbose mode')
	
	(options, args) = parser.parse_args()
	if len(args) != 2:
		print 'Need to provide input file and output dir.'
		print 'See --help for details.'
		print
		sys.exit(1)
	
	fold_nums = range(options.num_folds)
	fn_input, fold_dir = args
	
	tagset = corpus2.get_named_tagset(options.tagset)
	# count paragraphs in input
	if options.verbose:
		sys.stderr.write('Counting paragraphs... ')
	rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, fn_input)
	num_pars = 0
	while True:
		par = rdr.get_next_chunk()
		if not par:
			break
		num_pars += 1
	del rdr
	if options.verbose:
		sys.stderr.write('%d\n' % num_pars)
	# prepare index -- where to send ith paragraph
	rnd = random.Random(options.seedword)
	fold_of_par = [(par_idx % options.num_folds) for par_idx in xrange(num_pars)]
	rnd.shuffle(fold_of_par)
	# now the real run
	if options.verbose:
		sys.stderr.write('Generating folds...\n')
	rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, fn_input)
	fold_test = [corpus2.TokenWriter.create_path_writer(
			options.output_format,
			os.path.join(fold_dir, 'test%02d.xml' % (num + 1)), tagset)
			for num in fold_nums]
	fold_train = [corpus2.TokenWriter.create_path_writer(
			options.output_format,
			os.path.join(fold_dir, 'train%02d.xml' % (num + 1)), tagset)
			for num in fold_nums]
	first = True
	par_now = 0
	while True:
		par = rdr.get_next_chunk()
		if not par:
			break
		fold_now = fold_of_par[par_now]
		fold_test[fold_now].write_chunk(par)
		for other_num in fold_nums:
			if other_num != fold_now:
				fold_train[other_num].write_chunk(par)
		
		#fold_now = (fold_now + 1) % options.num_folds
		par_now += 1
	del rdr
	for w in fold_test: w.finish()
	for w in fold_train: w.finish()

if __name__ == '__main__':
	go()
