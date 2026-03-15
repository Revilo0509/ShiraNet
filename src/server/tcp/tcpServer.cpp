#include "tcpServer.hpp"

#include "logger/logger.hpp"
#include <algorithm>

ShiraNet::Servers::TcpServer::TcpServer(int Domain, int Port, int MaxClients) {
    serverSocket = new ShiraNet::Sockets::TcpSocket(Domain);
    serverSocket->bind(Port);
    serverSocket->listen(MaxClients);
}

ShiraNet::Servers::TcpServer::~TcpServer() {
    delete serverSocket;
}

void ShiraNet::Servers::TcpServer::removeClient(std::shared_ptr<ShiraNet::Sockets::TcpSocket> Client) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    clients.erase(std::remove(clients.begin(), clients.end(), Client), clients.end());
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
}

void ShiraNet::Servers::TcpServer::getConnection(ClientHandlerCallback Callback) {
    auto newSocket = serverSocket->getClientConnection();

    if (!newSocket.isSocketValid())
        return;

    auto client = std::make_shared<ShiraNet::Sockets::TcpSocket>(std::move(newSocket));

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(client);
    }

    std::thread(Callback, client, this).detach(); // todo: keep track of threads instead
}

void ShiraNet::Servers::TcpServer::getConnections() {
    serverThread = std::thread([this]() {
        while (true) {
            this->getConnection();
        }
    });
}

void ShiraNet::Servers::TcpServer::getConnections(ClientHandlerCallback Callback) {
    serverThread = std::thread([this, Callback]() {
        while (true) {
            this->getConnection(Callback);
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

void ShiraNet::Servers::TcpServer::sendToAllExcept(ShiraNet::NetworkData::Message Message, std::shared_ptr<ShiraNet::Sockets::TcpSocket> Client) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        if (client == Client)
            continue;
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