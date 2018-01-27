#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from optparse import OptionParser

# There is some bug in SWIG appearing when using two SWIG-wrapped libraries
# simultaneously. The bug manifests itself in throwing unwrapped
# swig::stop_iteration exceptions when iterating over C++ collections
# (e.g. iterating over sentence tokens).
# The lines below may be necessary to work around this bug. Consider adding
# them before importing corpus2 and wccl in your code.
import ctypes
sys.setdlopenflags(sys.getdlopenflags() | ctypes.RTLD_GLOBAL)
import corpus2, wccl

# helper string containing input/output formats available in corpus2
format_help = """
Available input formats: """ + ' '.join(corpus2.TokenReader.available_reader_types()) + """
""" + ' '.join(corpus2.TokenReader.available_reader_types_help()) + """
Available output formats: """ + ' '.join(corpus2.TokenWriter.available_writer_types()) + """
""" + ' '.join(corpus2.TokenWriter.available_writer_types_help())

descr = """%prog [options] WCCLFILE
Mimics (simplified) functionality of wccl-rules.
This script is a demo of the Python API.

NOTE: if you want to fire match rules, you should use one of those options:
* use a format dedicated for annotations (ccl or iob-chan) or
* use any other format but pass an 'ann' option to input spec (e.g. xces,ann)

If you use raw 'xces' (or 'plain' etc.), the code will try to wrap the sentence
as AnnotatedSentence anyway, but the original sentence object may still be not
updated -- that is the changes made by the rules may be not present in the
output when using the writer to write the original object (if the object has
already been an AnnotatedSentence, the wrapper function will return itself
intact).

Example usage:
doc/wccl-rules.py examples/simple-match.ccl -i xces,ann -o ccl < examples/in-xces.xml

To see annotation manipulation with Python:
doc/wccl-rules.py -A examples/simple-match.ccl -i xces,ann -o none < examples/in-xces.xml

""" + format_help

def get_reader(in_path, tagset, input_format):
	"""Creates a reader using the options. If in_path evaluates to False,
	will create a stdin reader."""
	if in_path:
		return corpus2.TokenReader.create_path_reader(
			input_format, tagset, in_path)
	else:
		return corpus2.TokenReader.create_stdin_reader(input_format, tagset)

def get_writer(out_path, tagset, output_format):
	"""Creates a writer using the options. If out_path evaluates to False,
	will create a stdout writer."""
	if out_path:
		return corpus2.TokenWriter.create_path_writer(output_format, out_path,
			tagset)
	else:
		return corpus2.TokenWriter.create_stdout_writer(output_format, tagset)

def process_sent(asent, wccl_file, shall_print_ann_info):
	if wccl_file.has_tag_rules():
		tag_rules = wccl_file.get_tag_rules_ptr()
		tag_rules.execute_once(asent)
	if wccl_file.has_match_rules():
		match_rules = wccl_file.get_match_rules_ptr()
		match_rules.apply_all(asent)
	if shall_print_ann_info:
		print '####', ' '.join(tok.orth_utf8() for tok in asent.tokens())
		print asent.annotation_info()
		for chan_name in asent.all_channels():
			chan = asent.get_channel(chan_name)
			# get the internal representation -- annotation id vector
			print chan_name, '\t', chan.dump_segments()
			# if you want to get it as a int vector, use chan.segments()
			# or to get one it at idx -- chan.get_segment_at(idx)
			# 0 means no annot there, positive values==ann indices
			
			# now iterate over annotations as whole segments
			ann_vec = chan.make_annotation_vector()
			for ann in ann_vec:
				idx_text = ','.join(str(idx) for idx in ann.indices)
				orth_text = ' '.join(asent.tokens()[idx].orth_utf8() for idx in ann.indices)
				print '\t%s\t%s\t(%s)' % (chan_name, idx_text, orth_text)


def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='kipi',
		help='set the tagset used in input; default: kipi')
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='xces',
		help='set the input format; default: xces')
	parser.add_option('-o', '--output-format', type='string', action='store',
		dest='output_format', default='xces',
		help='set the output format; default: xces')
	parser.add_option('-I', '--input-file', type='string', action='store',
		dest='in_path', default='',
		help='set input filename (do not read from stdin)')
	parser.add_option('-O', '--output-file', type='string', action='store',
		dest='out_path', default='',
		help='set output filename (do not write to stdout)')
	parser.add_option('-C', '--chunks', action='store_true',
		dest='preserve_chunks', default=False,
		help='preserve input paragraph chunks (the default is to read sentences only)')
	parser.add_option('-D', '--lex-dir', type='string', action='store',
		dest='lex_dir', default='.',
		help='use the given directory to look for lexicon files (default: .)')
	parser.add_option('-A', '--ann-info', action='store_true',
		dest='ann_info', default=False,
		help='print annotation info')
	
	(options, args) = parser.parse_args()
	
	if len(args) != 1:
		sys.stderr.write('You need to provide a WCCL file.\n')
		sys.stderr.write('See %s --help\n' % sys.argv[0])
		sys.exit(1)
	
	wccl_path = args[0]
	
	# create a tagset object for all subsequent processing
	# when reading input corpus, when creating new tags/tokens and when
	# creating a WCCL parser it must be specified what tagset is being used
	tagset = corpus2.get_named_tagset(options.tagset)
	
	# now instantiate a WCCL parser and parse the provided WCCL file
	# lex_dir is optional -- if the WCCL code references external
	# lexicons, they will be sought in the given search path
	# (defaults to '.')
	p = wccl.Parser(tagset)
	wccl_file = p.parseWcclFileFromPath(wccl_path, options.lex_dir)
	# check if there are any rules in the parsed WCCL file
	if not wccl_file.has_tag_rules() and not wccl_file.has_match_rules():
		sys.stderr.write('The given WCCL file contains no rules.\n')
		sys.exit(1)
	
	# create a corpus2 reader using the given input format specs
	# (e.g. xces,flat), tagset name
	# if options.in_path is empty, will create an stdin reader
	reader = get_reader(options.in_path, tagset, options.input_format)
	# create a corpus2 writer -- will be used to write the processed corpus
	writer = get_writer(options.out_path, tagset, options.output_format)
	
	# processing paragraph-by-paragraph
	if options.preserve_chunks:
		while True:
			chunk = reader.get_next_chunk()
			if not chunk:
				break # end of input
			# process each sentence separately
			for sent in chunk.sentences():
				# wrap the sentence as an AnnotatedSentence
				asent = corpus2.AnnotatedSentence.wrap_sentence(sent)
				process_sent(sent, wccl_file, options.ann_info)
			# save processed chunk
			# NOTE: if the input sent was not AnnotatedSentence, the changes
			# will be discarded
			writer.write_chunk(chunk)
	else:
		while True:
			sent = reader.get_next_sentence()
			if not sent:
				break # end of input
			# wrap the sentence as an AnnotatedSentence
			asent = corpus2.AnnotatedSentence.wrap_sentence(sent)
			process_sent(asent, wccl_file, options.ann_info)
			# save processed sentence (safe)
			# NOTE: if the input sent was not AnnotatedSentence, the changes
			# will be discarded
			writer.write_sentence(sent)

if __name__ == '__main__':
	go()
