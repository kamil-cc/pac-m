/**
 * FIFOIOSeparator.cpp
 *
 *  Created on: 27 sie 2016
 *      Author: Kamil Burzyñski
 */

#ifndef GAME_INCLUDES_MTFIFO_FIFOIOSEPARATOR_CPP_
#define GAME_INCLUDES_MTFIFO_FIFOIOSEPARATOR_CPP_

//Standard
#include <iostream>
#include <cstring>
#include <queue>
#include <map>
#include <typeinfo>
#include <sys/types.h> //Wymagany przez semaphore.h
//#include <semaphore.h>

//Boost
#include <boost/log/trivial.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

namespace mtfifo{

//typedef shared_ptr<sem_t> SemaphorePtr; //TODO
typedef shared_ptr<queue<any> > UniversalFIFOPtr;

/**
 * Separator kolejek wejscia/wyjscia, singleton
 */
class FIFOIOSeparator{
	private:
        FIFOIOSeparator();
        FIFOIOSeparator(FIFOIOSeparator const&); //Pusty
        void operator=(FIFOIOSeparator const&); //Pusty
    	//static sem_t semaphore; //TODO mo¿na rozdzielic na dwa semafory, dla FIFO i semaphores
    	//static map<string, SemaphorePtr> FIFOSemaphores; //TODO
    	static map<string, UniversalFIFOPtr> FIFOStorage;
	private:
    	template <typename T,  template<typename U> class W> void nameCheck(W<T>* caller, string& name){
    		string typeName = typeid(W<T>).name();
    		string::size_type i = name.find(typeName);
    		if (i != string::npos){
    			name.erase(i, typeName.length());
    		}else{
    			BOOST_LOG_TRIVIAL(error) << "Dynamic type incompatibility or wrong name";
    		exit(-1);
    		}
    	}

    public:
        virtual ~FIFOIOSeparator();
        static FIFOIOSeparator& getInstance();

        /**
         * Pobieranie w³aœciwej kolejki
         */
        template <typename T,  template<typename U> class W> queue<T>& getFIFO(W<T>* caller, string name){
        	nameCheck(caller, name);
        	queue<T>* newFIFO;
    		//sem_wait(&semaphore); //TODO
    		{//Sekcja krytyczna
    			if(FIFOStorage.count(name)){
    				UniversalFIFOPtr existingFIFO = FIFOStorage[name];
    				queue<any> anyPtr = existingFIFO.get();
    				//TODO
    				//TODO
    				//TODO
    				//newFIFO = dynamic_cast<queue<T>*>(anyPtr);
    				//if(!newFIFO){ //To nie powinno siê nigdy zdarzyc
    				//	BOOST_LOG_TRIVIAL(error) << "Dynamic type incompatibility: getFIFO()";
    				//	exit(-1); //TODO zastanowic siê
    				//}
    			}else{
    				//Tworzymy now¹ kolejkê
    				newFIFO = new queue<T>;
    				UniversalFIFOPtr ptr(newFIFO); //krytyczne miejsce
    				FIFOStorage.insert(make_pair(name, ptr));
    				//Tworzymy nowy semafor
    				//sem_t* semPtr = new sem_t; //TODO
    				//sem_init(semPtr, 0, 1); //TODO
    				//SemaphorePtr semaphorePtr(semPtr); //TODO krytyczne miejsce
    				//FIFOSemaphores.insert(make_pair(name, semaphorePtr)); //TODO
    			}
    		}
    		//sem_post(&semaphore); //TODO
    		return *newFIFO;
        }

        /**
         * Pobieranie semafora do kolejki
         */
        /*template <typename T,  template<typename U> class W> sem_t& getSemaphore(W<T> caller, string name){
            	nameCheck(caller, name);
            	sem_t* result;
        		//sem_wait(&semaphore); //TODO
        		{//Sekcja krytyczna
        			//Tworzymy now¹ kolejkê wiadomoœci
        			if(FIFOSemaphores.count(name)){
        				SemaphorePtr existingSemaphore = FIFOStorage[name];
        				result = dynamic_cast<sem_t*>(existingSemaphore);
        				if(!result){ //To nie powinno siê nigdy zdarzyc
        					BOOST_LOG_TRIVIAL(error) << "Dynamic type incompatibility: getSemaphore()";
        					exit(-1); //TODO zastanowic siê
        				}
        			}else{
    					BOOST_LOG_TRIVIAL(error) << "Wrong calling sequence: getSemaphore()";
        			}
        		}
        		//sem_post(&semaphore); //TODO
        		return *result;
        }*/
};

}

#endif /* GAME_INCLUDES_MTFIFO_FIFOIOSEPARATOR_CPP_ */
