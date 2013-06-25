#include "reNX.cpp.hpp"

template <typename T> inline int s(T val) {
    return (T(0) < val) - (val < T(0));
}

inline int nncmp(const uint16_t *ll, const char *r, uint16_t rl) {
	int memcmp = std::memcmp(reinterpret_cast<const char *>(ll) + 2, r, (*ll) < rl ? (*ll) : rl);
	return s(memcmp == 0 ? (*ll)-rl : memcmp);
}

namespace reNX {
	NXNode *NXNode::_nullnode = new NXNode(0);

	template <> int64_t NXNode::value<int64_t>() const {
		return _data->Type1;
	}
	
	template <> double NXNode::value<double>() const {
		return _data->Type2;
	}
	
	template <> std::string NXNode::value<std::string>() const {
		return _file->get_string(_data->Type3);
	}
	template <> nxstring NXNode::value<nxstring>() const {
		return _file->get_nxstring(_data->Type3);
	}
	template <> point NXNode::value<point>() const {
		return point(_data->Type4.X, _data->Type4.Y);
	}
	
	template <> char *NXNode::value<char *>() const {
		// TODO
		return nullptr;
	}

	uint32_t NXNode::length() const {
		switch(static_cast<NXDataType>(_data->Type)) {
			case canvas:
			case mp3:
				// TODO
				return 0;
			default:
				throw "Invalid node type.";
		}
		return 0;
	}
	
	const NXNode& NXNode::child(const char *n, uint16_t nl) const {
		uint32_t l = _data->FirstChildID;
		uint32_t m = _data->FirstChildID+_data->ChildCount-1;
		while (m >= l) {
			uint32_t c = l + ((m - l) / 2);
			switch(nncmp(reinterpret_cast<const uint16_t *>(_file->_mmap->base() + _file->_stable[_file->_nodes[c]._data->NodeNameID]), n, nl)) {
				case -1:
					l = c+1;
					break;
				case 1:
					m = c-1;
					break;
				case 0:
					return _file->_nodes[c];
			}
		}
		return *_nullnode;
	}
}
