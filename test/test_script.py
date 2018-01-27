import os
import sys

import requests


def process_files(path):
    url = 'http://localhost:8000/api/analyses/'
    files = os.listdir(path)
    total_files = len(files)
    total_files_processed = 0

    total_readability_score = 0
    total_hard_words_percentage = 0
    total_long_sentence_percentage = 0

    for i, f in enumerate(files, start=1):
        with open(os.path.join(path, f), 'rb') as data_file:
            print("Trwa analiza pliku {}/{}: {}".format(i, total_files, f))
            r = requests.post(url, files={'file': data_file})
            if r.status_code == 201:
                data = r.json()
                stats = data['stats']
                total_readability_score += int(stats['readability_score'].split('/')[0])
                total_hard_words_percentage += stats['hard_word_percentage']
                total_long_sentence_percentage += stats['long_sentence_percentage']
                total_files_processed += 1
            else:
                print("Błąd przy analize pliku {}".format(f))

    avg_readability_score = total_readability_score / total_files
    avg_hard_words_percentage = total_hard_words_percentage / total_files
    avg_long_sentence_percentage = total_long_sentence_percentage / total_files

    print("Przeanalizowano {}/{} plików".format(total_files_processed , total_files))
    print("Śr. trudność: {}".format(round(avg_readability_score, 2)))
    print("Śr. % trudnych słów: {}".format(round(avg_hard_words_percentage)))
    print("Śr. % długich zdań: {}".format(round(avg_long_sentence_percentage)))

if __name__ == '__main__':
    if len(sys.argv) > 1:
        process_files(sys.argv[1])
    else:
        print("Proszę podać ścieżkę do katalogu z plikami.")
