#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2012 Adam Radziszewski, Paweł Orłowicz.
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

import ctypes, sys
sys.setdlopenflags(sys.getdlopenflags() | ctypes.RTLD_GLOBAL)

import maca
import corpus2
from optparse import OptionParser

descr = """%prog MACA_CONFIG_FILE CORPUS_FILE [options]
Reads a corpus file and outputs all or some tokens.
Available input formats: plain premorph
Available output formats: """ + ' '.join(corpus2.TokenWriter.available_writer_types()) + """
""" + ' '.join(corpus2.TokenWriter.available_writer_types_help())

def sentences(reader):
	"""Yields subsequent sentences from a reader."""
	while True:
		sentence = reader.get_next_sentence()
		if not sentence:
			break
		yield sentence

def chunks(reader):
	"""Yields subsequent sentences from a reader."""
	while True:
		chunk = reader.get_next_chunk()
		if not chunk:
			break
		yield chunk

def maca_analyse():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='txt',
		help='set the input format; available formats: txt, premorph')
	parser.add_option('-o', '--output-format', type='string', action='store',
		dest='output_format', default='xces',
		help='set the output format; default: xces')
	parser.add_option('-s', action='store_true', default=False, dest='split', help='Split output into chunks on many-newline tokens')
	(options, args) = parser.parse_args()

	if len(args) != 2:
		print "MACA_CONFIG_FILE and CORPUS_FILEPATH are required"
		return

	maca_config = args[0]
	filepath = args[1]

	reader = ''
	if options.input_format == 'txt':
		reader = maca.PlainTextReader.create_file_reader(filepath, maca_config)
	elif options.input_format == 'premorph':
		reader = maca.PremorphTextReader.create_file_reader(filepath, maca_config)
	else:
		print "Unknown input format."
		return
	
	writer = corpus2.TokenWriter.create_stdout_writer(options.output_format, reader.tagset())

	if options.split:
		for chunk in chunks(reader):
			writer.write_chunk(chunk)
	else:
		for sentence in sentences(reader):
			writer.write_sentence(sentence)
		
	

if __name__ == '__main__':
	maca_analyse()
