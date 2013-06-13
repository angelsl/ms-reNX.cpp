#include <stdint.h>
#include <string>
#include <unordered_map>
#include <iterator>
#include "mmap.hpp"

#ifdef RENXCPP_EXPORTS
#define RENXCPP_API __declspec(dllexport)
#else
#define RENXCPP_API __declspec(dllimport)
#endif

#ifndef RENXCPPH
#define RENXCPPH
namespace reNX {
	class RENXCPP_API NXNode;
	class RENXCPP_API NXFile;
	enum RENXCPP_API NXDataType;
	struct NodeData;

	typedef std::pair<int32_t, int32_t> point;

	class RENXCPP_API NXFile {
		NXNode *_nodes;
		MemoryMappedFile *_mmap;
		void parse();	
		std::string get_string(uint32_t) const;
	public:
		NXFile(const char *fn);
		~NXFile();
		const NXNode& base() const;

		friend class NXNode;
		friend class NXNode;
	};

	class RENXCPP_API NXNode {
		NXFile *_file;
		NodeData *_data;
	public:
		std::string name() const;
		NXDataType type() const;
		size_t size() const;
		
		template <typename T> T value() const;
		uint32_t length() const;

		const NXNode &operator[] (const char *) const;
		const NXNode &operator[] (const ::std::string&) const;
		const NXNode &child(const char *) const;
		const NXNode &child(const ::std::string&) const;

		NXNode const *begin() const;
		NXNode const *end() const;
		
		bool operator ==(NXNode n) const { return n._data == _data && n._file == _file; }
		bool operator !=(NXNode n) const { return !(*this == n); }

		friend class NXFile;
	};

	enum RENXCPP_API NXDataType {
		nothing, int64, fpoint, string, vector, canvas, mp3
	};
}
#endif
