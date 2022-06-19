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

#include "MapCreator.h"
#include "server.h"
#include "Requests.h"

class MyFramework {

public:
	virtual bool init() {

		map_manager = std::make_shared<MapCreator>();

		RequestManager::setMapCreator(map_manager);

		return true;
	}

	virtual bool tick() {
		std::lock_guard<std::mutex> lock(map_manager->mt);

		map_manager->CheckCollisionsAll();
		map_manager->tick();

		//std::cout << map_manager->serialize() << "\n";

        return false;
	}
private:

	std::shared_ptr<MapCreator> map_manager;

	//Старая строка, но, кстати, icons ещё нигде нет, надо добавить.
	//И тебе и мне. Это всякие иконки типа щита, которые выпадают из астероидов
	//std::vector<Icon*> icons;
};

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main()
{
	std::cout << "Server started...\n";

	srand(time(NULL));

	
	unsigned short port_num = 3333;
    MyFramework framework;
    framework.init();

	std::thread t1([&framework]() {
		auto previous = std::chrono::high_resolution_clock::now();
		while (true)
		{
			if(framework.tick()) break;
			auto current = std::chrono::high_resolution_clock::now();
			auto delta = current - previous;
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			//std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::milliseconds(16) - delta));

		}
		});

	std::thread t2([&port_num]() {
		try
		{
			//it instantiates an object of the Server class named srv.
			Server srv;

			//before starting the server, the optimal size of the pool is calculated.
			// The general formula often used in parallel applications to find the optimal number of threads is the number of processors the computer has multiplied by 2.
			// We use the std::thread::hardware_concurrency() static method to obtain the number of processors.
			unsigned int thread_pool_size =
				std::thread::hardware_concurrency() * 2;

			//because this method may fail to do its job returning 0,
			// we fall back to default value represented by the constant DEFAULT_THREAD_POOL_SIZE, which is equal to 2 in our case.
			if (thread_pool_size == 0)
				thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

			srv.start(port_num, thread_pool_size);

			while(true) {
				std::this_thread::sleep_for(std::chrono::seconds(10000000));
			}

			srv.stop();
		}
		catch (boost::system::system_error& e)
		{
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
