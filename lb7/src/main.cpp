#include <mpi.h>
#include <iostream>
#include <cmath>
#include <vector>

void create_mpi_grid(int &dim, int &row, int &col, MPI_Comm &grid_comm, MPI_Comm &row_comm, MPI_Comm &col_comm) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    dim = static_cast<int>(std::sqrt(size));
    int dims[2] = {dim, dim};
    int periods[2] = {0, 0};
    int reorder = 1;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &grid_comm);
    MPI_Comm_rank(grid_comm, &rank);
    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);
    row = coords[0];
    col = coords[1];

    int remain_dims_row[2] = {0, 1};
    int remain_dims_col[2] = {1, 0};

    MPI_Cart_sub(grid_comm, remain_dims_row, &row_comm);
    MPI_Cart_sub(grid_comm, remain_dims_col, &col_comm);
}

void multiply_local(const std::vector<int> &a, const std::vector<int> &b, std::vector<int> &c, int block_size) {
    for (int i = 0; i < block_size; ++i) {
        for (int j = 0; j < block_size; ++j) {
            int temp = 0;
            for (int k = 0; k < block_size; ++k) {
                temp += a[i * block_size + k] * b[k * block_size + j];
            }
            c[i * block_size + j] += temp;
        }
    }
}

void fox_multiply(int n, std::vector<int> &a, std::vector<int> &b, std::vector<int> &c, int dim, int row, int col, MPI_Comm row_comm, MPI_Comm col_comm) {
    int block_size = n / dim;
    std::vector<int> temp_a(block_size * block_size, 0);

    int src = (row + 1) % dim;
    int dst = (row + dim - 1) % dim;

    for (int stage = 0; stage < dim; ++stage) {
        int root = (row + stage) % dim;

        if (root == col) {
            MPI_Bcast(a.data(), block_size * block_size, MPI_INT, root, row_comm);
            multiply_local(a, b, c, block_size);
        } else {
            MPI_Bcast(temp_a.data(), block_size * block_size, MPI_INT, root, row_comm);
            multiply_local(temp_a, b, c, block_size);
        }

        MPI_Sendrecv_replace(b.data(), block_size * block_size, MPI_INT, dst, 0, src, 0, col_comm, MPI_STATUS_IGNORE);
    }
}

void read_matrix(std::vector<int> &matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cin >> matrix[i * size + j];
        }
    }
}

void print_matrix(const std::vector<int> &matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cout << matrix[i * size + j] << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " --matrix-size <size>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    int matrix_size = std::stoi(argv[2]);

    if (matrix_size % static_cast<int>(std::sqrt(size)) != 0) {
        if (rank == 0) {
            std::cerr << "Error: Matrix size must be divisible by the square root of the number of processes." << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    int dim, row, col;
    MPI_Comm grid_comm, row_comm, col_comm;
    create_mpi_grid(dim, row, col, grid_comm, row_comm, col_comm);

    int block_size = matrix_size / dim;

    std::vector<int> local_a(block_size * block_size, 0);
    std::vector<int> local_b(block_size * block_size, 0);
    std::vector<int> local_c(block_size * block_size, 0);

    std::vector<int> a, b;
    if (rank == 0) {
        a.resize(matrix_size * matrix_size);
        b.resize(matrix_size * matrix_size);

        std::cout << "Enter matrix A (" << matrix_size << "x" << matrix_size << "):" << std::endl;
        read_matrix(a, matrix_size);

        std::cout << "Enter matrix B (" << matrix_size << "x" << matrix_size << "):" << std::endl;
        read_matrix(b, matrix_size);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(a.data(), block_size * block_size, MPI_INT, local_a.data(), block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b.data(), block_size * block_size, MPI_INT, local_b.data(), block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);

    fox_multiply(matrix_size, local_a, local_b, local_c, dim, row, col, row_comm, col_comm);

    std::vector<int> result;
    if (rank == 0) {
        result.resize(matrix_size * matrix_size, 0);
    }

    MPI_Gather(local_c.data(), block_size * block_size, MPI_INT, result.data(), block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Result matrix C (" << matrix_size << "x" << matrix_size << "):" << std::endl;
        print_matrix(result, matrix_size);
    }

    MPI_Finalize();
    return 0;
}