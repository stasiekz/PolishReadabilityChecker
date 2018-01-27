#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from optparse import OptionParser
import ctypes
sys.setdlopenflags(sys.getdlopenflags() | ctypes.RTLD_GLOBAL)
import corpus2, wccl
from collections import defaultdict

format_help = """
Available input formats: """ + ' '.join(corpus2.TokenReader.available_reader_types()) + """
""" + ' '.join(corpus2.TokenReader.available_reader_types_help()) + """
Available output formats: """ + ' '.join(corpus2.TokenWriter.available_writer_types()) + """
""" + ' '.join(corpus2.TokenWriter.available_writer_types_help())

descr = """%prog [options] WCCLFILE
NOTE: if you want to fire match rules, you should use one of those options:
* use a format dedicated for annotations (ccl or iob-chan) or
* use any other format but pass an 'ann' option to input spec (e.g. xces,ann)

This program runs WCCL match and/or tag rules. Tag rules are applied first.
Then analyses properties of tokens: builds priority list based on property keys: qtype1 > qtype2 > qtype3 (set with WCCL config file) 
and outputs the most valuable property.

If you use raw 'xces' (or 'plain' etc.), the code will try to wrap the sentence
as AnnotatedSentence anyway, but the original sentence object may still be not
updated -- that is the changes made by the rules may be not present in the
output when using the writer to write the original object (if the object has
already been an AnnotatedSentence, the wrapper function will return itself
intact).

Example usage:
doc/wccl-qa.py examples/prop-match.ccl -i xces,ann -o ccl < examples/in-xces.xml

""" + format_help


QTYPE1 = 'qtype1'
QTYPE2 = 'qtype2'
QTYPE3 = 'qtype3'

def default_int():
	return 0

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


def process_sent(asent, wccl_file):
	if wccl_file.has_tag_rules():
		tag_rules = wccl_file.get_tag_rules_ptr()
		tag_rules.execute_once(asent)
	if wccl_file.has_match_rules():
		match_rules = wccl_file.get_match_rules_ptr()
		match_rules.apply_all(asent)

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='ccl',
		help='set the input format; default: ccl')
	parser.add_option('-o', '--output-format', type='string', action='store',
		dest='output_format', default='ccl',
		help='set the output format; default: ccl')
	parser.add_option('-r', '--result', type='string', action='store',
		dest='result_path', default='',
		help='set filename for script output, output is appended to the file content; default: stdout')
	parser.add_option('-I', '--input-file', type='string', action='store',
		dest='in_path', default='',
		help='set input filename (do not read from stdin)')
	parser.add_option('-O', '--output-file', type='string', action='store',
		dest='out_path', default='',
		help='set the output filename for processed input file with wccl rules applied')
	parser.add_option('-C', '--chunks', action='store_true',
		dest='preserve_chunks', default=False,
		help='preserve input paragraph chunks (the default is to read sentences only)')
	parser.add_option('-D', '--lex-dir', type='string', action='store',
		dest='lex_dir', default='.',
		help='use the given directory to look for lexicon files (default: .)')
	parser.add_option('-t', '--tagset', type='string', action='store',
		dest='tagset', default='kipi',
		help='set the tagset used in input; default: kipi')
	parser.add_option('-q', '--qtype', type='string', action='store',
		dest='default_qtype', default='unknown',
		help='set the question type for unresolved instances; default: unknown')

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

	writer = None
	if options.out_path != '':
		writer = get_writer(options.out_path, tagset, options.output_format)
	qtypes = list([(QTYPE1, defaultdict(default_int)), (QTYPE2, defaultdict(default_int)), (QTYPE3, defaultdict(default_int))])
	
	while True:
		chunk = reader.get_next_chunk()
		if not chunk:
			break # end of input
		# process each sentence separately
		for sent in chunk.sentences():
			asent = corpus2.AnnotatedSentence.wrap_sentence(sent)
			process_sent(asent, wccl_file)
			for token in asent.tokens():
				metadata = token.get_metadata()
				if metadata:
					for qtype_key, qtype_dict in qtypes:
						if metadata.has_attribute(qtype_key):
							attr = metadata.get_attribute(qtype_key)
							qtype_dict[attr] = qtype_dict[attr] + 1
		# save processed object
		# NOTE: if the input sent was not AnnotatedSentence, the changes
		# will be discarded
		if writer:
			if options.preserve_chunks:
				writer.write_chunk(chunk)
			else:
				for sent in chunk.sentences():
					writer.write_sentence(sent)

	max_label = options.default_qtype
	summary = defaultdict(default_int)
	for qtype_key, qtype_dict in qtypes:
		for key_word in qtype_dict.keys():
			summary[key_word] = summary[key_word] + qtype_dict[key_word]
		if not summary.values():
			continue
		max_value = max(summary.values())
		if summary.values().count(max_value) == 1:
			max_label = max(summary.keys(), key=lambda k: summary[k])
			break
	
	return_label(max_label, options)

def return_label(label, options):
	if options.result_path == '':
		print label
	else:
		f = open(options.result_path, "a")
		f.write(label + "\n")
		f.close()


if __name__ == '__main__':
	go()
