#!/usr/bin/python
# -*- coding: utf-8 -*-

# This file is part of WCRFT
# Copyright (C) 2012 Adam Radziszewski.
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

from optparse import OptionParser
import sys
import corpus2
from StringIO import StringIO
from collections import defaultdict as dd
descr = """%prog [options] REF REANA OUT

Util to synchronise morphological analysis of a reference corpus (REF) with 
the same corpus but morphologically re-analysed and possibly re-tokenised
(REANA).

Subsequent tokens are compared (REF v. REANA). If the REF disamb interpretation
is still present in REANA, all the interpretations from there are taken and the
disamb marker is kept in the correct place. If the REF disamb interpretation is
not present in REANA, the REF token is substituted with an artificial one,
containing just ther REF disamb interpretation plus one with "unknown word" tag
and lemma equal to orthographic form.

NOTE: tokens subjected to segmentation change are left intact (their number
should be low, while it greatly simplifies processing).

NOTE 2: if a REF token contains more than one disamb interpretation, only one
will be chosen ("tagset-first").
"""

def text(tok_seq, respect_spaces, mark_boundaries = False):
	"""Extracts text from a sequence of tokens. If respect_spaces, will append
	spaces between tokens where no no-space markers present."""
	buff = StringIO()
	nonfirst = False
	for tok in tok_seq:
		if nonfirst and respect_spaces and tok.after_space():
			buff.write(' ')
		if mark_boundaries:
			buff.write('[')
		buff.write(tok.orth_utf8().decode('utf-8'))
		if mark_boundaries:
			buff.write(']')
		nonfirst = True
	return buff.getvalue()

def synced_sents(ref_rdr, rea_rdr, stats):
	"""Generates sentences with aligned token sequences from the two readers,
	containing synchronised morphology.
	Alignment is performed on the text level. Shortest token sequences with
	matching text are generated. Note: rdr_here is read sentence-by-sentence,
	while rdr_there is read token-by-token. Sentences from rdr_here always cut
	alignments.
	"""
	while True:
		ref_sent = ref_rdr.get_next_sentence()
		if not ref_sent:
			return
		# create a new Sentence object and append tokens there
		new_sent = corpus2.Sentence.create_sent(ref_sent.id())
		for chosen_tok in chosen_tokens(ref_sent, rea_rdr, stats):
			new_sent.append(chosen_tok.clone())
		yield new_sent


def chosen_tokens(ref_sent, rea_rdr, stats):
	for ref_seq, rea_seq in iter_tok_seqs(
				iter(ref_sent.tokens()), rea_rdr, stats):
		if len(ref_seq) == 1 and len(rea_seq) == 1:
			ref_tok = ref_seq[0]
			rea_tok = rea_seq[0]
			disamb_lex = ref_tok.get_preferred_lexeme(stats.tagset)
			reana_disambs = 0
			# try setting disambs to reana tok
			for rea_lex_idx in range(rea_tok.lexemes().size()):
				# must set disamb now for comparison
				rea_tok.lexemes()[rea_lex_idx].set_disamb(True)
				rea_lex = rea_tok.lexemes()[rea_lex_idx]
				if disamb_lex == rea_lex:
					reana_disambs += 1
				else:
					rea_tok.lexemes()[rea_lex_idx].set_disamb(False)
			
			if reana_disambs == 0:
				# reanalysis resulted in discarding of disamb lexeme
				# remove all-but-disamb lexeme, add unk_tag
				rea_tok.remove_all_lexemes()
				rea_tok.add_lexeme(disamb_lex)
				lemma_utf8 = unicode(rea_tok.orth()).lower().encode('utf-8')
				rea_tok.add_lexeme(
					corpus2.Lexeme.create_utf8(lemma_utf8, stats.unk_tag))
				stats.igned_toks += 1
			else:
				# seems ok
				assert reana_disambs == 1, (
						'%d reps of lexeme in "%s" (REANA corpus)' %
						(reana_disambs, rea_tok.orth_utf8()))
			# seems ready
			yield rea_tok
			stats.handled_toks += 1
		else:
			for ref_tok in ref_seq:
				# yield segchange tokens as they are, won't do any better
				yield ref_tok
				stats.segchange_toks += 1
		stats.ref_toks += len(ref_seq)
	

