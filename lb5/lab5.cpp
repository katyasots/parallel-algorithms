#include <iostream>
#include <mpi.h>
#include <vector>
#include <random>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //новый коммуникатор для процессов четного ранга
    int color = (rank % 2 == 0) ? 0 : MPI_UNDEFINED;
    MPI_Comm even_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &even_comm);

    if (color == 0) {
        //в коммуникаторе
        int even_rank;
        MPI_Comm_rank(even_comm, &even_rank);

        //генерация случайных данных
        std::random_device rd;
        std::mt19937 gen(rd() + rank); //уникальный seed для каждого процесса
        std::uniform_real_distribution<> dis(0.0, 100.0);

        std::vector<double> local_data(3);
        for (auto& val : local_data) {
            val = dis(gen);
        }

        //вывод данных каждого процесса
        std::cout << "Process " << rank << " data: ";
        for (const auto& val : local_data) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        std::vector<double> global_min(3);

        //редукция для нахождения мин значений
        MPI_Reduce(local_data.data(), global_min.data(), 3, MPI_DOUBLE, MPI_MIN, 0, even_comm);

        if (even_rank == 0) {
            std::cout << "Global minimum values: ";
            for (const auto& val : global_min) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        MPI_Comm_free(&even_comm);
    }

    MPI_Finalize();
    return 0;
}