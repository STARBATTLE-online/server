#define CURL_STATICLIB

#include <iostream>
#include <random>
#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <regex>
#include <thread>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Logger.hpp"
#include "World.h"
#include "server.h"
#include "Requests.h"


const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

/**
 * @brief This class manages the main game loop
 */
class MyFramework {

public:
	virtual bool init() {

		map_manager = std::make_shared<World>();

		RequestManager::setWorld(map_manager);

		return true;
	}

	virtual bool tick() {
		std::lock_guard<std::mutex> lock(map_manager->mt);

		map_manager->tick();
		map_manager->checkAllCollisions();
		
        return false;
	}
private:
	std::shared_ptr<World> map_manager;
};
	
int main() {
	//initialize logger, must be done before anything else
	Logger();

	Logger::log("Server started...\n");

	srand(time(NULL));
	
	unsigned short port_num = 3333;
    MyFramework framework;
    framework.init();

	std::thread t1([&framework]() { //weird time management purposes, ignore
		while (true) {
			auto previous = std::chrono::high_resolution_clock::now();
			if(framework.tick()) break;
			auto current = std::chrono::high_resolution_clock::now();
			std::this_thread::sleep_for(std::chrono::milliseconds(16) - (current - previous));
		}
	});

	std::thread t2([&port_num]() {  //launch server
		try {
			//it instantiates an object of the Server class named srv.
			Server srv;
			unsigned int thread_pool_size = DEFAULT_THREAD_POOL_SIZE; // While we technically can use more than 1 thread, it won't give us any more performance.

			srv.start(port_num, thread_pool_size);

			while(true) {
				std::this_thread::sleep_for(std::chrono::seconds(10000000));
			}

			srv.stop();
		}
		catch (boost::system::system_error& e) {
			std::cout << "Error occured! Error code = "
				<< e.code() << ". Message: "
				<< e.what();
		}
		});

	t1.join();
	t2.join();

	std::cout << "Server shut down.\n";

	return 0;
}
