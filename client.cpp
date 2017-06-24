#include "rc4.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <iomanip> 

using boost::asio::ip::tcp;
using std::cout;
using std::endl;
using std::string;
using namespace marusa;
enum { max_length = 1024 };

class Client{
public:
    Client(char* argv[]){
        std::cout << "argv[]: " << argv[1]<< " " <<argv[2] << std::endl;
        s=new tcp::socket(io_service);
        // tcp::socket s(io_service);
        resolver = new tcp::resolver(io_service);
        boost::asio::connect(*s, resolver->resolve({argv[1], argv[2]}));        
    }
    ~Client(){
       delete s;
       delete resolver; 
    }
private:
    boost::asio::io_service io_service;
    tcp::socket *s;
    tcp::resolver *resolver;

};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        // Client client(argv);
        // exit(0);

        boost::asio::io_service io_service;

        tcp::socket s(io_service);
        tcp::resolver resolver(io_service);
        boost::asio::connect(s, resolver.resolve({argv[1], argv[2]}));

        std::cout << "Enter message: ";
        //char request[max_length];
        std::string command;
        //cout << command << endl;
        std::getline(std::cin,command);
        char *request = new char[51];
        if(command.size()>50){
            std::strcpy(request, command.substr(command.size() - 50).c_str());  
            cout << "обрезали" <<endl;
        }
        std::strcpy(request, command.c_str()); 
        // const char *request = command.substr(command.size() - 50).c_str();
        // char *request = new char[command.size()+1];
        // std::strcpy(request, command.substr(command.size() - 50).c_str());
        //std::cin >> std::setw(50) >> request;
        size_t request_length = std::strlen(request);
        // request[request_length]=0;
        boost::asio::write(s, boost::asio::buffer(request, request_length));

        int hsize=0;
        int* headr=&hsize; 
        s.read_some( boost::asio::buffer(headr, sizeof(hsize)));
        std::cout << *headr << std::endl;

        char reply[max_length];
        std::ofstream myfile;
        myfile.open ("clientsave.txt");

        int countread = hsize%1024;
        int sread=s.read_some( boost::asio::buffer(reply));

        char* S = new char[256];
        char K[]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        RC4P<sizeof(K)> r(S, K);
        r.calculate(reply, countread);

        myfile.write(reply, sread);

        for(int i=0; i<hsize/1024; i++){
            size_t reply_length2 = s.read_some( boost::asio::buffer(reply, 1024));
            r.calculate(reply, reply_length2);
            myfile.write(reply,reply_length2);
        }

        std::cout << reply << std::endl;
        std::cout << "Reply is: ";

        // delete[] request;

        myfile.close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
