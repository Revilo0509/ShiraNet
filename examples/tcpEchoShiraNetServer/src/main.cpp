#include <ShiraNet.hpp>

#include <cstring>
#include <iostream>

#define BUFFERSIZE 1024

void handleClient(std::shared_ptr<ShiraNet::Sockets::TcpSocket> Client, ShiraNet::Servers::TcpServer* Server) {
    ShiraNet::NetworkData::Message message = Client->receiveMessage();
    Client->send(message);
}

void server() {
    ShiraNet::Servers::TcpServer server(AF_INET, 57942, 5, handleClient); // this is not the best example for servers since this handles one client at a time
    while (true) {
        server.getConnection();
        std::cout << server.getClient(0)->getAddressInfoToStringIP() << std::endl;
    }
}

void client() {
    ShiraNet::Sockets::TcpSocket socket(AF_INET);
    socket.connect("localhost", 57942);

    std::cout << "Enter data to send\n";
    std::string dataToSend;
    std::cin.ignore();
    std::getline(std::cin, dataToSend);
    dataToSend += "\n";

    ShiraNet::NetworkData::Message messageToSend(0, dataToSend);
    socket.send(messageToSend);
    ShiraNet::NetworkData::Message receiveMessage = socket.receiveMessage();
    std::cout << receiveMessage.payload;
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
