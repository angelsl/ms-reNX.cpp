#include "mmap.hpp"
#if defined RENXCPP_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined RENXCPP_LNX
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#endif

reNX::MemoryMappedFile::MemoryMappedFile(const char* filename) {
#if defined RENXCPP_WIN
    _ptr = 0;
    if ((_file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE ||
        (_mapping = CreateFileMappingA(_file, NULL, PAGE_READONLY, 0, 0, NULL)) == NULL ||
        (_ptr = reinterpret_cast<char *>(MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0))) == 0)
        return;
#elif defined RENXCPP_LNX
	_ptr = const_cast<const char *>(reinterpret_cast<char *>(MAP_FAILED));
	struct stat finfo;
	if((_file = open(filename, O_RDONLY)) == -1 ||
		fstat(_file, &finfo) == -1 ||
		(_size = finfo.st_size) < 1 || 
		(_ptr = const_cast<const char *>(reinterpret_cast<char *>(mmap(nullptr, _size, PROT_READ, MAP_SHARED, _file, 0)))) == MAP_FAILED) return;
#endif
}

reNX::MemoryMappedFile::~MemoryMappedFile() {
#if defined RENXCPP_WIN
    if (_ptr != 0) UnmapViewOfFile(_ptr);
    if (_mapping != NULL) CloseHandle(_mapping);
    if (_file != INVALID_HANDLE_VALUE) CloseHandle(_file);
#elif defined RENXCPP_LNX
	if(_ptr != const_cast<const char *>(reinterpret_cast<char *>(MAP_FAILED))) munmap(reinterpret_cast<void *>(const_cast<char *>(_ptr)), _size);
	if(_file != -1) close(_file);
#endif
}
