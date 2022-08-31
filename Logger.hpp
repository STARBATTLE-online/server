#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <iomanip>



class Logger {
public:
    static void log(std::string str);
    //Here, make modifications to the output process of the logger.
    static void setSettings();
    static void addMemory(size_t size);
    Logger(){
        /*Logger::filename="./log.log";
        Logger::out.open(Logger::filename);
        Logger::log("Logger created\n");
        Logger::memory=0;*/
    }
    //change this in constructor if you need to
    static std::string filename;
    static std::ofstream out;
    static std::mutex inner_mutex;
    static int memory;
};

void Logger::log(std::string str) {
    std::cout << str;
    /*std::thread log_thread([str]() {  //launch server
        auto time_now=std::chrono::system_clock::now();
        auto other_time=std::chrono::system_clock::to_time_t(time_now);
        std::lock_guard<std::mutex> logger_lock{Logger::inner_mutex};
        out<<std::ctime(&other_time)<<"   memory bytes active: "<<std::setw(7)<<memory<<"     "<<str<<"\n";
    });*/
}

void Logger::addMemory(size_t size) {
    /*memory+=size;

    auto time_now=std::chrono::system_clock::now();
    auto other_time=std::chrono::system_clock::to_time_t(time_now);
    out<<std::ctime(&other_time)<<"   memory bytes active: "<<std::setw(7)<<memory<<"\n";*/
}