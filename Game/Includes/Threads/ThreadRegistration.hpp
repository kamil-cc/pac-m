/*
 * ThreadRegistration.hpp
 *
 *  Created on: 1 wrz 2016
 *      Author: Kamil
 */

#ifndef GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_
#define GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_

//Std
#include <map>

//Boost
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>

//App
#include <GameAssert/GameAssert.hpp>

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
    	boost::lock_guard<ThreadRegistration> guard(*this);
    	std::string key = name + std::string(": ") + boost::lexical_cast<std::string>(id);
    	std::pair<std::map<const boost::thread::id, const std::string>::iterator, bool> res  =
    			map_.insert(std::pair<const boost::thread::id, const std::string>(id, key));
    	if(!res.second)
    		gameAssert(!"Double registration");
    }

    std::string getName(const boost::thread::id& id){ //TODO do pliku cpp
    	boost::lock_guard<ThreadRegistration> guard(*this);
    	std::map<const boost::thread::id, const std::string>::iterator it;
    	if((it = map_.find(id)) == map_.end())
    		gameAssert(!"No such id");
    	return it->second;
    }
private:
    std::map<const boost::thread::id, const std::string> map_;
};

}

#endif /* GAME_INCLUDES_THREADS_THREADREGISTRATION_HPP_ */
