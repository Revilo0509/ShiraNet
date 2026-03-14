#include "tcpServer.hpp"

#include "logger/logger.hpp"

ShiraNet::Servers::TcpServer::TcpServer(int Domain, int Port, int MaxClients, ClientHandler Handler) {
    serverSocket = new ShiraNet::Sockets::TcpSocket(Domain);
    serverSocket->bind(Port);
    serverSocket->listen(MaxClients);
    clientHandler = Handler;
}

ShiraNet::Servers::TcpServer::~TcpServer() {
    delete serverSocket;
}

void ShiraNet::Servers::TcpServer::getConnection() {
    auto newSocket = serverSocket->getClientConnection();

    if (!newSocket.isSocketValid())
        return;

    auto client = std::make_shared<ShiraNet::Sockets::TcpSocket>(std::move(newSocket));

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(client);
    }

    std::thread(clientHandler, client, this).detach();
}

void ShiraNet::Servers::TcpServer::getConnections() {
    serverThread = std::thread([this]() {
        while (true) {
            this->getConnection();
        }
    });
}

void ShiraNet::Servers::TcpServer::sendToAll(ShiraNet::NetworkData::Message Message) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        try {
            client->send(Message);
        } catch (...) {
            Logger::info("some client failed in the sendToAll\n");
        }
    }
}

std::shared_ptr<ShiraNet::Sockets::TcpSocket> ShiraNet::Servers::TcpServer::getClient(int i) {
    return clients.at(i);
}