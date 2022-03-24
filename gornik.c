// THIS SIMPLE PROGRAM SHOWS THE COMPLEXITY AND HOW TIME CONSUMING IS FIND
// CORRECT BITCOIN BLOCK ACCORDING TO GIVEN DIFFICULTY LEVEL (DESIRED NUMBER OF ZEROS IN SHA256 RESULT)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <limits.h>

#define BLOCK_SIZE 215

void *findBlock(void *ptr);

unsigned char zeros[32] = {0};
int zeroNum = 0;
int thSize;
char mainString[BLOCK_SIZE] = {0};

int main(int argc, char **argv)
{
    srand(time(NULL));
    int i = 0;
    int *nums;
    pthread_t *th;

    if (argc == 1)
    {
        puts("Too few arguments!");
        printf("usage: %s [number_of_threads] [number_of_zeros]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc > 5)
    {
        perror("Too many arguments");
        return EXIT_FAILURE;
    }

    for (i = 0; i < sizeof(mainString); i++)
    {
        mainString[i] = rand(); // fill begin of the block with random data
    }

    zeroNum = atoi(argv[2]); // convert the initial start parameters (number of zeros)
    thSize = atoi(argv[1]);  // convert the initial start parameters (number of threads)

    th = malloc(sizeof *th * (thSize + 1));
    if (!th)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    nums = malloc(sizeof *nums * (thSize + 1));
    printf("Threads number: %d\n", thSize);
    printf("Number of zeros to find: %s\n", argv[2]);

    for (i = 0; i < thSize; i++)
    {
        nums[i] = i;
        pthread_create(&th[i], NULL, findBlock, (void *)&nums[i]);
    }

    for (i = 0; i < thSize; i++)
    {
        pthread_join(th[i], NULL);
    }

    return 0;
}

void *findBlock(void *ptr)
{
    char string[BLOCK_SIZE] = {0};
    int t1, t2;
    unsigned long oldnum;
    unsigned long *thNum = ptr;
    unsigned long number = *thNum, number2;

    unsigned char digest[SHA256_DIGEST_LENGTH];

    char mdString[SHA256_DIGEST_LENGTH * 2 + 1];
    memcpy(string, mainString, sizeof(string));
    int i = 0;

    t1 = time(NULL);

    for (number = *thNum; number < ULONG_MAX; number = number + thSize)
    {
        memcpy(string + 180, &number, sizeof(number));
        SHA256((unsigned char *)&string, sizeof(string), (unsigned char *)&digest);

        if (memcmp(digest, zeros, zeroNum) == 0)
        {
            for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
                sprintf(&mdString[i * 3], "%02x ", (unsigned int)digest[i]); // print the found blocks sha256 digest with zeros on the begin

            printf("SHA256 digest[%lu]: %s\n", number, mdString);
        }
    }
    printf("bye!\n");
    return 0;
}
