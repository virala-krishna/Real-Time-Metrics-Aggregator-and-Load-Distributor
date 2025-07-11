// agent.cpp
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include "common.h"

using namespace std;

float getCPUUsage() {
    ifstream stat("/proc/stat");
    string line;
    getline(stat, line);
    stat.close();

    unsigned long long user, nice, system, idle;
    sscanf(line.c_str(), "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
    static unsigned long long prevIdle = 0, prevTotal = 0;

    unsigned long long idleTime = idle;
    unsigned long long totalTime = user + nice + system + idle;

    float cpu = 0.0;
    if (prevTotal != 0) {
        cpu = 100.0 * (1.0 - (float)(idleTime - prevIdle) / (totalTime - prevTotal));
    }

    prevIdle = idleTime;
    prevTotal = totalTime;

    return cpu;
}

float getMemoryUsage() {
    ifstream meminfo("/proc/meminfo");
    string label;
    long total, free;
    meminfo >> label >> total;
    meminfo >> label >> free;
    meminfo.close();
    return (total - free) / 1024.0; // in MB
}

void runAgent(const string& server_ip, int agent_id) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket creation failed.\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(AGGREGATOR_PORT);
    inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connection to aggregator failed.\n";
        return;
    }

    cout << "[Agent " << agent_id << "] Connected to Aggregator at " << server_ip << endl;

    while (true) {
        Metrics m;
        m.cpu_usage = getCPUUsage();
        m.mem_usage = getMemoryUsage();
        m.agent_id = agent_id;

        send(sock, &m, sizeof(Metrics), 0);

        cout << "[Agent " << agent_id << "] Sent - CPU: " << m.cpu_usage
             << "% | MEM: " << m.mem_usage << "MB" << endl;

        sleep(2); // Send every 2 seconds
    }

    close(sock);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./agent <aggregator_ip> <agent_id>\n";
        return 1;
    }

    string aggregator_ip = argv[1];
    int agent_id = stoi(argv[2]);

    runAgent(aggregator_ip, agent_id);

    return 0;
}

