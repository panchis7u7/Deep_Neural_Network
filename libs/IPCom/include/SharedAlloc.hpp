#pragma once

#include "../../platform.hpp"
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>

template <typename T>
LIBEXP T* shalloc(const std::string& shmem_name, std::string& err_message ) {
    int fd = -1;
    if((fd = shm_open(shmem_name.c_str(), O_CREAT | O_RDWR, 0666)) < 0) {
        err_message = "Open failed.";
        return nullptr;
    }

    if(ftruncate(fd, sizeof(T)) < 0) {
        err_message = "Truncate failed.";
        return nullptr;
    }

    T* shmem = nullptr;
    if ((shmem = (T*)mmap(NULL, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        err_message = "mmap failed.";
        return nullptr;
    }

    return shmem;
}