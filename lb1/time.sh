#!/bin/bash

mpicc -o time time_and_size.c -lmpi -lm

for num_processes in 1 4 8 16; do
    echo "Running with $num_processes processes"
    
    for data_size in 1 10 100 1000; do
       
        mpirun -np $num_processes ./time $data_size
        
      
    done
    
    echo ""
done

