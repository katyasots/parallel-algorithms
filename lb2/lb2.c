#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void generate_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 3; // Генерация случайных чисел
    }
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int array_size = 10; // Размер массива по умолчанию
    if (argc > 1) {
        array_size = atoi(argv[1]); // Считывание размера массива
    }

    int *array = (int *)malloc(array_size * sizeof(int)); // Выделение памяти для массива во всех процессах
    int local_count = 0;
    int *results = NULL;

    if (rank == 0) {
        // Процесс 0 генерирует массив
        generate_array(array, array_size);
        // Вывод сгенерированного массива
        printf("Generated array: ");
        for (int i = 0; i < array_size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        // Рассылка частей массива другим процессам
        int local_size = array_size / size;
        int remainder = array_size % size;
        for (int i = 1; i < size; i++) {
            int start_index = i * local_size + (i < remainder ? i : remainder);
            int end_index = start_index + local_size + (i < remainder ? 1 : 0);
            MPI_Send(array + start_index, end_index - start_index, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Обработка своей части массива
        int start_index = 0;
        int end_index = local_size + (0 < remainder ? 1 : 0);
        printf("Process 0: ");
        for (int i = start_index; i < end_index; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        for (int i = start_index; i < end_index; i++) {
            if (array[i] == 0) {
                local_count++;
            }
        }
    } else {
        // Другие процессы получают свои части массива
        int local_size = array_size / size;
        int remainder = array_size % size;
        int start_index = rank * local_size + (rank < remainder ? rank : remainder);
        int end_index = start_index + local_size + (rank < remainder ? 1 : 0);
        MPI_Recv(array + start_index, end_index - start_index, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Вывод элементов, которые получил текущий процесс
        printf("Process %d: ", rank);
        for (int i = start_index; i < end_index; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        // Поиск нулевых элементов в локальной части массива
        for (int i = start_index; i < end_index; i++) {
            if (array[i] == 0) {
                local_count++;
            }
        }
    }

    // Сбор результатов с помощью MPI_Send и MPI_Recv
    if (rank == 0) {
        results = (int *)malloc(size * sizeof(int));
        results[0] = local_count;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&results[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Вывод результатов
        int total_count = 0;
        for (int i = 0; i < size; i++) {
            total_count += results[i];
        }
        printf("Total number of zero elements: %d\n", total_count);

        free(results);
    } else {
        // Другие процессы отправляют результаты процессу 0
        MPI_Send(&local_count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    free(array); // Освобождение памяти для массива во всех процессах
    MPI_Finalize();
    return 0;
}
