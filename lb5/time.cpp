#include <iostream>
#include <mpi.h>
#include <vector>
#include <random>
#include <fstream>

int main(int argc, char** argv) {
    
    std::ofstream fout("./data.txt", std::ios::app);

    int data_size;
    if (argc > 1)
        data_size = atoi(argv[1]);

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //новый коммуникатор для процессов четного ранга
    int color = (rank % 2 == 0) ? 0 : MPI_UNDEFINED;

    double start, end, time;
    start = MPI_Wtime();

    MPI_Comm even_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &even_comm);

    if (color == 0) {
        //в коммуникаторе
        int even_rank;
        MPI_Comm_rank(even_comm, &even_rank);

        std::vector<float> local_data(data_size, 0.0), global_min(data_size, 0.0);

        //редукция для нахождения мин значений
        MPI_Reduce(local_data.data(), global_min.data(), data_size, MPI_FLOAT, MPI_MIN, 0, even_comm);

        if (even_rank == 0) {
            end = MPI_Wtime();
            time = end - start;
            fout << "Data size: " << data_size << " Time: " << time << " Processes: " << size << '\n';
            fout.close();
        }
        MPI_Comm_free(&even_comm);
    }

    MPI_Finalize();
    return 0;
}
