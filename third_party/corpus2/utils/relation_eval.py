#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2012 Paweł Orłowicz.
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

descr = """%prog [options] BATCH_REF BATCH_TARGET REL_NAME

Reads the two files BATCH_REF and BATCH_TARGET which contain lists of pairs 
of files (separated with semicolon, one pair per line):
chunk_annotated_corpora.xml;relation_annotated_corpora.xml

REL_NAME describes the name of relation to be counted

Outputs precision and recall values for the following settings:
1. chunk + head recognition (as above + heads placed on the same tokens)
2. chunk recognition (counting as hit when exactly same tokens)
3. head recognition alone (only head position is compared)
"""


from optparse import OptionParser
import sys
import corpus2

class RelStats:
	def __init__(self):
		self.both_hits = 0
		self.head_hits = 0
		self.chun_hits = 0
		self.any_hits = 0

	#helper method to get annotation vector from annotated sentence
	def get_channel_annotations(self, ann_sent, dir_point):
		chann_name = dir_point.channel_name()
		annotation_number = dir_point.annotation_number() - 1
		channel = ann_sent.get_channel(chann_name)
		ann_vec = channel.make_annotation_vector()
		return ann_vec[annotation_number]

	#helper method to get list of tokens' indices
	def get_indices(self, annotated_sentence, direction_point):
		ann_chann  = self.get_channel_annotations(annotated_sentence, direction_point)
		indices = ann_chann.indices
		#loop to unwrap Integer objects from ann_chann.indices
		inds = []
		cap = indices.capacity()
		for i in range(cap) :
			inds.append(indices[i])
		return inds

	#helper to get index of the chunk's head
	def get_head_index(self, annotated_sentence, direction_point):
		ann_chann = self.get_channel_annotations(annotated_sentence, direction_point)
		head_index = ann_chann.head_index
		return head_index

	#returns values of hits from one direction point of relation
	def verify_relation(self, ref_ann_sent, dir_point_ref, target_ann_sent, dir_point_target):
		both, head, chun = 0,0,0
		#if indices from ref chunk and target chunks equals (tokens are the same) then chun hits
		if self.get_indices(ref_ann_sent, dir_point_ref) == self.get_indices(target_ann_sent, dir_point_target):
			chun = 1
#			if chun hits and head indices match then head hits
#			if self.get_head_index(ref_ann_sent, dir_point_ref) == self.get_head_index(target_ann_sent, dir_point_target): 
#				head =1
		#if indices are different (chunks consists of different sets of words) but heads match then head hits
		if self.get_head_index(ref_ann_sent, dir_point_ref) == self.get_head_index(target_ann_sent, dir_point_target):
			head = 1
		if chun == 1 and head == 1:
			both = 1
		return both,chun,head

	#if there was a hit on both sides of relation (dir_from, dir_to) then update counters
	def update_stats(self, both, chun, head):
		if chun == 2:
			self.chun_hits+=1
		if head == 2:
			self.head_hits += 1
		if chun == 2 and head == 2:
			self.both_hits += 1
		if both > 0 and chun+head > 2:
			self.any_hits+=1
		if both == 0 and chun+head > 1:
			self.any_hits+=1

	def print_stats(self,ref_rels_count, target_rels_count, stat_mode):
		p = 0.0 if target_rels_count == 0 else 100.0 * self.any_hits / target_rels_count
		r = 0.0 if ref_rels_count == 0 else 100.0 * self.any_hits / ref_rels_count
		f = 0.0 if p + r == 0.0 else 2.0 * p * r / (p + r)
		if not stat_mode : 
			print ('Any chunk or head match:\t')
		print '%.2f\t%.2f\t%.2f' % (p, r, f) 
		p = 0.0 if target_rels_count == 0 else 100.0 * self.both_hits / target_rels_count
		r = 0.0 if ref_rels_count == 0 else 100.0 * self.both_hits / ref_rels_count
		f = 0.0 if p + r == 0.0 else 2.0 * p * r / (p + r)
		if not stat_mode : 
			print ('Chunk and head match:\t')
		print '%.2f\t%.2f\t%.2f' % (p, r, f) 
		p = 0.0 if target_rels_count == 0 else 100.0 * self.chun_hits / target_rels_count
		r = 0.0 if ref_rels_count == 0 else 100.0 * self.chun_hits / ref_rels_count
		f = 0.0 if p + r == 0.0 else 2.0 * p * r / (p + r)
		if not stat_mode : 
			print ('Chunk match:\t')
		print '%.2f\t%.2f\t%.2f' % (p, r, f) 
		p = 0.0 if target_rels_count == 0 else 100.0 * self.head_hits / target_rels_count
		r = 0.0 if ref_rels_count == 0 else 100.0 * self.head_hits / ref_rels_count
		f = 0.0 if p + r == 0.0 else 2.0 * p * r / (p + r)
		if not stat_mode : 
			print ('Head match:\t')
		print '%.2f\t%.2f\t%.2f' % (p, r, f) 

