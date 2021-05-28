#include <vector>
#include <iostream>

#include <pthread.h>

#include <gtest/gtest.h>

#include "shmem/SharedMemoryRequestQueue.h"

using namespace std;
using namespace activebsp;

activebsp::SharedMemoryRequestQueue threaded_queue;

void * consumer_thread(void * arg)
{
    int * expected = (int *) arg;

    for (int i = 0; i < *expected; ++i)
    {
        ActiveObjectRequest req = threaded_queue.getNextMessage();

        EXPECT_EQ(-1, req.getSource());
    }

    EXPECT_TRUE(threaded_queue.isEmpty());

    return NULL;
}

void * producer_thread(void * arg)
{
    int * expected = (int *) arg;

    for (int i = 0; i < *expected; ++i)
    {
        threaded_queue.postMessage(ActiveObjectRequest(std::make_shared<CallActorMessage>(-1, nullptr, 0), -1));
    }

    return NULL;
}

TEST(testThreadedQueue, testThreadedQueue)
{
    pthread_t threads[2];
    int nmessages = 5;

    pthread_create(&threads[0], NULL, consumer_thread, &nmessages);
    pthread_create(&threads[1], NULL, producer_thread, &nmessages);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    EXPECT_EQ(42, 42);
}
