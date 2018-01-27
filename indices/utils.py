import mimetypes
import hashlib

import requests
import magic
from lxml import etree
from lxml.builder import E

from django.conf import settings

from indices.exceptions import InvalidFileFormatError


def text_as_p(text):
    return ''.join('<p>{}</p>'.format(txt) for txt in text.split('\n') if txt)


def convert_text_to_docbook_xml(text_src):
    """Returns string with text in DocBook XML format.

    text_src could be either string or file-like object with read() method in any of supported formats.
    """

    # converter requires file name with proper extension
    # need to fake html format because used unoconv does not handle encoding in txt files
    if isinstance(text_src, str):
        text_src = text_as_p(text_src)
        file_data = ('file.html', '<html>{}</html>'.format(text_src).encode())
    else:
        text_src = text_src.read()
        file_mime = magic.from_buffer(text_src, mime=True)
        all_ext = mimetypes.guess_all_extensions(file_mime)
        if '.txt' in all_ext:
            text_src = '<html>{}</html>'.format(text_as_p(text_src.decode()))
            ext = '.html'
        else:
            ext = all_ext[0] if all_ext else None

        if ext not in settings.ACCEPTED_FILE_EXTENSIONS:
            raise InvalidFileFormatError
        file_data = ('file{}'.format(ext), text_src)

    r = requests.post(settings.CONVERTER_URL, files={'file': file_data})
    return r.text


def compose_premorph_xml(text):
    """Returns text in premorph format accepted by WCRFT2 tagger.

    This allows to preserve text structure while tagging.

    text must be valid DocBook XML string.
    """

    input_doc = etree.fromstring(text.encode())

    chunks = []
    for i, p in enumerate(input_doc.iter('para'), start=1):
        chunks.append(E.chunk(p.text or '', type="p", id="p{}".format(i)))

    output_root = E.chunkList(*chunks)

    return etree.tostring(output_root, xml_declaration=True, encoding='UTF-8')


def tag_text(premorph_text):
    """Returns text tagged by WCRFT2 tagger in ccl XML format.

    premorph_text must be valid premorph XML string.
    """
    if isinstance(premorph_text, str):
        premorph_text = premorph_text.encode()
    r = requests.post(settings.TAGGER_URL, files={'file': premorph_text})
    return r.text


def md5(text):
    """Text can be either string or file-like object with read() method."""
    hash_md5 = hashlib.md5()

    if isinstance(text, str):
        hash_md5.update(text.encode())
    else:
        for chunk in iter(lambda: text.read(4096), b""):
            hash_md5.update(chunk)
        text.seek(0)
    return hash_md5.hexdigest()
