#pragma once

#include "../../sockets/tcp/TcpSocket.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// Servers are classes containing sockets with a generic implementation of accepting clients and keeping track of them, this is a higher level part of the library and if you do not like the Server implementation you can use xSocket.hpp and create your own implementation
namespace ShiraNet::Servers {

    class TcpServer {
    private:
        using ClientHandler = std::function<void(std::shared_ptr<ShiraNet::Sockets::TcpSocket>, ShiraNet::Servers::TcpServer*)>;

        ClientHandler clientHandler;
        ShiraNet::Sockets::TcpSocket* serverSocket = nullptr;
        std::vector<std::shared_ptr<ShiraNet::Sockets::TcpSocket>> clients;
        std::mutex clientsMutex;
        std::thread serverThread;

    public:
        // creates the servers socket, binds it and listens
        TcpServer(int Domain, int Port, int MaxClients, ClientHandler Handler);
        ~TcpServer();

        void getConnection();  // Accepts any incomming connection, adds it's socket to the clients list
        void getConnections(); // Accepts any incomming connection in a loop run on the serverThread;
        std::shared_ptr<ShiraNet::Sockets::TcpSocket> getClient(int i);
        void sendToAll(ShiraNet::NetworkData::Message Message);
    };

}