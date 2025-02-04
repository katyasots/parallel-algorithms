#include <mpi.h>
#include <iostream>
#include <vector>

struct Message
{
    int rank;
    float message;
};

float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int world_rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    srand(time(NULL) + world_rank);
    Message message = {world_rank, randomFloat()};

    MPI_Datatype MPI_Message;
    MPI_Datatype types[2] = {MPI_INT, MPI_FLOAT};
    int block_lengths[2] = {1, 1};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(Message, rank);
    offsets[1] = offsetof(Message, message);

    MPI_Type_create_struct(2, block_lengths, offsets, types, &MPI_Message);
    MPI_Type_commit(&MPI_Message);

    std::vector<Message> messages;
    if (world_rank == 0)
    {
        messages.resize(world_size);
    }

    MPI_Gather(&message, 1, MPI_Message, messages.data(), 1, MPI_Message, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        std::cout << "List of numbers:\n";
        for (int i = 0; i < world_size; i++)
        {
            std::cout << "Message: " << messages[i].message << " from rank: " << messages[i].rank << std::endl;
        }
    }

    MPI_Type_free(&MPI_Message);
    MPI_Finalize();
    return 0;
}