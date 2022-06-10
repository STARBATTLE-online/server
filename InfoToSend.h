#pragma once
#include "MapCreator.h"

#include "boost/asio.hpp"

class InfoToSend
{
public:
	InfoToSend() {

    };
	~InfoToSend() {

    };

	//Тут надо подумать. Посмотри, что я на клиенте хочу отправлять, надо это на сервере принять.
	//Ну а отправлять всё, что есть, получается.
	
private:
	std::string to_send;

};
