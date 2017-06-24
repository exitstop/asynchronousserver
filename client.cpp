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
    void write(char* buffer, size_t buffer_length);
    void read (char* buffer, size_t buffer_length);
    void sh();
    void file_recv(char* filename);
private:
    boost::asio::io_service io_service;
    tcp::socket *s;
    tcp::resolver *resolver;

    char request[51];
    char reply[max_length];
    std::string command;
};

void Client::write(char* buffer, size_t buffer_length){
    boost::asio::write(*s, boost::asio::buffer(buffer, buffer_length));
};

void Client::read(char* buffer, size_t buffer_length){
    s->read_some( boost::asio::buffer(buffer, buffer_length));
};

void Client::sh(){
    std::cout << "user@server: ";        
    std::getline(std::cin,command);        
    if(command.size()>50){       
        std::strcpy(request, command.substr(command.size() - 50).c_str());  
    }
    std::strcpy(request, command.c_str()); 
    size_t request_length = std::strlen(request);
    boost::asio::write(*s, boost::asio::buffer(request, request_length));

    if(strcmp(request,"send")==0){
       file_recv("clientsave.txt");
    }else if(strcmp(request,"regme")){
       
    }else{
       cout << "default" << endl;
    }           
        
};

void Client::file_recv(char* filename){
    int hsize=0;
    s->read_some( boost::asio::buffer(&hsize, sizeof(hsize)));    
    int countread = hsize%1024;
    int sread=s->read_some( boost::asio::buffer(reply));

    std::ofstream myfile;
    myfile.open (filename);    

    char* S = new char[256];
    char K[]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    RC4P<sizeof(K)> r(S, K);
    r.calculate(reply, countread);

    myfile.write(reply, sread);

    for(int i=0; i<hsize/1024; i++){
        size_t reply_length2 = s->read_some( boost::asio::buffer(reply, 1024));
        r.calculate(reply, reply_length2);
        myfile.write(reply,reply_length2);
    }

    myfile.close();
    std::cout << "Получен файл. Размер: " << hsize << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        Client cli(argv);
        cli.sh();
        exit(0);

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
