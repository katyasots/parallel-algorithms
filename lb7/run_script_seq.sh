#!/bin/bash

sizes=(10 50 100 200 500 1000)
nprocs=(1)
output_file="data/seq.txt"

> $output_file

mpic++ -o bin/time src/time.cpp -std=c++11

for size in "${sizes[@]}"
do
        echo "Running with matrix size $size and 1 processes"
        mpiexec -n 1 --oversubscribe bin/time --matrix-size $size --output-file $output_file
done