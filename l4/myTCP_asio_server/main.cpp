//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// https://think-async.com/Asio/asio-1.20.0/doc/asio/examples/cpp14_examples.html

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <cstring>

using boost::asio::ip::tcp;

class session
    : public std::enable_shared_from_this<session>
{
public:
    //bool* run;

    session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start(/*bool* run_*/)
    {
        //run=run_;
        std::cout << "Client accepted\n";
        do_read();
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];

    void do_read()
    {        
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](std::error_code ec, std::size_t length)
                                {
                                    if (!ec)
                                    {
                                        do_command(length);
                                    }
                                });
    }

    void do_command(std::size_t length)
    {
        auto self(shared_from_this());
        // Command analyse
        data_[length] = '\0';
        std::string dat(data_);
        std::cout << dat << std::endl;
        std::istringstream iss(dat);
        std::string command = {0};
        std::string filename = {0};
        std::string fiction = {0};
        size_t fict = 0;
        int size_fict = 0;

        iss >> command >> filename >> fiction >> size_fict;
        if ("exit" == command) {
            std::cout << "Breaking" << std::endl;
            //*run = false;
            std::exit(0);
        }

        if ("get" == command && length > 4) {
            if ("beans" == fiction) {
                fict = 1;
            } else if ("size" == fiction) {
                fict = 2;
            }
        }

        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                          [this, self](std::error_code ec, std::size_t /*length*/)
                          {
                             //if (!ec && *run)
                             if (!ec)
                              {
                                  do_read();
                              }
                           });
    }


};

class server
{
public:
    //bool run=true;
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_,
                               [this](std::error_code ec)
                               {
                                   if (!ec)
                                   {   //auto a=std::make_shared<session>(std::move(socket_));
                                       std::make_shared<session>(std::move(socket_))->start();
                                       //a->start(&run);

                                   }
                                   /*if (run)*/ do_accept();
                               });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

int main(int argc, char* argv[])
{
    std::cout << "Running echo server...\n" << std::endl;
    std::cout << "Command:\n" << std::endl;
    std::cout << "        exit - to quit\n" << std::endl;
    std::cout << "        get <filename> <beans/size> <number>\n" << std::endl;

    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