def iter_tok_seqs(iter_here, rdr_there, stats):
	
	tok_here = iter_here.next()
	tok_there = rdr_there.get_next_token()
	
	assert tok_here and tok_there, 'no input'
	
	buff_here = [tok_here]
	buff_there = [tok_there]
	
	LIMIT = 30
	
	num_iter = 0
	
	while True:
		num_iter += 1
		if stats.verbose_mode and num_iter % 10000 == 0:
			print num_iter, 'iterations...'
		if len(buff_here) > LIMIT or len(buff_there) > LIMIT:
			raise IOError('limit exceeded')
		
		text_here = text(buff_here, stats.respect_spaces)
		text_there = text(buff_there, stats.respect_spaces)
		
		if stats.debug_mode:
			print '%s (%d) v. %s(%d)' % (text_here, len(text_here),
			text_there, len(text_there))
		
		if len(text_here) == len(text_there):
			# already aligned
			if stats.verbose_mode and text_here != text_there:
				# the same text length but different chars
				# won't recover, flush it as it is
				print 'WARNING: mismatch', text_here, '|', text_there
			yield (buff_here, buff_there)
			buff_here = []
			buff_there = []
			tok_here = iter_here.next()
			tok_there = rdr_there.get_next_token()
			if not tok_here or not tok_there:
				assert not tok_here and not tok_there, 'one ended prematurely'
				break
			buff_here = [tok_here]
			buff_there = [tok_there]
		elif len(text_here) < len(text_there):
			# try fetching here
			tok_here = iter_here.next()
			assert tok_here, 'first ended prematurely'
			buff_here.append(tok_here)
		else: # len(text_here) > len(text_there):
			# try fetching there
			tok_there = rdr_there.get_next_token()
			assert tok_there, 'second ended prematurely'
			buff_there.append(tok_there)
	
	if buff_here or buff_there:
		assert buff_here and buff_there
		yield (buff_here, buff_there)

class Stats:
	def __init__(self, tagset, respect_spaces, verbose_mode, debug_mode):
		self.tagset = tagset
		self.unk_tag = tagset.make_ign_tag()
		self.respect_spaces = respect_spaces
		self.verbose_mode = verbose_mode
		self.debug_mode = debug_mode
		self.ref_toks = 0 # all in ref
		self.segchange_toks = 0 # subjected to segmentation differences
		self.handled_toks = 0 # besides segchange
		self.igned_toks = 0 # turned "unknown"
	
	def dump(self):
		print 'Reference: %d tokens' % self.ref_toks
		print 'Handled %d tokens (%.02f%%)' % (
			self.handled_toks, 100.0 * self.handled_toks / self.ref_toks)
		print 'UNKNOWN tokens: %d (%.02f%% of handled, %.02f%% of all ref)' % (
			self.igned_toks, 100.0 * self.igned_toks / self.handled_toks,
			100.0 * self.igned_toks / self.ref_toks)

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
	parser.add_option('-s', '--ignore-spaces', action='store_false',
		default=True, dest='respect_spaces',
		help='ignore spaces between tokens when comparing')
	parser.add_option('-d', '--debug', action='store_true', dest='debug_mode')
	(options, args) = parser.parse_args()
	
	if len(args) != 3:
		print 'You need to provide a REF, REANA and OUTPUT files.'
		print 'See --help for details.'
		print
		sys.exit(1)
	
	ref_fn, rea_fn, out_fn = args
	
	num_ref_toks = 0
	num_segchange_toks = 0
	num_handled_toks = 0
	num_igned_toks = 0
	
	tagset = corpus2.get_named_tagset(options.tagset)
	
	ref_rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, ref_fn)
	rea_rdr = corpus2.TokenReader.create_path_reader(options.input_format, tagset, rea_fn)
	
	writer = corpus2.TokenWriter.create_path_writer(options.output_format, out_fn, tagset)
	
	stats = Stats(tagset, options.respect_spaces, options.verbose, options.debug_mode)
	
	for sent in synced_sents(ref_rdr, rea_rdr, stats):
		writer.write_sentence(sent)
	
	if options.verbose:
		stats.dump()
	
if __name__ == '__main__':
	go()
