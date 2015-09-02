#pragma once
#ifdef _WIN32
#define RENXCPP_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined __linux__
#define RENXCPP_LNX
#include <sys/types.h>
#else
#error Unsupported platform.
#endif

#ifndef RENXCPPMMAPH
#define RENXCPPMMAPH

namespace reNX {
    class MemoryMappedFile {
        const char* _ptr;
#if defined RENXCPP_WIN
        HANDLE _file;
        HANDLE _mapping;
#elif defined RENXCPP_LNX
		int _file;
		off_t _size;
#endif
    public:
        MemoryMappedFile(const char* filename);
        ~MemoryMappedFile();

        inline const char* base() const {
            return _ptr;
        }
    };
}
#endif
