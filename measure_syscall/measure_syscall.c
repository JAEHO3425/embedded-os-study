// measure_syscall.c

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define NLOOPS 1000000
#define NTRIALS 10

long diff_in_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000L + // 초 차이구하고 나노초 변환
           (end.tv_nsec - start.tv_nsec); 
}

void measure_getpid() {
    struct timespec start, end;
    double total_us = 0.0;

    printf("=== getpid() 측정 ===\n");

    for (int t = 0; t < NTRIALS; t++) {

        clock_gettime(CLOCK_MONOTONIC, &start); // 측정 시작 시각 저장

        for (int i = 0; i < NLOOPS; i++) { // NLOOPS 만큼 반복 시작
            getpid();
        }

        clock_gettime(CLOCK_MONOTONIC, &end); // 측정 종료 시각 저장

        long elapsed_ns = diff_in_ns(start, end); // start와 end 사이 총 경과 시간

        double avg_us = (double)elapsed_ns / NLOOPS / 1000.0; //getpid 1회당 평균 us 계산

        printf("Trial %d: 평균 %.6f us\n", t + 1, avg_us);

        total_us += avg_us; // trial 별 평균 누적
    }

    printf(">>> getpid 최종 평균: %.6f us\n\n", total_us / NTRIALS);
}

void measure_read() {
    struct timespec start, end;
    double total_us = 0.0;

    printf("=== read(fd, NULL, 0) 측정 ===\n");

    int fd = open("/dev/null", O_RDONLY);

    for (int t = 0; t < NTRIALS; t++) {

        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < NLOOPS; i++) {
            read(fd, NULL, 0);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        long elapsed_ns = diff_in_ns(start, end);

        double avg_us = (double)elapsed_ns / NLOOPS / 1000.0;

        printf("Trial %d: 평균 %.6f us\n", t + 1, avg_us);

        total_us += avg_us;
    }

    printf(">>> read 최종 평균: %.6f us\n\n", total_us / NTRIALS);

    close(fd);
}

int main() {
    measure_getpid();
    measure_read();
    return 0;
}