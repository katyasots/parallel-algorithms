#include <mpi.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#define TAG 0

typedef struct
{
    int source;
    int destination;
    int data;
} Packet;

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        printf("This program requires at least 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    srand(time(0) + rank); // Инициализация генератора случайных чисел

    if (rank == 0)
    {
        std::map<int, Packet> packets_map;

        // Получаем пакеты от всех процессов
        for (int i = 1; i < size; i++)
        {
            Packet buf;
            MPI_Recv(&buf, sizeof(Packet), MPI_BYTE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            packets_map[buf.source] = buf;
        }

        // Переадресовываем пакеты процессам-адресатам
        for (int i = 1; i < size; i++)
        {
            MPI_Send(&packets_map[i], sizeof(Packet), MPI_BYTE, packets_map[i].destination, TAG, MPI_COMM_WORLD);
        }

        // Получаем подтверждения от адресатов
        for (int i = 1; i < size; i++)
        {
            bool delivery_confirmation;
            MPI_Recv(&delivery_confirmation, sizeof(bool), MPI_BYTE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        sleep(2);

        // Информируем источники об успешной доставке
        for (int i = 1; i < size; i++)
        {
            bool delivery_confirmation = true;
            MPI_Send(&delivery_confirmation, sizeof(bool), MPI_BYTE, packets_map[i].source, TAG, MPI_COMM_WORLD);
        }
    }
    else
    {
        // Генерация пакета с случайным адресатом (кроме самого себя и 0)
        Packet packet;
        packet.source = rank;
        do
        {
            packet.destination = rand() % size;
        } while (packet.destination == rank || packet.destination == 0);
        packet.data = rank;

        // Отправляем пакет процессу 0
        MPI_Send(&packet, sizeof(Packet), MPI_BYTE, 0, TAG, MPI_COMM_WORLD);
        std::cout << "Process " << rank << " sent a packet to process " << packet.destination << std::endl;

        // Ожидание получения переадресованного пакета
        std::map<int, Packet> packets_map;
        MPI_Status status;
        int attempts = 0;

        while (attempts < 100000) // Необходимо, чтобы избежать случая, когда процесс не дождался посылки, хотя она была отправлена
        {
            int flag = 0;
            MPI_Iprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if (flag)
            {
                Packet received_packet;
                MPI_Recv(&received_packet, sizeof(Packet), MPI_BYTE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::cout << "Process " << rank << " received a packet from process " << received_packet.source << std::endl;
                packets_map[received_packet.source] = received_packet;
            }
            else
            {

                attempts++;
            }
        }

        for (int i = 0; i < packets_map.size(); i++)
        {
            bool delivery_confirmation = true;
            MPI_Send(&delivery_confirmation, sizeof(bool), MPI_BYTE, 0, TAG, MPI_COMM_WORLD);
        }

        // Получение подтверждения успешной доставки для исходного пакета
        bool success;
        MPI_Recv(&success, sizeof(bool), MPI_BYTE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (success)
        {
            std::cout << "Process " << rank << " received delivery confirmation for its packet." << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
