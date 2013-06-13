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
	class NXEmptyNode;
	
	class StringHash {
	public:
		uint32_t operator() (const std::string &s) const
		{
			uint32_t hash = 0;
			for(UINT i = 0; i < s.length(); ++i)
				hash = 101 * hash + s[i];
			return hash ^ (hash >> 16);
		}
	};

	typedef std::unordered_map<std::string, reNX::NXEmptyNode *, StringHash> child_map;
	typedef std::pair<int32_t, int32_t> point;

	class RENXCPP_API NXNode final {
		static NXNode *_nullnode;
		NXEmptyNode *_node;
		NXNode(NXEmptyNode *node) : _node(node) {}

	public:
		struct RENXCPP_API const_iterator : std::iterator<std::forward_iterator_tag, const NXNode> {
			child_map::const_iterator _actual;
			const_iterator(child_map::const_iterator n) : _actual(n) {}

		public:
			bool operator ==(const const_iterator&) const;
			bool operator !=(const const_iterator&) const;

			const_iterator& operator ++();
			const_iterator operator ++(int);

			reference operator *() const;
			pointer operator ->() const;

			friend class NXNode;
		};

		std::string name() const;
		const NXFile *const file() const;
		const NXNode parent() const;
		NXDataType type() const;
		size_t size() const;
		uint32_t length() const;

		bool exists() const;

		const NXNode operator [](const char *n) const;
		const NXNode operator [](const ::std::string&) const;
		const NXNode child(const char *n) const;
		const NXNode child(const ::std::string&) const;

		const_iterator begin() const;
		const_iterator end() const;

		template <typename T> T value() const;

		bool operator ==(NXNode n) const;
		bool operator !=(NXNode n) const;
		
		friend class NXEmptyNode;

	};

	class RENXCPP_API NXFile {
		NXEmptyNode * _base;
		MemoryMappedFile* _mmap;
		void parse();	
		std::string get_string(uint32_t) const;
	public:
		NXFile(const char * fn);
		~NXFile();
		const NXNode base() const;

		friend class NXNode;
		friend class NXEmptyNode;
		template<typename T> friend class NXValuedNode;
		friend class NXParentNode;
	};

	enum RENXCPP_API NXDataType {
		nothing, int64, fpoint, string, vector, canvas, mp3
	};
}
#endif
