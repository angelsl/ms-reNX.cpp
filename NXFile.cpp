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
	const HeaderData *h = reinterpret_cast<const HeaderData *>(_mmap->base());
	if(h->PKG4 != 0x34474B50) throw "Invalid magic.";
	_stable = reinterpret_cast<const uint64_t *>(_mmap->base() + h->StringBlock);
	_ntable = reinterpret_cast<const NodeData *>(_mmap->base() + h->NodeBlock);
	_nodes = new NXNode[h->NodeCount];
	uint32_t nr = h->NodeCount - 1;
	for(NXNode *node = _nodes + nr; node >= _nodes; --node, --nr)
	{
		node->_data = _ntable + nr;
		node->_file = this;
	}
}

reNX::NXFile::~NXFile() {
	delete[] _nodes;
	delete _mmap;
}