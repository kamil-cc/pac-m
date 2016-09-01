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
    static ThreadRegistration& getInstance(){ //TODO wyrzucic implementacjê do pliku cpp
    	//Dodac w tym miejscu jakiœ zamek, np mutex statyczny
        static ThreadRegistration instance;
        return instance;
    }
};

void registerThread(const boost::thread::id& id, const std::string& name){ //TODO wrzucic implementacjê do pliku cpp
	boost::lock_guard<ThreadRegistration> guard(ThreadRegistration::getInstance());
	//wstawianie do mapy
}

}

#endif /* GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_ */
