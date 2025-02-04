#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>

struct Message
{
    int rank;
    std::vector<int> message;
};

int main(int argc, char **argv)
{
    std::ofstream fout("./data.txt", std::ios::app);

    MPI_Init(&argc, &argv);

    int world_rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int data_size;
    if (argc > 1)
        data_size = atoi(argv[1]);

    std::vector<int> vec(data_size, world_rank);

    std::vector<int> recv_buffer;
    if (world_rank == 0)
    {
        recv_buffer.resize(world_size * data_size);
    }

    double start_time = MPI_Wtime();

    MPI_Gather(vec.data(), data_size, MPI_INT, recv_buffer.data(), data_size, MPI_INT, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();
    double time = end_time - start_time;

    if (world_rank == 0)
    {
        fout << "Data size: " << data_size << " Time: " << time << " Processes: " << world_size << '\n';
        fout.close();
    }

    MPI_Finalize();
    return 0;
}