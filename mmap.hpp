#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef RENXCPPMMAPH
#define RENXCPPMMAPH
namespace reNX {
	class MemoryMappedFile {
		char *_ptr;

		HANDLE _file;
		HANDLE _mapping;
	public:
		MemoryMappedFile(const char *filename);
		~MemoryMappedFile();
		char *base() { return _ptr; }
	};
}
#endif