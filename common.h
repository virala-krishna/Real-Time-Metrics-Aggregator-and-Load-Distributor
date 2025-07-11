// common.h
#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <netinet/in.h>

// Config
#define AGGREGATOR_PORT 9090
#define MAX_AGENTS 10
#define BUFFER_SIZE 1024

// Agent data struct
struct Metrics {
    float cpu_usage;      // In percentage
    float mem_usage;      // In MB
    int agent_id;         // To identify agent
};

#endif

