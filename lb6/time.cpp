#include <iostream>
#include <mpi.h>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    std::ofstream fout("./data.txt", std::ios::app);

    int data_size;
    if (argc > 1)
        data_size = atoi(argv[1]);

    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // программа не работает на одном процессе
    if (size < 2) {
        if (rank == 0) {
            std::cerr << "Для работы программы необходимо минимум два процесса!" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }
    
    double start, end, time;
    start = MPI_Wtime();

    // размерность декартовой топологии
    int dims = 1; // одномерная решетка
    int periods[] = {1}; // периодичность по осям (1 означает, что решетка циклична)
    int reorder = 1; // возможность изменения порядка процессов

    // создание декартовой топологии
    MPI_Comm cart_comm;
    int coords[1]; // координаты процесса в декартовой решетке
    MPI_Cart_create(MPI_COMM_WORLD, dims, &size, periods, reorder, &cart_comm);
    
    // получаем координаты текущего процесса в декартовой топологии
    MPI_Cart_coords(cart_comm, rank, 1, coords);
    
    // Инициализация данных
    std::vector<double> data(data_size, rank * 1.0);
    std::vector<double> received_data(data_size);
    
    // определяем соседей для пересылки
    int source, dest;
    MPI_Cart_shift(cart_comm, 0, -1, &source, &dest); // сдвиг с шагом -1 (отправка в предыдущий процесс)
    
    // отправка и принятие данных
    MPI_Status status;
    
    if (rank == 0) { //процесс 0
        // отправляет данные последнему процессу (size-1)
        MPI_Send(data.data(), data_size, MPI_DOUBLE, dest, 0, cart_comm);
        // принимает данные от процесса 1
        MPI_Recv(received_data.data(), data_size, MPI_DOUBLE, (rank + 1) % size, 0, cart_comm, &status);
    } else { // Остальные процессы
        // аринимают данные от следующего
        MPI_Recv(received_data.data(), data_size, MPI_DOUBLE, (rank + 1) % size, 0, cart_comm, &status);
        // отправляют данные предыдущему
        MPI_Send(data.data(), data_size, MPI_DOUBLE, dest, 0, cart_comm);
    }
    
    // вывод полученных данных
    //std::cout << "Процесс " << rank << " (данные = " << data << ") получил данные: " << received_data << std::endl;
    end = MPI_Wtime();
    time = end - start;
    
    if (rank == 0) {
        fout << "Data size: " << data_size << " Time: " << time << " Processes: " << size << '\n';
    }
    fout.close();
    MPI_Finalize();
    
    return 0;
}