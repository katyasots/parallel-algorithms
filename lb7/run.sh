#!/bin/bash

./run_script.sh
./run_script_seq.sh

python3 src/graphics.py
python3 src/speedup.py
