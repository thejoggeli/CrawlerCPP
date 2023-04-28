#pragma once 

#include <mutex>
#include <condition_variable>

// take from 
// https://gist.github.com/Kuxe/6bdd5b748b5f11b303a5cccbb8c8a731

/** General semaphore with N permissions **/
class Semaphore {
	const size_t num_permissions;
	size_t avail;
	std::mutex m;
	std::condition_variable cv;
public:

	/** Default constructor. Default semaphore is a binary semaphore **/
	explicit Semaphore(const size_t& num_permissions = 1);

	/** Copy constructor. Does not copy state of mutex or condition variable,
		only the number of permissions and number of available permissions **/
	Semaphore(const Semaphore& s);

	void Acquire();

	void Release();

	size_t Available() const;

};
