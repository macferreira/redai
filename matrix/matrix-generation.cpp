// CreateMat.c : Create a random double square matrix and stores it in a file.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    int k, dim, size, chunk, chunk_count, extra;
    double *matrix;
    unsigned char *aux;

    if (argc != 3)
    {
        printf("Usage: %s <filename> <dim>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[2]);
    if (dim < 0 || dim > 10000)
    {
        printf("Invalid dim (%d). Use a value between 1 and 10000\n", dim);
        return 1;
    }
    size = dim * dim * sizeof(double);
    matrix = (double *)malloc(size);
    if (matrix == NULL)
    {
        printf("Insufficient memory!\n");
        return 1;
    }
    if ((fp = fopen(argv[1], "wb")) == NULL)
    {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }

    /* Generate the square matrix with random elements between 0.0 and 1.0 */
    srand((unsigned int)time(NULL)); // Initialize random number generator
    for (k = 0; k < dim * dim; k++)
        matrix[k] = (double)rand() / RAND_MAX;

    /* Write the dimension (square matrix) */
    fwrite(&dim, sizeof(int), 1, fp);

    /* write the bytes to the file */
    chunk = 10 * 1024 * 1024;
    aux = (unsigned char *)matrix;
    chunk_count = (int)(size / chunk);
    extra = size % chunk;
    for (k = 0; k < chunk_count; k++)
    {
        fwrite(aux, chunk, 1, fp);
        aux += chunk;
    }
    if (extra != 0)
        fwrite(aux, extra, 1, fp);
    fclose(fp);

    return 0;
}
