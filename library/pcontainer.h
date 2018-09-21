#ifdef __cplusplus
extern "C"
{
#endif

#include <processor_container/processor_container.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

    int pcontainer_delete(int devfd, int cid);
    int pcontainer_create(int devfd, int cid);
    int pcontainer_context_switch_handler(int devfd, int cid);
    int pcontainer_init(int devfd);
    int DEVFD;

    /**
     * handler function for the timer to run the context switch function.
     */
    static void handler()
    {
        pcontainer_context_switch_handler(DEVFD, 0);
    }

    /**
     * set up the alarm for context switch every time duration.
     */
    int pcontainer_init(int devfd)
    {
        struct sigaction sa;
        struct itimerval timeout;

        sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_ONSTACK;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = handler;
        if (sigaction(SIGPROF, &sa, NULL) == -1)
        {
            fprintf(stderr, "sigaction");
            exit(1);
        }

        DEVFD = devfd;
        timeout.it_value.tv_sec = 0;
        timeout.it_value.tv_usec = 5;
        timeout.it_interval = timeout.it_value;
        if (setitimer(ITIMER_PROF, &timeout, NULL) == 1)
            fprintf(stderr, "Timer failed\n");
        return 0;
    }

#ifdef __cplusplus
}
#endif