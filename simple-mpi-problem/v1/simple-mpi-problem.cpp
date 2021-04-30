#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int np, id, dest;
    char message[100];
    MPI_Status st;
    double ts;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if (id == 0) // process 0 start measuring time
        ts = -MPI_Wtime();

    sprintf(message, "Message from process nr. %d", id); // building a string message
    dest = (id + 1) % np;
    MPI_Send(message, 100, MPI_CHAR, dest, 0, MPI_COMM_WORLD); // every process sends the message (before receiving)
    MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
    printf("Process %d received the messsage: %s\n", id, message);

    if (id == 0)
    {
        ts += MPI_Wtime(); // process 0 measures time
        printf("Time: %f s\n", ts);
    }
    fflush(stdout);
    MPI_Finalize();
    return 0;
}