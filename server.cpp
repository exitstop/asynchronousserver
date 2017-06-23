#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <fstream>

#define FILESIZE 1024

using boost::asio::ip::tcp;
using std::cout;
using std::endl;

class session
: public std::enable_shared_from_this<session>
{
    public:
        session(tcp::socket socket)
            : socket_(std::move(socket))
        {
        }

        void start()
        {
            do_read();
        }

    private:
        void do_read()
        {
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                    [this, self](boost::system::error_code ec, std::size_t length)
                    {
                        if (!ec)
                        {
                            do_write(length);
                        }
                    });
        }

        void do_write(std::size_t length)
        {
            auto self(shared_from_this());
            
            cout << data_ << endl;

            
            std::ifstream myfile;
            myfile.open ("example.txt");
            //myfile << "Writing this to a file.\n";
            myfile.seekg(0, myfile.end);
            length = myfile.tellg();
            myfile.seekg(0, myfile.beg);
            cout << "Размер" <<length << endl;
            
            myfile.read(data_, length);
            myfile.close();
            cout << "read from file: " << data_ << " length: "<< length << endl;


            boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                    [this, self](boost::system::error_code ec, std::size_t length_)
                    {
                        cout << "read from file: length: "<< length_ << endl;
                        if (!ec)
                        {
                            do_read();
                        }
                    });
           // cout << "-start" << endl;
            //socket_.write_some(boost::asio::buffer(datafile, length));
           // cout << "-end" << endl;
        }

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
};

class server
{
    public:
        server(boost::asio::io_service& io_service, short port)
            : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
            socket_(io_service)
    {
        do_accept();
    }

    private:
        void do_accept()
        {
            cout << "acceptor" << endl;
            acceptor_.async_accept(socket_,
                    [this](boost::system::error_code ec)
                    {
                        if (!ec)
                        {
                            std::make_shared<session>(std::move(socket_))->start();
                        }

                    do_accept();
                    });
            cout << "end" << endl;
        }

        tcp::acceptor acceptor_;
        tcp::socket socket_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        server s(io_service, std::atoi(argv[1]));
        sleep(1);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

