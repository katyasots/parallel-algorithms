#!/bin/bash

mpicc -o time with_time.c

for num_processes in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do
    echo "$num_processes processes"
    
    for data_size in 1 10 100 1000 10000; do
    	echo "$data_size len"
       
        mpirun -np $num_processes ./time $data_size
        
      
    done
    
    echo ""
done

