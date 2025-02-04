#!/bin/bash

output_file="data/data.txt"

> $output_file

mpic++ -o bin/time src/time.cpp -std=c++11

for size in 10 50 100 200 500 1000
do
    for n in 4 25
    do
        echo "Running with matrix size $size and $n processes"
        mpiexec -n $n --oversubscribe bin/time --matrix-size $size --output-file $output_file
    done
done

for size in 12 51 102 201 501 1002
do
    for n in 9
    do
        echo "Running with matrix size $size and $n processes"
        mpiexec -n $n --oversubscribe bin/time --matrix-size $size --output-file $output_file
    done
done

for size in 12 52 100 200 500 1000
do
    echo "Running with matrix size $size and 16 processes"
    mpiexec -n 16 --oversubscribe bin/time --matrix-size $size --output-file $output_file
done

for size in 12 48 102 198 498 1002
do
    echo "Running with matrix size $size and 36 processes"
    mpiexec -n 36 --oversubscribe bin/time --matrix-size $size --output-file $output_file
done

for size in 14 49 105 203 504 1001
do
    echo "Running with matrix size $size and 49 processes"
    mpiexec -n 49 --oversubscribe bin/time --matrix-size $size --output-file $output_file
done

for size in 8 48 104 200 504 1000
do
    echo "Running with matrix size $size and 64 processes"
    mpiexec -n 64 --oversubscribe bin/time --matrix-size $size --output-file $output_file
done