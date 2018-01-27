import math

from hyphen import Hyphenator
from lxml import etree
from html_sanitizer import Sanitizer

from indices.scales import interpret_index, GENERAL_SCALE_MESSAGES, MAX_READABILITY_POINTS
from indices.utils import tag_text, convert_text_to_docbook_xml, compose_premorph_xml

h_pl = Hyphenator('pl_PL')

TOTAL_ORTH_INDICES_NUMBER = 4

FOG_IGNORED_TAGS = ['interp']
PISAREK_IGNORED_TAGS = FOG_IGNORED_TAGS + ['num', 'numcol']


def proper_name(pos, word):
    """Checks whether word is a proper name based on text position and word itself.

    pos -- position in sentence starting from 0
    word -- word string
    """
    return pos and word[0:1].isupper()


class Readability:
    def __init__(self, text_src):
        self.text_src = text_src
        self.data = {}

        self.html_text = ''

        self.paragraph_count = 0
        self.sentence_count = 0
        self.long_sentence_count = 0
        self.word_count = 0
        self.hard_word_count = 0
        self.asl = 0
        self.fog_base = None
        self.fog_orth = None
        self.l_pisarek_base = None
        self.l_pisarek_orth = None
        self.nl_pisarek_base = None
        self.nl_pisarek_orth = None
        self.lix = None

    def compute_indices(self):

        xml_text = convert_text_to_docbook_xml(self.text_src)

        premorph_text = compose_premorph_xml(xml_text)

        tagged_text = tag_text(premorph_text)

        if not tagged_text:
            return {'result': 'error'}
        parsed_tagged_text = etree.fromstring(tagged_text.encode())

        paragraph_count = 0
        sentence_count = int(parsed_tagged_text.xpath('count(//sentence)'))
        word_count = 0
        fog_orth_hard_words_count = 0
        fog_base_hard_words_count = 0
        pisarek_orth_hard_words_count = 0
        pisarek_base_hard_words_count = 0
        lix_hard_words_count = 0

        hard_words = set()
        for paragraph in parsed_tagged_text.iter('chunk'):
            if len(paragraph):
                paragraph_count += 1
            for sentence in paragraph.iter('sentence'):
                for pos, el in enumerate(sentence):
                    if el.tag == 'tok':
                        orth = el.xpath('orth/text()')[0]
                        base = el.xpath('lex/base/text()')[0]
                        ctag = el.xpath('lex/ctag/text()')[0].split(":")[0]
                        if ctag not in FOG_IGNORED_TAGS:
                            word_count += 1
                            if len(h_pl.syllables(orth)) >= 4:
                                fog_orth_hard_words_count += 1
                                hard_words.add(orth)
                            if len(h_pl.syllables(base)) >= 4:
                                fog_base_hard_words_count += 1

                        if ctag not in PISAREK_IGNORED_TAGS and not proper_name(pos, orth):
                            if len(h_pl.syllables(orth)) >= 4:
                                pisarek_orth_hard_words_count += 1
                                hard_words.add(orth)
                            if len(h_pl.syllables(base)) >= 4:
                                pisarek_base_hard_words_count += 1

                        if len(orth) > 8:
                            lix_hard_words_count += 1
                            hard_words.add(orth)

        if not word_count:
            return {'result': 'error'}

        asl = word_count / sentence_count
        fog_phw_orth = 100 * (fog_orth_hard_words_count / word_count)
        fog_phw_base = 100 * (fog_base_hard_words_count / word_count)
        pisarek_phw_orth = 100 * (pisarek_orth_hard_words_count / word_count)
        pisarek_phw_base = 100 * (pisarek_base_hard_words_count / word_count)

        self.paragraph_count = paragraph_count
        self.sentence_count = sentence_count
        self.word_count = word_count
        self.hard_word_count = len(hard_words)
        self.asl = asl
        self.fog_base = round(0.4 * (asl + fog_phw_base), 2)
        self.fog_orth = round(0.4 * (asl + fog_phw_orth), 2)
        self.l_pisarek_base = round(1/3 * asl + 1/3 * pisarek_phw_base + 1, 2)
        self.l_pisarek_orth = round(1/3 * asl + 1/3 * pisarek_phw_orth + 1, 2)
        self.nl_pisarek_base = round(math.sqrt(asl**2 + pisarek_phw_base**2) / 2, 2)
        self.nl_pisarek_orth = round(math.sqrt(asl**2 + pisarek_phw_orth**2) / 2, 2)
        self.lix = round(asl + 100 * lix_hard_words_count / word_count, 2)

        self.html_text = self._compose_html(parsed_tagged_text, hard_words)

        self._set_data()
        self._interpret()
        return self.data

    def _compose_html(self, tagged_text, hard_words):

        sanitizer = Sanitizer({
            'tags': {'p', 'mark', 'span'},
            'attributes': {
                'span': ('class',)
            },
            'empty': set(),
            'separate': {'p', 'mark', 'span'},
        })

        html_paragraphs = []
        for paragraph in tagged_text.iter('chunk'):
            sentences = []
            for sentence in paragraph.iter('sentence'):
                no_space = False
                s = ''
                for pos, el in enumerate(sentence):
                    if el.tag == 'ns':
                        no_space = True
                        continue
                    elif el.tag == 'tok':

                        orth = el.xpath('orth/text()')[0]
                        base = el.xpath('lex/base/text()')[0]

                        if orth in hard_words or base in hard_words:
                            orth = '<mark>{}</mark>'.format(orth)

                        if no_space:
                            s += orth
                            no_space = False
                        else:
                            s += ' ' + orth

                sentence_length = sentence.xpath('count(./tok)')
                if sentence_length > 20:
                    self.long_sentence_count += 1
                    s = '<span class="long_sentence">{}</span>'.format(s)

                sentences.append(s)

            html_paragraphs.append('<p>{}</p>'.format(' '.join(sentences)))

        html_text = ''.join(html_paragraphs)
        return sanitizer.sanitize(html_text)

    def _set_data(self):
        self.data = {
            'paragraph_count': self.paragraph_count,
            'sentence_count': self.sentence_count,
            'long_sentence_count': self.long_sentence_count,
            'word_count': self.word_count,
            'average_sentence_length': math.ceil(self.asl),
            'hard_word_count': self.hard_word_count,
            'hard_word_percentage': round((self.hard_word_count / self.word_count) * 100, 2),
            'long_sentence_percentage': round((self.long_sentence_count / self.sentence_count) * 100, 2),
            'fog': {
                'base': {
                    'value': self.fog_base,
                },
                'orth': {
                    'value': self.fog_orth,
                },
            },
            'pisarek': {
                'linear': {
                    'base': {
                        'value': self.l_pisarek_base,
                    },
                    'orth': {
                        'value': self.l_pisarek_orth,
                    }
                },
                'nonlinear': {
                    'base': {
                        'value': self.nl_pisarek_base,
                    },
                    'orth': {
                        'value': self.nl_pisarek_orth,
                    }
                }
            },
            'lix': {
                'value': self.lix,
                'interpretation': interpret_index(self.lix, 'lix')
            }
        }

    def _interpret(self):
        general_grade_sum = 0

        interpretation, grade = interpret_index(self.fog_base, 'fog')
        self.data['fog']['base']['interpretation'] = interpretation

        interpretation, grade = interpret_index(self.fog_orth, 'fog')
        self.data['fog']['orth']['interpretation'] = interpretation
        general_grade_sum += grade

        interpretation, grade = interpret_index(self.l_pisarek_base, 'pisarek')
        self.data['pisarek']['linear']['base']['interpretation'] = interpretation

        interpretation, grade = interpret_index(self.l_pisarek_orth, 'pisarek')
        self.data['pisarek']['linear']['orth']['interpretation'] = interpretation
        general_grade_sum += grade

        interpretation, grade = interpret_index(self.nl_pisarek_base, 'pisarek')
        self.data['pisarek']['nonlinear']['base']['interpretation'] = interpretation

        interpretation, grade = interpret_index(self.nl_pisarek_orth, 'pisarek')
        self.data['pisarek']['nonlinear']['orth']['interpretation'] = interpretation
        general_grade_sum += grade

        interpretation, grade = interpret_index(self.lix, 'lix')
        self.data['lix']['interpretation'] = interpretation
        general_grade_sum += grade

        general_readability_grade = round((general_grade_sum / TOTAL_ORTH_INDICES_NUMBER) * len(GENERAL_SCALE_MESSAGES))
        self.data['readability_score'] = "{}/{}".format(general_readability_grade + 1, len(GENERAL_SCALE_MESSAGES))
        general_interpretation_msg = GENERAL_SCALE_MESSAGES[general_readability_grade]
        self.data['interpretation'] = general_interpretation_msg

        orth_points = self.fog_orth + self.l_pisarek_orth + self.nl_pisarek_orth + self.lix
        self.data['readability_points'] = math.ceil(MAX_READABILITY_POINTS - orth_points)
        # self.data['max_readability_points'] = MAX_READABILITY_POINTS
