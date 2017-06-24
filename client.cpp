#include "rc4.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>

using boost::asio::ip::tcp;
using namespace marusa;
enum { max_length = 1024 };

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::socket s(io_service);
        tcp::resolver resolver(io_service);
        boost::asio::connect(s, resolver.resolve({argv[1], argv[2]}));

        std::cout << "Enter message: ";
        char request[max_length];
        size_t request_length = std::strlen(request);
        boost::asio::write(s, boost::asio::buffer(request, request_length));

        int hsize=0;
        int* headr=&hsize; 
        s.read_some( boost::asio::buffer(headr, sizeof(hsize)));
        std::cout << *headr << std::endl;

        char reply[max_length];
        std::ofstream myfile;
        myfile.open ("clientsave.txt");
        int sread=s.read_some( boost::asio::buffer(reply));

        char* S = new char[256];
        char K[]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        int countread = hsize%1024;
        RC4P<sizeof(K)> r(S, K);
        r.calculate(reply, countread);

        myfile.write(reply, sread);

        for(int i=0; i<hsize/1024; i++){
            size_t reply_length2 = s.read_some( boost::asio::buffer(reply));
            r.calculate(reply, reply_length2);
            myfile.write(reply,reply_length2);
        }

        std::cout << reply << std::endl;
        std::cout << "Reply is: ";


        myfile.close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
