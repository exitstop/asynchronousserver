#include "rc4plus.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <iomanip> 


using boost::asio::ip::tcp;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using namespace marusa;

enum { max_length = 1024 };

char K[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

class Client{
public:
    Client(char* argv[]):
                    socket(new tcp::socket(io_service)),
                    resolver(new tcp::resolver(io_service)){

        std::cout << "argv[]: " << argv[1]<< " " <<argv[2] << std::endl;
        boost::asio::connect(*socket, resolver->resolve({argv[1], argv[2]}));  
        
        criptRC4P=new RC4P<sizeof(K)>(S, K);      
    }
    ~Client(){
       delete socket;
       delete resolver;
       delete criptRC4P; 
    }
    void write(char* buffer, size_t buffer_length);
    void read (char* buffer, size_t buffer_length);
    void sh();
    void file_poll(char* filename);
    void file_push(char* filename);
private:
    boost::asio::io_service io_service;
    tcp::socket *socket;
    tcp::resolver *resolver;

    char request[51];
    char reply[max_length];
    std::string command;
    char S[256];
    ;
    RC4P<sizeof(K)> *criptRC4P;
};

void Client::write(char* buffer, size_t buffer_length){
    boost::asio::write(*socket, boost::asio::buffer(buffer, buffer_length));
};

void Client::read(char* buffer, size_t buffer_length){
    socket->read_some( boost::asio::buffer(buffer, buffer_length));
};

void Client::sh(){
    std::cout << "user@server: ";        
    std::getline(std::cin,command);        
    if(command.size()>50){       
        std::strcpy(request, command.substr(command.size() - 50).c_str());  
    }
    std::strcpy(request, command.c_str()); 
    size_t request_length = std::strlen(request);
    boost::asio::write(*socket, boost::asio::buffer(request, request_length));

    if(strcmp(request,"poll")==0){
       file_poll("clientsave.txt");
    }else if(strcmp(request,"push")==0){
          
    
    }else if(strcmp(request,"regme")==0){
          
    }else{
       cout << "default" << endl;
    }           
        
};

void Client::file_poll(char* filename){
    int hsize=0;
    socket->read_some( boost::asio::buffer(&hsize, sizeof(hsize)));    
    int countread = hsize%1024;
    int sread=socket->read_some( boost::asio::buffer(reply));

    std::ofstream myfile;
    myfile.open (filename);    


    criptRC4P->calculate(reply, countread);

    myfile.write(reply, sread);

    for(int i=0; i<hsize/1024; i++){
        size_t reply_length2 = socket->read_some( boost::asio::buffer(reply, 1024));
        criptRC4P->calculate(reply, reply_length2);
        myfile.write(reply,reply_length2);
    }

    myfile.close();
    std::cout << "Получен файл. Размер: " << hsize << std::endl;
}

int main(int argc, char* argv[])
{
    
    string com;
    cin >> com;

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
