// #ifdef __linux__
// #ifdef _WIN32
// #if defined __APPLE__ && defined __MACH__

#include "mmap.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

reNX::MemoryMappedFile::MemoryMappedFile(const char *filename) {
	_ptr = 0;
	if((_file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE ||
		(_mapping = CreateFileMapping(_file, NULL, PAGE_READONLY, 0, 0, NULL)) == NULL ||
		(_ptr = reinterpret_cast<char *>(MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0))) == 0) return;
}

reNX::MemoryMappedFile::~MemoryMappedFile() {
	if(_ptr != 0) UnmapViewOfFile(_ptr);
	if(_mapping != NULL) CloseHandle(_mapping);
	if(_file != INVALID_HANDLE_VALUE) CloseHandle(_file);
}