/*
 * @Author: XiaChuerwu 1206575349@qq.com
 * @Date: 2023-09-07 10:07:57
 * @LastEditors: XiaChuerwu 1206575349@qq.com
 * @LastEditTime: 2023-09-07 10:18:27
 * @FilePath: \SkipList\stress\stress_test.cpp
 */

#include "../SkipList.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 1
#define TEST_COUNT 100000
SkipList<int, std::string> skipList(18);

void *insertElement(void *threadid) {
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for (int i = tid * tmp, count = 0; count < tmp; i++) {
        count++;
        skipList.insertElement(rand() % TEST_COUNT, "a");
    }
    pthread_exit(NULL);
}

void *getElement(void *threadid) {
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for (int i = tid * tmp, count = 0; count < tmp; i++) {
        count++;
        skipList.searchElement(rand() % TEST_COUNT);
    }
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    {

        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for (i = 0; i < NUM_THREADS; i++) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, insertElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for (i = 0; i < NUM_THREADS; i++) {
            if (pthread_join(threads[i], &ret) != 0) {
                perror("pthread_create() error");
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    }

    pthread_exit(NULL);
    return 0;
}
