#include <cstdlib>
#include "rc4plus.h" 
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <fstream>


#include "sql.h"

#define FILESIZE 1024

using boost::asio::ip::tcp;
using namespace marusa;
using std::cout;
using std::endl;
using std::string;
using std::vector;



char* S = new char[256];
char K[]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
RC4P<sizeof(K)> r(S, K);


myprosql::SQL sql;

// static int callback(void *NotUsed, int argc, char **argv, char **azColName){
//   int i;
//   for(i=0; i<argc; i++){
//     cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL" ) << endl;
//   }
//   printf("\n");
//   return 0;
// }




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

            
            socket_.async_read_some(boost::asio::buffer(message, 50),
                    [this, self](boost::system::error_code ec, std::size_t length)
                    {
                        
                        
                        r.calculate(message, length);   //Расшифровываем полученное от клиента сообщение

                        sql.writeEvent(message, length);
                        if (!ec)
                        {
                            message[length]=0;
                            if(strcmp(message,"poll")==0){
                                cout << "send" << endl;
                                do_write(length);
                            }else if(strcmp(message,"regme")){

                            }else{
                                cout << "default" << endl;
                            }
                            cout << message << endl;
                        
                            
                        }
                    });
        }

        void do_write(std::size_t length)
        {
            auto self(shared_from_this());
            

            std::ifstream myfile;
            myfile.open ("example.txt");
            myfile.seekg(0, myfile.end);
            int lengthfile = myfile.tellg();
            myfile.seekg(0, myfile.beg);

            
            int countread = lengthfile%1024;
            myfile.read(data_, countread);

            
            r.calculate(data_, countread );

            int* cc=&lengthfile;
            socket_.write_some(boost::asio::buffer(cc,sizeof(lengthfile)));
            socket_.write_some(boost::asio::buffer(data_, countread));
            
            for(int i=0; i<lengthfile/1024; i++){           
                myfile.read(data_, 1024);
                r.calculate(data_, 1024);
                socket_.write_some(boost::asio::buffer(data_, 1024));
            }
            cout << myfile.gcount() << endl;
            myfile.close();
            socket_.write_some(boost::asio::buffer("e", sizeof(lengthfile)));
            //boost::asio::async_write(socket_, boost::asio::buffer("end date", length),
            //        [this, self](boost::system::error_code ec, std::size_t length_)
            //        {
            //            if (!ec)
            //            {
            //                do_read();
            //            }
            //        });
           // cout << "-start" << endl;
            //socket_.write_some(boost::asio::buffer(datafile, length));
           // cout << "-end" << endl;
        }

        tcp::socket socket_;
        enum { max_length = 4024 };
        char data_[max_length];
        char message[51];
        
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
            acceptor_.async_accept(socket_,
                    [this](boost::system::error_code ec)
                    {
                        if (!ec)
                        {
                            std::make_shared<session>(std::move(socket_))->start();
                        }

                    do_accept();
                    });
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

        // sqlite3 *db;
        // char *zErrMsg = 0;
        // int rc;

        // rc = sqlite3_open_v2("mybase", &db,  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        // if( rc ){
        //   cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
        //   sqlite3_close(db);
        //   return(1);
        // }


       
        // std::vector<string> vRequest {R"(
        //             CREATE TABLE IF NOT EXISTS COMPANY(
        //                ID INTEGER PRIMARY KEY   AUTOINCREMENT,
        //                TIME             text,
        //                EVENTS           CHAR(50)
        //             );)", 
        //             R"( INSERT INTO COMPANY (TIME, EVENTS)
        //                 VALUES (DATETIME('now','localtime'),'start server');)"};


        // for (auto i: vRequest) {
        //     rc = sqlite3_exec(db, i.c_str(), 0, 0, &zErrMsg);
        //     if( rc!=SQLITE_OK ){
        //       cout << "Can't open database: " << zErrMsg << endl;
        //       sqlite3_free(zErrMsg);
        //     }
        // }

        boost::asio::io_service io_service;

        server s(io_service, std::atoi(argv[1]));
        io_service.run();
        // 
        cout << "exit" << endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

