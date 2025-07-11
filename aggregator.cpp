// aggregator.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "common.h"

using namespace std;

void handleAgent(int client_socket) {
    Metrics m;

    while (true) {
        ssize_t bytes_received = recv(client_socket, &m, sizeof(Metrics), 0);
        if (bytes_received <= 0) {
            cerr << "[Aggregator] Lost connection to agent.\n";
            break;
        }

        cout << "[Aggregator] Agent " << m.agent_id
             << " → CPU: " << m.cpu_usage << "% | MEM: " << m.mem_usage << " MB" << endl;
    }

    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "[Aggregator] Failed to create socket.\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(AGGREGATOR_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "[Aggregator] Bind failed.\n";
        return 1;
    }

    if (listen(server_socket, MAX_AGENTS) < 0) {
        cerr << "[Aggregator] Listen failed.\n";
        return 1;
    }

    cout << "[Aggregator] Listening on port " << AGGREGATOR_PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        int client_socket = accept(server_socket, (sockaddr*)&clientAddr, &clientSize);

        if (client_socket >= 0) {
            cout << "[Aggregator] Agent connected.\n";
            thread(handleAgent, client_socket).detach();
        }
    }

    close(server_socket);
    return 0;
}

