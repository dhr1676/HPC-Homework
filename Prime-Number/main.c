// Compile: gcc myprimenumber.c -std=c99 -pthread -lm
// Run: ./myprimenumber.c 1000 4
// Explanation:
// The first argument is the range of the prime number;
// The second argument is the thread number;
// If you want to see the print out answer, just change the value print in main.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// Find Prime number Part
typedef struct Number {
    int num;
    int ptrue; // ptrue = prime/unchecked, false = non-prime
} Number;

Number *list;

//double CLOCK() {
//    struct timespec t;
//    clock_gettime(CLOCK_MONOTONIC,  &t);
//    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
//}

int max, part_length,thread_num;

int isPrime( int num) // 判断这个数是不是质数
{
    if(num == 2 || num == 3 )
        return 1;
    if( num %6 != 1 && (num %6 != 5) )
        return 0;
    int tmp = (int)sqrt(num);
    for( int k =5; k<= tmp; k+=6) {
        if ((num % k == 0) || (num % (k + 2) == 0))
            return 0;
    }
    return 1;
}

void* sieve (void *arg)
{
    int threadnum = *(int *)arg;        // 把参数传进来 Get the argument in
    int start, end;       // 设定找质数的起点和终点 Set the start and end of the finding
    start = part_length * threadnum + 1;
//    printf("Thread %d's start is %d\n", threadnum, start);
    if(threadnum < thread_num - 1) {
        end = part_length * (threadnum + 1);
        for(int i = start; i <= end; i++) {
            list[i].ptrue = isPrime(i);
        }
        printf("it's IF! Thread %d's End is %d \n", threadnum, end);
    }       // 不是最后一个分块 If this thread doesn't control the last part
    else {
        end = max;
        for(int i = (part_length * threadnum + 1); i <= end; i++)
            list[i].ptrue = isPrime(i);
        printf("it's ELSE! Thread %d's End is %d \n", threadnum, max);
    }       //最后一个分块要多检测几个数字
//     If this thread controls to test the last part, it needs to test a few more numbers
    pthread_exit(NULL);
}

int main(int argc, char **argv) {

    if(argc < 3){
        printf("Input format is <prime number max> <threads number>\n");
        return 0;
    }

    // Timer part
    struct timeval startTime,endTime;
    double Timeuse;
    gettimeofday(&startTime,NULL);
//    double start, finish;
//    start = CLOCK();
    // Prime part
    int print = 0;
    int partnum;
    int count = 0;             // 给输出计数用的
    max = atoi(argv[1]);       // 第一个参数是要找质数的上限 The first argument is the range of the prime number;
    partnum = atoi(argv[2]);   // 第二个参数是thread的个数 The second argument is the thread number;

    list = malloc(sizeof(Number) * max);
    for(int i = 0; i < max+1; i++) {
        list[i].num = i;
        list[i].ptrue = 1;
//        printf("%d ", list[i].num);
    }

    part_length = max / partnum;
    thread_num = partnum;

    // Thread part
    pthread_t id[partnum];
    int idnum[partnum];
    for (int i = 0; i < partnum; i++) {
        idnum[i] = i;
        pthread_create(&id[i], NULL, sieve, &idnum[i]);
    }

    for (int i = 0; i < partnum; i++) {
        pthread_join(id[i], NULL);
//        printf("Here %d is done! \n", i);
    }
//    printf("Here join is OK! \n");

//    finish = CLOCK();
//    double total_time = finish - start;
//    printf("HU time is %lf\n", total_time);
    gettimeofday(&endTime,NULL);
    Timeuse = 100000*(endTime.tv_sec - startTime.tv_sec)+(endTime.tv_usec - startTime.tv_usec);
    Timeuse /= 1000000;
    printf("Timeuse = %lf s\n", Timeuse);

    if (print)
        printf("Prime numbers below %d:\n", max);
    for(int i = 2; i < max+1; i++) {
        if(list[i].ptrue) {
            if (print)
                printf("%d\n", list[i].num);
            count++;
        }
    }
    printf("Total prime number %d\n", count);

    return 0;
}