#include <ShiraNet.hpp>

#include <cstring>
#include <iostream>

#define BUFFERSIZE 1024

void server() {
    ShiraNet::Sockets::TcpSocket socket(AF_INET);
    socket.bind(57942);
    socket.listen(5);
    while (true) {
        ShiraNet::Sockets::TcpSocket client = socket.getClientConnection();
        std::cout << client.getAddressInfoToStringIP() << std::endl;
        ShiraNet::NetworkData::Message messageToSend(0, client.receiveMessage().payload);
        client.send(messageToSend);
    }
}

void client() {
    ShiraNet::Sockets::TcpSocket socket(AF_INET);
    socket.connect("127.0.0.1", 57942);

    std::cout << "Enter data to send\n";
    std::string dataToSend;
    std::cin.ignore();
    std::getline(std::cin, dataToSend);
    dataToSend += "\n";

    ShiraNet::NetworkData::Message messageToSend(0, dataToSend);

    socket.send(messageToSend);
    ShiraNet::NetworkData::Message message = socket.receiveMessage();
    std::cout << message.payload;
}

int main() {
    int i = 0;
    std::cout << "Act as server [1], Act as client [2]\n";
    std::cin >> i;

    if (i == 1) {
        std::cout << "Acting as server\n";
        server();
    } else if (i == 2) {
        std::cout << "Acting as client\n";
        client();
    } else {
        return 0;
    }

    return 0;
}
