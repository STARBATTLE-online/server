//
// Created by fail on 6/13/22.
//

#ifndef STARWARS_SERVER_SERVER_H
#define STARWARS_SERVER_SERVER_H

#include <boost/asio.hpp>

#include <memory>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <utility>

#include "Requests.h"
#include "World.h"

std::string makeString(boost::asio::streambuf& streambuf) {
    return {buffers_begin(streambuf.data()), 
            buffers_end(streambuf.data())};
}

//responsible for handling a single client by reading the request message, processing it, and then sending back the response message.
//Each instance of the Service class is intended to handle one connected client
//by reading the request message, processing it, and then sending the response message back.
class Service {
public:
    //The class's constructor accepts a shared pointer to an object representing a socket connected to a particular client as an argument
    // and caches this pointer. This socket will be used later to communicate with the client application.
    explicit Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) : m_sock(std::move(sock)) {}

    //This method starts handling the client by initiating the asynchronous reading operation
    //to read the request message from the client specifying the onRequestReceived() method as a callback.
    void startHandling() {

        boost::asio::async_read_until(*m_sock,
                               m_request,
                               '\n',
                               [this](
                                       const boost::system::error_code &ec,
                                       std::size_t bytes_transferred) {
                                   //When the request reading completes, or an error occurs, the callback method onRequestReceived() is called.
                                   onRequestReceived(ec,
                                                     bytes_transferred);
                               });
    }

private:
    void onRequestReceived(const boost::system::error_code &ec,
                           std::size_t bytes_transferred) {
        //This method first checks whether the reading succeeded by testing the ec argument that contains the operation completion status code.
        if (ec.value() != 0) {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
            //reading finished with an error, the corresponding message is output to the standard output stream
            //and then the onFinish() method is called.
            onFinish();
            return;
        }

        // Process the request.
        m_response = processTCPRequest(m_request);
        // When the ProcessRequest() method completes and returns the string containing the response message,
        // the asynchronous writing operation is initiated to send this response message back to the client.
        boost::asio::async_write(*m_sock, boost::asio::buffer(m_response),
                          [this](
                                  const boost::system::error_code &ec,
                                  std::size_t bytes_transferred) {
                              //The onResponseSent() method is specified as a callback.
                              onResponseSent(ec, bytes_transferred);
                          });
    }

    void onResponseSent(const boost::system::error_code &ec,
                        std::size_t bytes_transferred) {
        // This method first checks whether the operation succeeded.
        if (ec.value() != 0) {
            // If the operation failed, the corresponding message is output to the standard output stream.
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }

        //method is called to perform the cleanup.
        onFinish();
    }

    // Here we perform the cleanup.
    void onFinish() {
        //delete this;
    }

    //To keep things simple,  we implement a dummy service which only emulates the execution of certain operations
    //The request processing emulation consists of performing many increment operations to emulate operations
    //that intensively consume CPU and then putting the thread of control to sleep for some time to emulate I/O operations
    static std::string processTCPRequest(boost::asio::streambuf &request) {

        // In this method we parse the request, process it
        // and prepare the request.

        // Emulate CPU-consuming operations.

        // Emulate operations that block the thread
        // (e.g. synch I/O operations).

        // Prepare and return the response message.
        //std::string response = "Response from server\n";
        std::string response;
        /*int kbsize=1;
        int bytesInKB=1024;
        for(int j=0; j<kbsize; j++){
            for(int i=0; i<bytesInKB; i++){
                response.push_back('1');
            }
        }
        response.push_back('\n');*/
        
        response = RequestManager::processRequest(makeString(request) + "\n");
        return response;
    }

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
    std::string m_response;
    boost::asio::streambuf m_request;
};

//responsible for accepting the connection requests arriving from clients and instantiating the objects of the Service class,
// which will provide the service to connected clients.
class Acceptor {
public:
    //Its constructor accepts a port number on which it will listen for the incoming connection requests as its input argument.
    Acceptor(boost::asio::io_service &ios, unsigned short port_num) : m_ios(ios),
            //The object of this class contains an instance of the asio::ip::tcp::acceptor class as its member named m_acceptor,
            //which is constructed in the Acceptor class's constructor.
                                                               m_acceptor(m_ios,
                                                                          boost::asio::ip::tcp::endpoint(
                                                                                  boost::asio::ip::address_v4::any(),
                                                                                  port_num)),
                                                               m_isStopped(false) {}

    //The Start() method is intended to instruct an object of the Acceptor class to start listening and accepting incoming connection requests.
    void start() {
        //It puts the m_acceptor acceptor socket into listening mode
        m_acceptor.listen();
        initAccept();
    }

    // Stop accepting incoming connection requests.
    void stop() {
        m_isStopped.store(true);
    }

private:
    void initAccept() {
        //constructs an active socket object and initiates the asynchronous accept operation
        std::shared_ptr<boost::asio::ip::tcp::socket>
                sock(new boost::asio::ip::tcp::socket(m_ios));

        //calling the async_accept() method on the acceptor socket object
        // and passing the object representing an active socket to it as an argument.
        m_acceptor.async_accept(*sock,
                                [this, sock](
                                        const boost::system::error_code &error)
                                {
                                    //When the connection request is accepted or an error occurs, the callback method onAccept() is called.
                                    onAccept(error, sock);
                                });
    }

    void onAccept(const boost::system::error_code &ec,
                  std::shared_ptr<boost::asio::ip::tcp::socket> sock) {
        if (ec.value() == 0) {
            //an instance of the Service class is created and its StartHandling() method is called
            (new Service(std::move(sock)))->startHandling();
        } else {
            //the corresponding message is output to the standard output stream.
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }

        // Init next async accept operation if
        // acceptor has not been stopped yet.
        if (!m_isStopped.load()) {
            initAccept();
        } else {
            // Stop accepting incoming connections
            // and free allocated resources.
            m_acceptor.close();
        }
    }

private:
    boost::asio::io_service &m_ios;
    //used to asynchronously accept the incoming connection requests.
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_isStopped;
};

//represents the server itself
class Server {
public:
    Server() {
        m_work = std::make_unique<boost::asio::io_service::work>(m_ios);
    }

    // Start the server.
    // Accepts a protocol port number on which the server should listen for the incoming connection requests
    // and the number of threads to add to the pool as input arguments and starts the server
    // Nonblocking Method
    void start(unsigned short port_num,
               unsigned int thread_pool_size) {

        assert(thread_pool_size > 0);

        // Create and start Acceptor.
        acc = std::make_unique<Acceptor>(m_ios, port_num);
        acc->start();

        // Create specified number of threads and
        // add them to the pool.
        for (unsigned int i = 0; i < thread_pool_size; i++) {
            std::unique_ptr<std::thread> th(
                    new std::thread([this]()
                                    { m_ios.run(); }));

            m_thread_pool.push_back(std::move(th));
        }
    }

    // Stop the server.
    // Blocks the caller thread until the server is stopped and all the threads running the event loop exit.
    void stop() {
        acc->stop();
        m_ios.stop();

        for (auto &th : m_thread_pool) {
            th->join();
        }
    }

private:
    boost::asio::io_service m_ios;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::unique_ptr<Acceptor> acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};

#endif //STARWARS_SERVER_SERVER_H
