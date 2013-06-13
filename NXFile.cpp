#include "reNX.cpp.hpp"
#include <stdint.h>
#include "mmap.hpp"

namespace reNX {
#pragma pack(push, 4)
	struct HeaderData {
		uint32_t PKG4;
		uint32_t NodeCount;
		uint64_t NodeBlock;
		uint32_t StringCount;
		uint64_t StringBlock;
		uint32_t BitmapCount;
		uint64_t BitmapBlock;
		uint32_t SoundCount;
		uint64_t SoundBlock;
	};
#pragma pack(pop)
}

reNX::NXFile::NXFile(const char *n) {
	_mmap = new MemoryMappedFile(n);
	parse();
}

reNX::NXFile::~NXFile() {
	delete[] _nodes;
	delete _mmap;
}

const reNX::NXNode& reNX::NXFile::base() const {
	return *_nodes;
}

void reNX::NXFile::parse() {
	HeaderData* h = reinterpret_cast<HeaderData *>(_mmap->base());
	if(h->PKG4 != 0x34474B50) throw "Invalid magic.";
	_nodes = new NXNode[h->NodeCount];
	_nodes->_data = reinterpret_cast<NodeData *>(_mmap->base() + *reinterpret_cast<uint64_t *>(_mmap->base() + reinterpret_cast<HeaderData *>(_mmap->base())->NodeBlock));
	_nodes->_file = this;
}

std::string reNX::NXFile::get_string(uint32_t id) const {
	char *p = _mmap->base() + *reinterpret_cast<uint64_t *>(_mmap->base() + reinterpret_cast<HeaderData *>(_mmap->base())->StringBlock + id*8);
	return std::string(p + 2, *reinterpret_cast<uint16_t *>(p));
}