import sys
import os

corpus_path = sys.argv[1]

filenames = os.listdir(corpus_path)

word_count = 0

print("Counting words in corpus...", end=' ')

for filename in filenames:
    file = open(corpus_path + '/' + filename)
    lines = file.readlines()
    file.close()
    for line in lines:
        word_count += len(line.split())

print("Finish!")
print("Word count: {0}".format(word_count))
print("Average word count: {0:f}".format(word_count / len(filenames)))
