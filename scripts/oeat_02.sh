#!/bin/bash

#corpus_path - $1
#frequency_count - $2
#frequency_file_path - $3

../build/labs/oeat_02/oeat_02 $1 $2 $3

python ../labs/oeat_02/distribution_plot.py $3
