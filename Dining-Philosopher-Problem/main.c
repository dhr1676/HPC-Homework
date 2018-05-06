#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define random(x) (rand()%x)

//double CLOCK() {
//    struct timespec t;
//    clock_gettime(CLOCK_MONOTONIC,  &t);
//    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
//}
typedef struct {
    int thread_num;
    int phil_num;
    int eating_round;
}arg_struct;

omp_lock_t forklock[100];

void eat(void *arg)
{
    srand(24);
    arg_struct args_inner = *(arg_struct *)arg;
    int eat_round_inner = args_inner.eating_round;
    int eat_round_now =0;
    int thread_num_inner = args_inner.thread_num;
//    int phil_num_inner = omp_get_thread_num();
    int phil_num_inner = args_inner.phil_num;
    int fork_left,fork_right;

    if (phil_num_inner == thread_num_inner - 1){
        fork_left = 0;
        fork_right = phil_num_inner;
    } else {
        fork_left = phil_num_inner;
        fork_right = phil_num_inner + 1;
    }

    while( eat_round_now < eat_round_inner)
    {
//        printf("Philosopher %d is thinking!\n",phil_num_inner);
//        sleep((unsigned)random(5));
        sleep(1);


//        printf("Philosopher %d is hungry!\n",phil_num_inner);
        omp_set_lock(&forklock[fork_left]);
//        printf("Philosopher %d takes the left fork %d\n",phil_num_inner,fork_left);
        omp_set_lock(&forklock[fork_right]);
//        printf("Philosopher %d takes the right fork %d\n",phil_num_inner,fork_right);
//        printf("Philosopher %d is eating!\n",phil_num_inner);
//        sleep((unsigned)random(5));
        sleep(1);

        omp_unset_lock(&forklock[fork_right]);
//        printf("Philosopher %d puts down the right fork %d\n",phil_num_inner,fork_right);
        omp_unset_lock(&forklock[fork_left]);
//        printf("Philosopher %d puts down the left fork %d\n",phil_num_inner,fork_left);
        printf("Philosopher %d Eat times = %d\n", omp_get_thread_num(),eat_round_now+1);

        eat_round_now++;
    }
}

int main(int argc, char **argv)
{
    if(argc < 3){
        printf("Input format is <philosopher number> <eating round>\n");
        return 0;
    }

//    double start, finish,total_time;
    struct timeval startTime,endTime;
    double Timeuse;
    int input_thread_num = atoi(argv[1]);
    int input_eating_round = atoi(argv[2]);

    int max_thrds_num,act_thrds_num;
    max_thrds_num = omp_get_max_threads();
    printf("Max available threads num = %d\n", max_thrds_num);
    if(max_thrds_num > input_thread_num) {
        omp_set_num_threads(input_thread_num);
        act_thrds_num = input_thread_num;
    }
    else {
        omp_set_num_threads(max_thrds_num);
        act_thrds_num = max_thrds_num;
    }
    printf("Actual threads num = %d\n", act_thrds_num);

    omp_lock_t forklock[act_thrds_num];
    for(int i = 0; i < act_thrds_num; i++){
        omp_init_lock( &forklock[i]);
    }

    arg_struct *args = malloc(sizeof(arg_struct));
    args->thread_num = act_thrds_num;
    args->eating_round = input_eating_round;

//    start = CLOCK();
    gettimeofday(&startTime,NULL);
    int k;
#pragma omp parallel for private(k)
    for (k = 0; k < act_thrds_num; k++){
        args->phil_num = k;
        eat(args);
    }
//#pragma omp barrier

//    finish = CLOCK();
//    total_time = finish - start;
//    printf("Parallel time = %lf ms\n",total_time);

    gettimeofday(&endTime,NULL);
    Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec)+(endTime.tv_usec - startTime.tv_usec);
    Timeuse /= 1000000;
    printf("Timeuse = %lf s\n", Timeuse);
    return 0;
}