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
#pragma pack(push, 2)
	struct NodeData {
        uint32_t NodeNameID;
        uint32_t FirstChildID;
        uint16_t ChildCount;
        uint16_t Type;
		union {
			int64_t Type1;
			double Type2;
			uint32_t Type3;
			struct {
				int32_t X;
				int32_t Y;
			} Type4;
			struct {
				uint32_t ID;
				uint16_t W;
				uint16_t H;
			} Type5;
			struct {
				uint32_t ID;
				uint32_t Len;
			} Type6;
		};
	};
#pragma pack(pop)

	typedef std::pair<int32_t, int32_t> point;

	class RENXCPP_API NXFile {
		const uint64_t *_stable;
		NXNode *_nodes;
		MemoryMappedFile *_mmap;
		void parse();	
		inline std::string get_string(uint32_t id) const { const char *p = _mmap->base() + _stable[id]; return std::string(p + 2, *reinterpret_cast<const uint16_t *>(p)); }
	public:
		NXFile(const char *fn);
		~NXFile();
		inline const NXNode& base() const { return *_nodes; }

		friend class NXNode;
		friend class NXNode;
	};

	class RENXCPP_API NXNode {
		static NXNode *_nullnode;
		const NXFile *_file;
		const NodeData *_data;
		inline NXNode() {}
		inline NXNode(int) { _file = nullptr; _data = nullptr; }
	public:
		inline std::string name() const { return _file->get_string(_data->NodeNameID); }
		inline NXDataType type() const { return static_cast<NXDataType>(_data->Type); }
		inline size_t size() const { return _data->ChildCount; }
		inline bool exists() const { return _data != nullptr; }
		
		template <typename T> T value() const;
		uint32_t length() const;

		inline const NXNode &operator[] (const char *n) const { return child(std::string(n)); }
		inline const NXNode &operator[] (const ::std::string& n) const { return child(n); }
		inline const NXNode &child(const char *n) const { return child(std::string(n)); }
		const NXNode &child(const ::std::string&) const;

		inline const NXNode *begin() const { return _file->_nodes + _data->FirstChildID; }
		inline const NXNode *end() const { return _file->_nodes + _data->FirstChildID + _data->ChildCount; }
		
		bool operator ==(NXNode n) const { return n._data == _data && n._file == _file; }
		bool operator !=(NXNode n) const { return !(*this == n); }

		friend class NXFile;
	};

	enum RENXCPP_API NXDataType {
		nothing, int64, fpoint, string, vector, canvas, mp3
	};
}
#endif
