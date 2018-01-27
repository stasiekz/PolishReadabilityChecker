#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, sys, codecs, re
from optparse import OptionParser
from StringIO import StringIO

descr = """%prog [options] IN OUT

Attempts to convert JOSKIPI rules to WCCL rules.
NOTE: this is based on very naive heuristics. Dirty regex manipulation is done
instead of real parsing.

Known issues:
1. Will not skip superfluous none in non-empty sets.
2. Complex agreement/hasnum/isbig predicates will not be handled properly.
"""


p_strset = re.compile(u'{(\\s*"[^"]*"(\\s*,\\s*"[^"]*")*\\s*)}', re.U)
p_negposref = re.compile(u'\$\-([0-9]+)([A-Z][A-Za-z0-9]*)')
p_posposref = re.compile(u'\$[\+]?([0-9]+)([A-Z][A-Za-z0-9]*)')
p_isbig = re.compile(u'isbig\(([^)]*)\)', re.U)
p_hasnum = re.compile(u'hasnum\(([^)]*)\)', re.U)
p_agr = re.compile(u'(w?agr[a-z]*)\\s*\(\\s*([\-0-9]+)\\s*,\\s*([\-0-9]+)\\s*,\\s*({[^}]*})\\s*,\\s*([\-0-9]+)\\s*\)', re.U)

def jos2ccl(what):
	what = what.replace('{none}', '{}')
	what = what.replace('flex[', 'class[')
	what = p_negposref.sub(u'$\\2 - \\1', what)
	what = p_posposref.sub(u'$\\2 + \\1', what)
	what = p_strset.sub(u'[\\1]', what)
	what = p_isbig.sub(u'regex(\\1, "\\\\\\p{Lu}.*")', what)
	what = p_hasnum.sub(u'regex(\\1, ".*[0-9].*")', what)
	what = p_agr.sub(u'\\1(\\2, \\3, \\4)', what)
	return what
	
class IndentWriter:
	def __init__(self, out, baseindent = 0):
		self.out = out
		self.indent = baseindent
	
	def write(self, text):
		for line in text.split('\n'):
			line = line.strip()
			if line:
				meat = line.split('//', 1)[0]
				idelta = meat.count('(') - meat.count(')')
				nowindent = (self.indent + idelta) if meat.startswith(')') else self.indent
				self.out.write('\t' * nowindent)
				self.out.write(line)
				self.out.write('\n')
				self.indent += idelta
			else:
				self.out.write('\n')

class Rule:
	def __init__(self, text):
		pre, cond = text.split(':-')
		act, name = pre.split('#')
		self.cond = jos2ccl(cond.strip())
		self.act = jos2ccl(act.strip())
		self.name = name.strip()
	
	def write(self, out, sep = False):
		out.write('rule("%s",\n' % self.name)
		out.write('%s,\n' % self.cond)
		out.write('%s\n' % self.act)
		out.write(')%s\n' % (';' if sep else ''))

def rule_texts(infile):
	buf = StringIO()
	for line in infile:
		if 'delete' in line: # TODO: regex with other actions
			yield buf.getvalue()
			buf = StringIO()
		buf.write(line)
	yield buf.getvalue()

def rules(infile):
	for rt in rule_texts(infile):
		rt = '\n'.join(line.split('//', 1)[0] for line in rt.split('\n'))
		if rt.strip():
			yield Rule(rt)

def go():
	parser = OptionParser(usage=descr)
	parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='set the verbose mode')
	
	(options, args) = parser.parse_args()
	
	if len(args) != 2:
		print 'You need to provide input and output paths'
		print 'See %s --help' % os.path.basename(sys.argv[0])
		sys.exit(1)
	
	inpath, outpath = args
	
	inf = codecs.open(inpath, 'rb', 'utf-8')
	outf = codecs.open(outpath, 'wb', 'utf-8')
	w = IndentWriter(outf)
	
	indent = 0
	w.write('tag_rules(\n')
	# quick and dirty: to get all but last
	allrules = [r for r in rules(inf)]
	for rule in allrules[:-1]:
		rule.write(w, True)
	allrules[-1].write(w, False) # no comma
	w.write(')\n')
	
	outf.close()
	inf.close()

if __name__ == '__main__':
	go()
