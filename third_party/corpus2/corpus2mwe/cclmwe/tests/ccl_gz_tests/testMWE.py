#!/bin/python
# -*- coding: utf-8 -*-

import unittest
import tempfile
import shutil
import corpus2, corpus2mwe
import os
import codecs
import documents

class ccl_gz_mwe_test(unittest.TestCase):
    def test_one_sentence_path_writer(self):
        path = self._save_temp_file('one_sentence_path_writer.ccl', documents.short_ccl)
        self.tagset = corpus2.get_named_tagset('nkjp')

        reader = corpus2mwe.CclMWEReader(path, self.tagset)
        reader.use_annotations(False)
        doc = reader.read()
        writer = corpus2.TokenWriter.create_path_writer('ccl:gz', path + '.gz', self.tagset)
        for chunk in doc.paragraphs():
            writer.write_chunk(chunk)
        del writer

        readerGZ  = corpus2.TokenReader.create_path_reader('ccl:gz', self.tagset, path + '.gz')
        self.assertEqual(u'Szlachetnie urodzona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'żelazna dziewica', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'napchała się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'lanymi kluskami', readerGZ.get_next_token().orth_utf8().decode('utf8'))

    def test_multiple_sentences_path_writer(self):
        path = self._save_temp_file('multiple_sentences_path_writer.ccl', documents.many_sentences_ccl)
        self.tagset = corpus2.get_named_tagset('nkjp')

        reader = corpus2mwe.CclMWEReader(path, self.tagset)
        reader.use_annotations(False)
        doc = reader.read()
        writer = corpus2.TokenWriter.create_path_writer('ccl:gz', path + '.gz', self.tagset)
        for chunk in doc.paragraphs():
            writer.write_chunk(chunk)
        del writer

        readerGZ  = corpus2.TokenReader.create_path_reader('ccl:gz', self.tagset, path + '.gz')
        self.assertEqual(u'Szlachetnie urodzona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'żelazna dziewica', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'napchała się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'lanymi kluskami', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Świeżo upieczona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'juniorka młodsza', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'spotkała', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'płetwala karłowatego', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'i', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'razem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'z', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'nim', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'po', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'wiejsku', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'nacieszyła się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'zespołem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Rittera', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Preimplantacyjna diagnostyka', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'jest', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'luksusowym dobrem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'w', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'socjalistycznym realizmie', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))

    def test_multiple_chunks_path_writer(self):
        path = self._save_temp_file('multiple_chunks_path_writer.ccl', documents.many_chunks_ccl)
        self.tagset = corpus2.get_named_tagset('nkjp')

        reader = corpus2mwe.CclMWEReader(path, self.tagset)
        reader.use_annotations(False)
        doc = reader.read()
        writer = corpus2.TokenWriter.create_path_writer('ccl:gz', path + '.gz', self.tagset)
        for chunk in doc.paragraphs():
            writer.write_chunk(chunk)
        del writer

        readerGZ  = corpus2.TokenReader.create_path_reader('ccl:gz', self.tagset, path + '.gz')
        self.assertEqual(u'Szlachetnie urodzona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'żelazna dziewica', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'napchała się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'lanymi kluskami', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Świeżo upieczona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'juniorka młodsza', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'spotkała', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'płetwala karłowatego', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'i', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'razem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'z', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'nim', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'po', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'wiejsku', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'nacieszyła się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'zespołem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Rittera', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Najświętsza Ofiara', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'w', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Wielki Post', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'podczas', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'cichej mszy', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'została', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'odprawiona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'o', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'bladym świcie', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Preimplantacyjna diagnostyka', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'jest', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'luksusowym dobrem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'w', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'socjalistycznym realizmie', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Biały', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'proszek', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'to', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'świetny', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'sposób', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'żeby', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'wyszaleć się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'do', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'bladego świtu', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))

    # def test_write_token_path_writer(self):
    #     path = self._save_temp_file('write_token_path_writer.ccl', documents.short_ccl)
    #     self.tagset = corpus2.get_named_tagset('nkjp')

    #     reader = corpus2mwe.CclMWEReader(path, self.tagset)
    #     reader.use_annotations(False)
    #     doc = reader.read()
    #     writer = corpus2.TokenWriter.create_path_writer('ccl:gz', path + '.gz', self.tagset)
    #     for chunk in doc.paragraphs():
    #         for sent in chunk.sentences():
    #             for tok in sent.tokens():
    #                 writer.write_token(tok)
    #     del writer

    #     readerGZ  = corpus2.TokenReader.create_path_reader('ccl:gz', self.tagset, path + '.gz')
    #     self.assertEqual(u'Szlachetnie urodzona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
    #     self.assertEqual(u'żelazna dziewica', readerGZ.get_next_token().orth_utf8().decode('utf8'))
    #     self.assertEqual(u'napchała się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
    #     self.assertEqual(u'lanymi kluskami', readerGZ.get_next_token().orth_utf8().decode('utf8'))

    def test_write_sentence_path_writer(self):
        path = self._save_temp_file('write_sentence_path_writer.ccl', documents.many_sentences_ccl)
        self.tagset = corpus2.get_named_tagset('nkjp')

        reader = corpus2mwe.CclMWEReader(path, self.tagset)
        reader.use_annotations(False)
        doc = reader.read()
        writer = corpus2.TokenWriter.create_path_writer('ccl:gz', path + '.gz', self.tagset)
        for chunk in doc.paragraphs():
            for sent in chunk.sentences():
                writer.write_sentence(sent)
        del writer

        readerGZ  = corpus2.TokenReader.create_path_reader('ccl:gz', self.tagset, path + '.gz')
        self.assertEqual(u'Szlachetnie urodzona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'żelazna dziewica', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'napchała się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'lanymi kluskami', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Świeżo upieczona', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'juniorka młodsza', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'spotkała', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'płetwala karłowatego', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'i', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'razem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'z', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'nim', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'po', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'wiejsku', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u',', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'nacieszyła się', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'zespołem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Rittera', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'Preimplantacyjna diagnostyka', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'jest', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'luksusowym dobrem', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'w', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'socjalistycznym realizmie', readerGZ.get_next_token().orth_utf8().decode('utf8'))
        self.assertEqual(u'.', readerGZ.get_next_token().orth_utf8().decode('utf8'))

    def setUp(self):
        self.tagset = corpus2.get_named_tagset('nkjp')
        self.temp_dir = os.path.join(tempfile.gettempdir(), "corpus2_test");
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
        os.mkdir(self.temp_dir)

    def tearDown(self):
        shutil.rmtree(self.temp_dir)

    def _save_temp_file(self, name, content):
        file_path = os.path.join(self.temp_dir, name)
        with codecs.open(os.path.join(self.temp_dir, name), 'wb', 'utf-8') as outf:
            outf.write(content)
        return file_path


if __name__ == "__main__":
    unittest.main()
