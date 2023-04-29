#include "Barrier.h"


Barrier::Barrier(std::size_t num) : num_threads(num), wait_count(0), instance(0), mut(), cv(){
    if (num == 0) {
        throw std::invalid_argument("Barrier thread count cannot be 0");
    }    
}

void Barrier::Wait() {

     // acquire lock
    std::unique_lock<std::mutex> lock(mut);

    // store current instance for comparison in predicate
    std::size_t inst = instance; 
    
    if (++wait_count == num_threads) { 
        // all threads reached barrier
        wait_count = 0; // reset wait_count
        instance++; // increment instance for next use of barrier and to pass condition variable predicate
        cv.notify_all();
    } else { 
        // not all threads have reached barrier
        cv.wait(lock, [this, &inst]() { return instance != inst; });
        // NOTE: The predicate lambda here protects against spurious
        //       wakeups of the thread. As long as this->instance is
        //       equal to inst, the thread will not wake.
        //       this->instance will only increment when all threads
        //       have reached the barrier and are ready to be unblocked.
    }
}
