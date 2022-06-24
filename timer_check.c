#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

struct timespec diff_timespec(const struct timespec *time1,
                     const struct timespec *time0) {

    struct timespec diff = {.tv_sec = time1->tv_sec - time0->tv_sec, .tv_nsec =
                            time1->tv_nsec - time0->tv_nsec};
    if (diff.tv_nsec < 0) {
        diff.tv_nsec += 1000000000;
        diff.tv_sec--;
    }
    return diff;
}

void print_clock(const struct timespec *abs_time, const struct timespec *diff){
    printf("    absolute time   = %10ld.%09ld\n", abs_time->tv_sec, abs_time->tv_nsec);
    printf("    increased time  = %10ld.%09ld\n", diff->tv_sec, diff->tv_nsec);
    return;
}

void check_time(unsigned int stall_time) {
    struct timespec realtime_ts_curr;
    struct timespec realtime_ts_prev;
    struct timespec monotonic_ts_curr;
    struct timespec monotonic_ts_prev;
    struct timespec proc_cputime_ts_curr;
    struct timespec proc_cputime_ts_prev;
    int counter = 0;

    for (;;)
    {
        // get current time
        clock_gettime(CLOCK_REALTIME, &realtime_ts_curr);
        clock_gettime(CLOCK_MONOTONIC, &monotonic_ts_curr);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &proc_cputime_ts_curr);

        // skip first step.
        if (counter == 0)
        {
        } else {
            // diff previous time.
            struct timespec realtime_ts_diff = diff_timespec(&realtime_ts_curr,
                                                            &realtime_ts_prev);
            struct timespec monotonic_ts_diff = diff_timespec(&monotonic_ts_curr,
                                                            &monotonic_ts_prev);
            struct timespec proc_cputime_ts_diff = diff_timespec(&proc_cputime_ts_curr,
                                                            &proc_cputime_ts_prev);
            printf("------ %d iteration ----", counter);
            printf("* realtime clock\n");
            print_clock(&realtime_ts_curr, &realtime_ts_diff);
            printf("* monotonic clock\n");
            print_clock(&monotonic_ts_curr, &monotonic_ts_diff);
            printf("* proc cpu time\n");
            print_clock(&proc_cputime_ts_curr, &proc_cputime_ts_diff);
        }
        // print time data.
        memcpy(&realtime_ts_prev, &realtime_ts_curr, sizeof(realtime_ts_curr));
        memcpy(&monotonic_ts_prev, &monotonic_ts_curr, sizeof(monotonic_ts_curr));
        memcpy(&proc_cputime_ts_prev, &proc_cputime_ts_curr, sizeof(proc_cputime_ts_curr));
        counter += 1;
        sleep(stall_time);
    }
}

int main(int argc, char** argv) {
    int stall_time = 0;
    if (argc > 1) {
        stall_time = atoi(argv[1]);
    }
    else
    {
        stall_time = 1;
    }

    printf("this program iterates per %d seconds\n", stall_time);
    printf("please send SIGINT if you want to terminate.\n");

    check_time((unsigned int)stall_time);

    return 0;
}