#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from optparse import OptionParser
import ctypes
sys.setdlopenflags(sys.getdlopenflags() | ctypes.RTLD_GLOBAL)
import corpus2, wccl

descr = """%prog [options] CORPUSFILE
Mimics (simplified) functionality of wccl-run.
This script is a demo of the Python API."""

def chunks(rdr):
	"""Yields subsequent sentences from a reader."""
	while True:
		chunk = rdr.get_next_chunk()
		if not chunk:
			break
		yield chunk

def iter_sent(sent):
	"""Iterates over a sentence, yielding the context with current_pos set
	to the subsequent tokens. NOTE: the same context object is returned each
	time, so tweaking with its state will affect iteration."""
	con = wccl.SentenceContext(sent)
	con.goto_start()
	while con.is_current_inside():
		yield con
		con.advance()

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='xces',
		help='set the input format; default: xces')
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='kipi',
		help='set the tagset used in input; default: kipi')
	(options, args) = parser.parse_args()
	
	ts = corpus2.get_named_tagset(options.tagset)
	p = wccl.Parser(ts)
	
	ops = [] # (name, op) pairs
	infiles = []
	for arg in args:
		if arg.endswith('.xml'):
			infiles.append(arg)
		elif arg.endswith('.ccl'):
			f = p.parseWcclFileFromPath(arg)
			ops.extend(f.gen_all_op_pairs())
		else:
			# parse arg as single op string
			op = p.parseAnyOperator(arg)
			ops.append((arg, arg))
	if ops and infiles:
		for fname in infiles:
			rdr = corpus2.TokenReader.create_path_reader(options.input_format, ts, fname)
			for chunk in chunks(rdr):
				# dump op names
				print '\t'.join(name for (name, _) in ops)
				# iterate and dump values
				for sent in chunk.sentences():
					for con in iter_sent(sent):
						print '\t'.join(op.base_apply(con).to_string(ts) for (_, op) in ops)



if __name__ == '__main__':
	go()
