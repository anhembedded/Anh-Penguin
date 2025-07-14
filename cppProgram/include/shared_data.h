#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <cstdint>

struct SharedData {
    uint32_t counter {};
    char message[128];
};

const char* SHM_NAME = "/app_shm";

#endif