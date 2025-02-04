#!/bin/bash
> data.txt

mpic++ -o time time.cpp

sizes=(10 100 1000 10000 50000)

for size in "${sizes[@]}"
do
    for nprocs in {1..16}
    do
        mpiexec -n $nprocs ./time $size
    done
done

python3 graphics.py