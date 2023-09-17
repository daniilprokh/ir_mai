import sys
import wikipediaapi
from pathlib import Path

wiki_wiki = wikipediaapi.Wikipedia('TestProject')

corpus_path = sys.argv[1]
Path(corpus_path).mkdir(parents=True, exist_ok=True)

document_count = 0

file_path = sys.argv[2]
file = open(file_path, "r")
titles = file.readlines()
file.close()

print("Corpus generation...", end=' ')

for title in titles:
  document_count += 1
  page = wiki_wiki.page(title[:-1])

  title_file = open(corpus_path + "/{0}.txt".format(document_count) , "w")
  title_file.write(page.text)
  title_file.close()

print("Finish!")
