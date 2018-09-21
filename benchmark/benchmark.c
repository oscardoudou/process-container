#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pcontainer.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

int devfd;
pthread_mutex_t mutex;
int total = 0;

/**
 * Thread body that creates task in a specified container, does some simple calculations
 * and deletes the task in that container.
 */
void *thread_body(void *x)
{
    int processed = 0;
    int i;
    double sum;
    int cid = *((int *)x);

    // allocate/associate a container for the thread.
    pcontainer_create(devfd, cid);

    while (total < 50000000)
    {
        // calculate some dumb numbers here.
        for (i = 0; i < 1000000; i++)
        {
            sum += 1.0 / (1.2 + i);
            processed++;
        }

        // update the total counter.
        pthread_mutex_lock(&mutex);
        total += 1000000;
        pthread_mutex_unlock(&mutex);
    }
    // The sum of each container should be close.
    fprintf(stderr, "TID: %d, Container: %d, Processed: %d\n", (int)syscall(SYS_gettid), cid, processed);

    // Delete a container.
    pcontainer_delete(devfd, cid);
    return NULL;
}

/**
 * main function to create/run/delete threads in different containers 
 */
int main(int argc, char *argv[])
{
    int i, num_of_containers, tasks;
    int total_tasks = 0; 
    int *tasks_in_containers;
    int *cid;
    pthread_t *threads;

    // check num of arguments.
    if (argc < 3)
    {
        fprintf(stderr, "Not enough parameters\n");
        fprintf(stderr, "usage: ./benchmark <num_container> [<num_task_in_container> ...]\n");
        exit(1);
    }
    
    // open the kernel module
    devfd = open("/dev/pcontainer", O_RDWR);
    if (devfd < 0)
    {
        fprintf(stderr, "Device open failed\n");
        exit(1);
    }

    // parse number of containers
    num_of_containers = atoi(argv[1]);
    fprintf(stderr, "num_of_containers: %d\n", num_of_containers);

    // the number of task arguments should be at least equal to num of container
    if (argc - 2 < num_of_containers)
    {
        fprintf(stderr, "Not enough parameters\n");
        fprintf(stderr, "usage: ./benchmark <num_container> [<num_task_in_container> ...]\n");
        exit(1);
    }
    
    // generate and store number of tasks and cid for each container.
    tasks_in_containers = (int *) calloc(num_of_containers, sizeof(int));
    cid = (int *) calloc(num_of_containers, sizeof(int));

    for (i = 0; i < num_of_containers; i++)
    {
        tasks = atoi(argv[i+2]);
        fprintf(stderr, "task for container %d: %d\n", i, tasks);
        total_tasks += tasks;
        tasks_in_containers[i] = tasks;
        cid[i] = i;
    }
    
    fprintf(stderr, "num_of_total_tasks: %d\n\n", total_tasks);

    // alarm and mutex lock initialization
    pcontainer_init(devfd);
    pthread_mutex_init(&mutex, NULL);

    // create threads and assign tasks.
    threads = (pthread_t*) calloc(total_tasks, sizeof(pthread_t));

    // reset the total task for assigning the thread array.
    total_tasks = 0;

    for (i = 0; i < num_of_containers; i++)
    {
        for (tasks = 0; tasks < tasks_in_containers[i]; tasks++)
        {
            pthread_create(&threads[total_tasks], NULL, thread_body, &cid[i]);
            total_tasks++;
        }
    }

    // wait for terminations of threads
    for (i = 0; i < total_tasks; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // cleanup
    free(tasks_in_containers);
    free(threads);
    free(cid);
    return 0;
}
