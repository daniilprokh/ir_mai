import sys
import wikipediaapi
from pathlib import Path

wiki_wiki = wikipediaapi.Wikipedia('TestProject')
categories = ["Category:Music"]

count = eval(sys.argv[1])
if count < 100:
    sys.exit("The number of documents is less than 100")
count_percent = count // 100
curr_count = 0

file_path = sys.argv[2]
open(file_path, "w").close()

print("Getting names from wikipedia:")
while True:
  new_categories = []
  titles = []
  for category in categories: 
    category_page = wiki_wiki.page(category)
    file = open(file_path, "a")
    for page in category_page.categorymembers.values():
      title = page.title
      if title.startswith("Category:"):
        new_categories.append(title)
      elif not (title.startswith("File:") or title.startswith("Portal:")):
        file.write(title + '\n')
        curr_count += 1
        if curr_count == count:
          file.close()
          sys.exit("Complete!")
        elif curr_count % count_percent == 0:
          percent = curr_count // count_percent
          print("Current count: {0} ({1:d}%)".format(curr_count, percent))
    file.close()
  
  if len(new_categories) == 0:
    break
  categories = new_categories

print("End count: {0} ({1:f}%)".format(curr_count, curr_count / count_percent))
