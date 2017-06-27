#include <iostream>
#include <SFML/Network.hpp>
#include <string>

using std::cout;
using std::endl;
using std::string;


static bool port_is_open(const std::string& address, int port)
{
    return (sf::TcpSocket().connect(address, port) == sf::Socket::Done);
}

int main()
{
    std::cout << "Port 80 : ";
    if (port_is_open("localhost", 80))
        std::cout << "OPEN" << std::endl;
    else
        std::cout << "CLOSED" << std::endl;
    return 0;
}