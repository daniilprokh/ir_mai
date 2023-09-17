#!/bin/bash

#title_list_file_path - $1
#title_count - $2
#corpus_path - $3

python ../labs/ir_01/titles.py $2 $1
python ../labs/ir_01/generate_corpus.py $3 $1
python ../labs/ir_01/word_count.py $3
