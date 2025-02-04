#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include <fstream>
#include <iostream>


void sendDataToBuffer(int *array, size_t size, int pipe_fd[2])
{
    // Закрываем ненужную сторону pipe для чтения
    close(pipe_fd[0]);

    // Отправляем данные в pipe
    write(pipe_fd[1], array, sizeof(int) * size);

    // Закрываем сторону pipe для записи после отправки данных
    close(pipe_fd[1]);
}

// Функция для чтения данных из буфера
void readDataFromBuffer(int *buffer, size_t size, int pipe_fd[2])
{
    // Закрываем ненужную сторону pipe для записи
    close(pipe_fd[1]);

    // Чтение данных из pipe
    read(pipe_fd[0], buffer, sizeof(int) * size);

    // Закрываем сторону pipe для чтения после получения данных
    close(pipe_fd[0]);
}

int main(int argc, char **argv)
{
    std::ofstream fout("data_seq.txt", std::ios::app);
    int pipe_fd[2]; // Массив для хранения файловых дескрипторов pipe


    int dataSize = 1;
    if (argc > 1)
    {
        dataSize = atoi(argv[1]);
    }

    int *data = new int[dataSize];

    int buffer[dataSize];

    auto start = std::chrono::high_resolution_clock::now();
    // Отправляем массив в буфер
    sendDataToBuffer(data, dataSize, pipe_fd);

    // Читаем данные из буфера
    readDataFromBuffer(buffer, dataSize, pipe_fd);

    auto end = std::chrono::high_resolution_clock::now();
    auto time = end - start;
    fout << time.count()  << '\n';
    fout.close();

    return 0;
}
