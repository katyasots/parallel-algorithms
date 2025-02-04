#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int ProcRank;
    int RecvRank;
    int ProcSize;

    // Инициализация MPI
    MPI_Init(&argc, &argv);

    // Получение ранга текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcSize);

    // Если это не процесс с рангом 0, отправляем свое значение ранга процессу с рангом 0
    if (ProcRank != 0) {
        MPI_Send(&ProcRank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Процесс с рангом 0 выводит свое значение ранга
    if (ProcRank == 0) {
        printf("Hello from process %3d\n", ProcRank);
        
        // Принимаем сообщения от других процессов
        for(int i = 1; i < ProcSize; i++){
        	MPI_Recv(&RecvRank, 1, MPI_INT, i, MPI_ANY_TAG,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        	printf("Hello from process %3d\n", RecvRank);
        }
    }

    // Завершение MPI программы
    MPI_Finalize();
    return 0;
}

