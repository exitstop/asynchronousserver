#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/atomic.hpp>

using boost::asio::ip::tcp;

using namespace std;

class tcp_client
{
public:
    tcp_client(boost::asio::io_service& io_service, tcp::endpoint& endpoint, long long timeout = 3000000) 
        :m_io_service (io_service),
        m_endpoint(endpoint),
        m_timer(io_service),
        m_timeout(timeout)
    {
        connect();
    }

    void stop()
    {
        m_socket->close();
    }

private:

    void connect()
    {
        m_socket.reset(new tcp::socket(m_io_service));

        std::cout << "TCP Connection in progress" << std::endl;
        m_socket->async_connect(m_endpoint,
            boost::bind(&tcp_client::handle_connect, this,
            m_socket,
            boost::asio::placeholders::error)
            );
        cout << "async_connect" << endl;

        m_timer.expires_from_now(boost::posix_time::microseconds(m_timeout));
        m_timer.async_wait(boost::bind(&tcp_client::HandleWait, this,m_socket, boost::asio::placeholders::error));
    }

    void handle_connect(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << socket->remote_endpoint().port() << " TCP Connection : connected !" << std::endl;
            m_timer.expires_at(boost::posix_time::pos_infin); // Stop the timer !
            // Read normally
        }
        else
        {
            std::cout << socket->remote_endpoint().port() << " BAD!" << std::endl;
        }
    }


public:
    void HandleWait(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << socket->remote_endpoint().port() << "Connection not established..." << std::endl;
            std::cout << "Trying to close socket..." << std::endl;
            stop();
            return;
        }
    }

    boost::asio::io_service&        m_io_service;
    boost::shared_ptr<tcp::socket>  m_socket;
    tcp::endpoint                   m_endpoint;
    boost::asio::deadline_timer     m_timer;
    long long                       m_timeout;
};


#define CENDL 150
// static std::atomic<int>      x;

void connect_handler(boost::asio::ip::tcp::socket *socket, boost::asio::deadline_timer  *m, boost::asio::ip::tcp::endpoint *e, int port, const boost::system::error_code& error)
{
    if (!error)
    {
        m->expires_at(boost::posix_time::pos_infin); // Stop the timer !
        socket->async_write_some(boost::asio::buffer("hello", sizeof("hello")),
                [&](boost::system::error_code ec, std::size_t length){});

        char hi[5];
        socket->read_some( boost::asio::buffer(&hi,sizeof(hi)));
        cout << hi << endl;        
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

void HandleWait( boost::asio::ip::tcp::socket *socket, const boost::system::error_code& error)
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


int main()
{


        // boost::asio::io_service io_service;
        // tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 2014); // invalid address
        // tcp_client tcpc(io_service, endpoint);
        // io_service.run();

    

    
    
    
    
        // boost::asio::ip::tcp::socket sock2(service);
    // std::vector<std::shared_ptr<boost::asio::ip::tcp::endpoint> > v;
    // boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string("127.0.0.1"), 2014); 
    // boost::asio::ip::tcp::socket sock(service);    
    // sock.async_connect(ep, boost::bind(&connect_handler, 2014, boost::asio::placeholders::error));

    // boost::asio::ip::tcp::endpoint ep2( boost::asio::ip::address::from_string("127.0.0.1"), 2015); 
    // boost::asio::ip::tcp::socket sock2(service);    
    // sock2.async_connect(ep2, boost::bind(&connect_handler, 2015, boost::asio::placeholders::error));

    #define TIMEC 1000
    // vector<boost::asio::ip::tcp::socket> vsocket; 

    boost::asio::io_service *service[50];

    for(int a=1; a<50; a++){  /* 65535 */

        service[a] = new boost::asio::io_service();


        for(int i=a*1000; i < (a*1000+1000); i++){
            boost::asio::ip::tcp::socket *s = new boost::asio::ip::tcp::socket(*service[a]);
            boost::asio::ip::tcp::endpoint *e = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), i);
            // boost::asio::ip::tcp::socket sock2_(service);        

            boost::asio::deadline_timer  *m = new boost::asio::deadline_timer(*service[a]);
            s->async_connect(*e, boost::bind(&connect_handler, s, m, e, i, boost::asio::placeholders::error));           

            m->expires_from_now(boost::posix_time::microseconds(TIMEC));
            m->async_wait(boost::bind(HandleWait, s,boost::asio::placeholders::error));
        }
        service[a]->run();
    }

    // boost::asio::io_service service2;
    // for(int i=65000; i <= 65535 ; i++){
    //     boost::asio::ip::tcp::socket *s = new boost::asio::ip::tcp::socket(service2);
    //     boost::asio::ip::tcp::endpoint *e = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), i);
    //     // boost::asio::ip::tcp::socket sock2_(service);        

    //     boost::asio::deadline_timer  *m = new boost::asio::deadline_timer(service2);
    //     s->async_connect(*e, boost::bind(&connect_handler, s, m, e, i, boost::asio::placeholders::error));           

    //     m->expires_from_now(boost::posix_time::microseconds(TIMEC));
    //     m->async_wait(boost::bind(HandleWait, s,boost::asio::placeholders::error));
    // }
    //     service2.run();
   
    for(int a=1; a<50; a++){
        service[a]->stop();        
        delete service[a];        
    }
    // service2.stop();
// 
}