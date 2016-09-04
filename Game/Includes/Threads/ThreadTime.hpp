/*
 * ThreadTime.hpp
 *
 *  Created on: 1 wrz 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_THREADS_THREADTIME_HPP_
#define GAME_INCLUDES_THREADS_THREADTIME_HPP_

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>

namespace thd{
	const boost::chrono::milliseconds LOG_TIME = boost::chrono::milliseconds(100);
	const boost::chrono::milliseconds TCPIP_RECEIVER_TIME = boost::chrono::milliseconds(10);
	const boost::chrono::milliseconds TCPIP_SENDER_TIME = boost::chrono::milliseconds(10);

	class TimeMaster{
	public:
		boost::chrono::milliseconds calculateTime(boost::chrono::milliseconds time,
				std::size_t length){
			int half = length / 2;
			if(half <= 1)
				return time;
			return time / half;
		}
	};

	class ThreadTime : private boost::noncopyable,
		public boost::basic_lockable_adapter<boost::mutex>{
		public:
		static ThreadTime& getInstance(){
			static ThreadTime tt;
			return tt;
		}

		TimeMaster *factory(){
			boost::lock_guard<ThreadTime> guard(*this);
			return new TimeMaster();
		}
	};
}

#endif /* GAME_INCLUDES_THREADS_THREADTIME_HPP_ */