def compare(rel1, rel2):
	dp1_from = rel1.rel_from()
	dp2_from = rel2.rel_from()
	dp1_to = rel1.rel_to()
	dp2_to = rel2.rel_to()
	if cmp(dp1_from.sentence_id(), dp2_from.sentence_id()) < 0:
		return -1
	elif cmp(dp1_from.sentence_id(), dp2_from.sentence_id()) > 0:
		return 1
	if cmp(dp1_from.channel_name(), dp2_from.channel_name()) < 0:
		return -1
	elif cmp(dp1_from.channel_name(), dp2_from.channel_name()) > 0:
		return 1
	if cmp(dp1_from.annotation_number(), dp2_from.annotation_number()) < 0:
		return -1
	elif cmp(dp1_from.annotation_number(), dp2_from.annotation_number()) > 0:
		return 1

	if cmp(dp1_to.sentence_id(), dp2_to.sentence_id()) < 0:
		return -1
	elif cmp(dp1_to.sentence_id(), dp2_to.sentence_id()) > 0:
		return 1
	if cmp(dp1_to.channel_name(), dp2_to.channel_name()) < 0:
		return -1
	elif cmp(dp1_to.channel_name(), dp2_to.channel_name()) > 0:
		return 1
	if cmp(dp1_to.annotation_number(), dp2_to.annotation_number()) < 0:
		return -1
	elif cmp(dp1_to.annotation_number(), dp2_to.annotation_number()) > 0:
		return 1

	if rel1.rel_name() < rel2.rel_name():
		return -1
	elif rel1.rel_name() > rel2.rel_name():
		return 1

	return 0

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='nkjp',
		help='set the tagset used in input; default: nkjp')
	parser.add_option('-s', '--stat', action='store_true',
		dest='stat_mode',
		help='output P,R,f with no text labels, order like in normal mode: \n Chunks or heads \n Chunks and heads \n Chunks match \n Heads match')
	(options, args) = parser.parse_args()
	
	stat_mode = options.stat_mode
	
	if len(args) != 3:
		sys.stderr.write('No args. See --help\n')
		sys.exit(1)

	batch_ref, batch_target, rel_name = args
	rel_stats = RelStats()

	corpus_type = "document"
	tagset = corpus2.get_named_tagset(options.tagset)

	ref_count = 0
	target_count = 0

	ref_file = open(batch_ref, "r")
	target_file = open(batch_target, "r")
	line_ref = ref_file.readline()
	line_target = target_file.readline()
	while line_ref and line_target:

		line_ref = line_ref.strip()
		ref_ccl_filename, ref_rel_filename = line_ref.split(";")
	
		line_target = line_target.strip()
		target_ccl_filename, target_rel_filename = line_target.split(";")
	
		ref_ccl_rdr = corpus2.CclRelReader(tagset, ref_ccl_filename, ref_rel_filename)
		target_ccl_rdr = corpus2.CclRelReader(tagset, target_ccl_filename, target_rel_filename)

		ref_doc = ref_ccl_rdr.read()
		target_doc = target_ccl_rdr.read()

		ref_rels = list( r for r in ref_doc.relations() if r.rel_name() == rel_name )
		target_rels = list( t for t in target_doc.relations() if t.rel_name() == rel_name)
		ref_count += len(ref_rels)
		target_count += len(target_rels)

		ref_sents = dict([ (s.id(), corpus2.AnnotatedSentence.wrap_sentence(s)) for c in ref_doc.paragraphs() for s in c.sentences()])
		target_sents = dict([ (s.id(), corpus2.AnnotatedSentence.wrap_sentence(s)) for c in target_doc.paragraphs() for s in c.sentences()])

		for pattern in ref_rels:
			t = filter(lambda x : (compare(x, pattern) == 0) , target_rels)
			if len(t) > 0:
				t = t[0]
				r = pattern
			
				both, chun, head = 0,0,0
				for dir_point_ref, dir_point_target in zip([r.rel_from(), r.rel_to()], [t.rel_from(), t.rel_to()]):
					ref_ann_sent = ref_sents[dir_point_ref.sentence_id()]
					target_ann_sent = target_sents[dir_point_target.sentence_id()]
					b,c,h = rel_stats.verify_relation(ref_ann_sent, dir_point_ref, target_ann_sent, dir_point_target)
					both,chun,head = map(sum, zip([b,c,h],[both,chun,head]))
				rel_stats.update_stats(both, chun, head)
		line_ref = ref_file.readline()
		line_target = target_file.readline()
	
	rel_stats.print_stats(ref_count, target_count, stat_mode)

if __name__ == '__main__':
	go()

