#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{


    MPI_Init(&argc, &argv);
    int procRank, procSize;

    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procSize);

    int dataSize = 1;
    if (argc > 1)
    {
        dataSize = atoi(argv[1]);
    }

    int *data = (int *)malloc(dataSize * sizeof(int));

    double start, end, time;

    if (procRank == 0)
    {
        start = MPI_Wtime();

        for (int i = 1; i < procSize; i++)
        {
            MPI_Recv(data, dataSize, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        end = MPI_Wtime();
        time = end - start;
        printf("Data: %d, Time: %.8f\n", dataSize, time);
    }
    else
    {
        for (int i = 0; i < dataSize; i++)
        {
            data[i] = procRank;
        }

        MPI_Send(data, dataSize, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    free(data);
    MPI_Finalize();
    return 0;
}

