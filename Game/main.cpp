/*
 * main.cpp
 *
 *  Created on: 02 maj 2016
 *      Author: Kamil Burzyñski
 */

//Standard
#include <cmath>
#include <cstring>

//Boost
#include <boost/log/trivial.hpp>
#include <MtFIFO/FIFO.hpp>
#include <MtFIFO/FIFODistributor.hpp>
#include <MtFIFO/FIFOInput.hpp>
#include <MtFIFO/FIFOOutput.hpp>
#include <SFML/Graphics.hpp>

//App

using namespace boost::log::trivial;

int main() {
	BOOST_LOG_TRIVIAL(trace) << "Main function start";

	mtfifo::FIFODistributor& FIFODistributor = mtfifo::FIFODistributor::getInstance();
	FIFODistributor.getFIFO<mtfifo::FIFOInput>("000");
	FIFODistributor.getFIFO<mtfifo::FIFOOutput>("000");
	FIFODistributor.getFIFO<mtfifo::FIFOInput>("001");

	sf::RenderWindow window(sf::VideoMode(640, 480), "Pac++M");
	sf::Clock stoper;

	while(window.isOpen()){
		//BOOST_LOG_TRIVIAL(trace) << "W pierwszej petli while";
		sf::Event event;

		while(window.pollEvent( event )){
			//BOOST_LOG_TRIVIAL(trace) << "W drugiej petli while";

			if( event.type == sf::Event::Closed ){
				window.close();
				return 0;
			}
		} //while

		/*window.clear();

		sf::CircleShape ksztalt( std::sin( stoper.getElapsedTime().asSeconds() )
			* window.getSize().y / 8 + window.getSize().y / 4 );
		ksztalt.setOrigin( sf::Vector2f( ksztalt.getRadius(), ksztalt.getRadius() ) );
		ksztalt.setPosition( window.getSize().x / 2.0f, window.getSize().y / 2.0f );
		ksztalt.setFillColor( sf::Color::Yellow );
		window.draw( ksztalt );

		window.display();*/
	} //while
	BOOST_LOG_TRIVIAL(trace) << "Main function end";
	return 0;
}
