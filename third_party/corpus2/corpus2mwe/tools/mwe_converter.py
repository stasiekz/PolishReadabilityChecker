#!/usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
from corpus_ccl import cclutils as ccl
import corpus2mwe as mwe


def parser():
    aparser = argparse.ArgumentParser()
    aparser.add_argument('-c', '--ccl', dest='ccl_file', required=True)
    aparser.add_argument('-o', '--output', dest='out_file', required=True)
    aparser.add_argument('-m', '--mwe_merged', action='store_false')
    aparser.add_argument('-t', '--tagset', default='nkjp')
    return aparser


class MWEConverter(object):

    def __init__(self, tagset='nkjp'):
        self.reader = None
        if tagset:
            self.tagset = ccl.get_tagset(tagset)
        else:
            self.tagset = ccl.get_tagset('nkjp')

    def convert(self, ccl_file, out_mwe_file, annots_used=False):
        if not self.reader:
            self.reader = mwe.CclMWEReader(ccl_file, self.tagset)
            self.reader.use_annotations(annots_used)
        else:
            self.reader.set_files(ccl_file)
        mwe_doc = self.reader.read()
        ccl.write_ccl(mwe_doc, out_mwe_file)


def main(argv=None):
    aparser = parser()
    args = aparser.parse_args(argv)

    converter = MWEConverter(tagset=args.tagset)
    converter.convert(args.ccl_file, args.out_file, args.mwe_merged)


if __name__ == "__main__":
    main()
