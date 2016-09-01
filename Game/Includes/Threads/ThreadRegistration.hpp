/*
 * ThreadRegistration.hpp
 *
 *  Created on: 1 wrz 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_
#define GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_

//Boost
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>

namespace thd{

class ThreadRegistration : private boost::noncopyable,
	public boost::basic_lockable_adapter<boost::mutex>{
public:
	/**
	 * Pobieranie jedynej instancji klasy.
	 * "in C++11 static locals are guaranteed to be initialized by the runtime in a thread-safe manner."
	 * http://stackoverflow.com/questions/6116520/c-is-this-getinstance-method-of-my-singleton-threadsafe
	 */
    static ThreadRegistration& getInstance(){ //TODO wrzucic implementacjê do pliku cpp
        static ThreadRegistration instance;
        return instance;
    }

    void registerThread(const boost::thread::id& id, const std::string& name){ //TODO wrzucic implementacjê do pliku cpp
    	boost::lock_guard<ThreadRegistration> guard(/*ThreadRegistration::getInstance()*/*this);

    }
};

}

#endif /* GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_ */
