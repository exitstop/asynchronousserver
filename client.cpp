#include "rc4plus.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <iomanip> 
#include <regex>
#include <boost/bind.hpp>
#include <stdio.h>

// #include <boost/algorithm/string.hpp>
#define TIMEC 1000

using boost::asio::ip::tcp;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using namespace marusa;

enum { max_length = 1024 };
char K[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

char chiperMessHi[] = "hi";
char chiperMessHello[] = "hello";
int StaticPort=0;

class Client{
public:
    Client(char* argv[]):
                    socket(new tcp::socket(io_service)),
                    resolver(new tcp::resolver(io_service)){

            std::cout << "argv[]: " << argv[1]<< " " <<argv[2] << std::endl;
            boost::system::error_code e;
            boost::asio::connect(*socket, resolver->resolve({argv[1], argv[2]}), e);  
            cout << "error code: " << e  << endl; 

            criptRC4P = new RC4P<sizeof(K)>(S, K); 

        if(e!=0){
            /*port  -------------------- start ----------------------     scaner*/           

            boost::asio::io_service *service[50];

            for(int a=1; a<50; a++){  /* 65535 */

                service[a] = new boost::asio::io_service();

                for(int i=a*1000; i < (a*1000+1000); i++){
                    boost::asio::ip::tcp::socket *s = new boost::asio::ip::tcp::socket(*service[a]);
                    boost::asio::ip::tcp::endpoint *e = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), i);
                    // boost::asio::ip::tcp::socket sock2_(service);        

                    boost::asio::deadline_timer  *m = new boost::asio::deadline_timer(*service[a]);
                    s->async_connect(*e, boost::bind(Client::connect_handler, s, m, e, i, boost::asio::placeholders::error));           

                    m->expires_from_now(boost::posix_time::microseconds(TIMEC));
                    m->async_wait(boost::bind(Client::HandleWait, s,boost::asio::placeholders::error));
                }
                service[a]->run();
            }
            for(int a=1; a<50; a++){
                service[a]->stop();        
                delete service[a];        
            }

            if(StaticPort != 0){ 
                boost::asio::connect(*socket, resolver->resolve({"localhost", std::to_string(StaticPort).c_str()}), e);  
                cout << "error code: " << e  << endl; 
            }else{
                cout << "Сервер не обнаружен! Exit(0); " << endl;
                exit(0);
            }

            /*port  --------------------- end ---------------------     scaner*/

        }

           
            criptRC4P->calculate(chiperMessHello, sizeof(chiperMessHello));
            criptRC4P->calculate(chiperMessHi, sizeof(chiperMessHi));
    
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
    void write_rc4plus(char* message,int sizemessage);
    static void connect_handler(boost::asio::ip::tcp::socket *socket, boost::asio::deadline_timer  *m, 
            boost::asio::ip::tcp::endpoint *e, int port, const boost::system::error_code& error);
    static void HandleWait( boost::asio::ip::tcp::socket *socket, const boost::system::error_code& error);
private:
    boost::asio::io_service io_service;
    tcp::socket *socket;
    tcp::resolver *resolver;

    char request[51];
    char reply[max_length];
    std::string command;
    char S[256];
    RC4P<sizeof(K)> *criptRC4P;
    
    
};

void Client::write(char* buffer, size_t buffer_length){
    boost::asio::write(*socket, boost::asio::buffer(buffer, buffer_length));
};

void Client::read(char* buffer, size_t buffer_length){
    socket->read_some( boost::asio::buffer(buffer, buffer_length));
};

void Client::sh(){  /* подобие командной строки */
    std::cout << "user@server: ";        
    std::getline(std::cin,command);        
    if(command.size()>80){       
        std::strcpy(request, command.substr(0, 80).c_str());  
    }

    std::strcpy(request, command.c_str()); 


    std::smatch smatch;
    std::regex regularExpression(R"(\regme '[^']*'|\push '[^']*'|\poll '[^']*')");
    std::string temp;


    while (std::regex_search (command, smatch, regularExpression)) {
        temp = smatch.str();
        int firstsize = temp.find(' ',1)+2;
        int sizetemp  = temp.size()-firstsize;
        if(sizetemp<50){

            if(temp.find("poll")!=std::string::npos){
               char sendmess[] = "poll";
               // boost::asio::write(*socket, boost::asio::buffer(sendmess, sizeof(sendmess)));
               write_rc4plus(sendmess, sizeof(sendmess));

               char filename[50];
               strcpy(filename,temp.substr(firstsize, sizetemp-1).c_str());
               file_poll(filename);
               
            }else if(temp.find("push")!=std::string::npos){
                char sendmess[] = "push";
                write_rc4plus(sendmess, sizeof(sendmess));  
            
            }else if(temp.find("regme")!=std::string::npos){
                  
            }else{
               cout << "default" << endl;
            } 
        }else{
            cout << "строка в кавычках больше 50 символов" << endl;
        }
        command = smatch.suffix().str();
        std::cout << std::endl;

        
    }


      
    


    // std::vector<std::string> strs;
    // boost::split(strs, command, boost::is_any_of("\t "));
    // for (auto str : strs) {
    //     cout << " boost: " << str << endl;
    // }



    // if(strcmp(request,"poll")==0){
    //    file_poll("clientsave.txt");
    // }else if(strcmp(request,"push")==0){
          
    
    // }else if(strcmp(request,"regme")==0){
          
    // }else{
    //    cout << "default" << endl;
    // }           
        
};
void Client::write_rc4plus(char* message,int sizemessage){
    criptRC4P->calculate(message, sizemessage);
    boost::asio::write(*socket, boost::asio::buffer(message, sizemessage));
}

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


void Client::connect_handler(boost::asio::ip::tcp::socket *socket, boost::asio::deadline_timer  *m, 
                    boost::asio::ip::tcp::endpoint *e, int port, const boost::system::error_code& error)
{
    if (!error)
    {
        m->expires_at(boost::posix_time::pos_infin); // Stop the timer !
        //char mess[] = "hello";
        socket->async_write_some(boost::asio::buffer(chiperMessHello, sizeof(chiperMessHello)),
                [&](boost::system::error_code ec, std::size_t length){});

        char hi[5];
        socket->read_some( boost::asio::buffer(&hi,sizeof(hi)));

        // cout << hi << endl;  
        if(hi[0]==chiperMessHi[0]&&hi[1]==chiperMessHi[1]){ cout << "Порт найден: " << port << endl; StaticPort=port; } 

        socket->close();
        delete e;
        delete m;
    }
    else
    {
        delete e;
        delete m;
    }
}

void Client::HandleWait( boost::asio::ip::tcp::socket *socket, const boost::system::error_code& error)
{
    if (!error)
    {
        socket->close();
        delete socket;
        return;
    }else{
        socket->close();
        delete socket;
    }
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
