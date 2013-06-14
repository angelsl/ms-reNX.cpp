#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef RENXCPPMMAPH
#define RENXCPPMMAPH
namespace reNX {
	class MemoryMappedFile {
		const char *_ptr;

		HANDLE _file;
		HANDLE _mapping;
	public:
		MemoryMappedFile(const char *filename);
		~MemoryMappedFile();
		inline const char *base() const { return _ptr; }
	};
}
#endif