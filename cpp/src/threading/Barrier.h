
#pragma once

#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <thread>

// originally taken from
// https://github.com/kirksaunders/barrier

class Barrier {
    
private:
    std::size_t num_threads; // number of threads using barrier
    std::size_t wait_count; // counter to keep track of waiting threads
    std::size_t instance; // counter to keep track of barrier use count
    std::mutex mut; // mutex used to protect resources
    std::condition_variable cv; // condition variable used to block threads

public:

    // Construct barrier for use with num threads.
    Barrier(std::size_t num);   

    // disable copying of barrier
    Barrier(const Barrier&) = delete;
    Barrier& operator =(const Barrier&) = delete;

    // This function blocks the calling thread until
    // all threads (specified by num_threads) have
    // called it. Blocking is achieved using a
    // call to condition_variable.wait().
    void Wait();
};
