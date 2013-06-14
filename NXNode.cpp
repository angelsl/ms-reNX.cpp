#include "reNX.cpp.hpp"

template <typename T> inline int s(T val) {
    return (T(0) < val) - (val < T(0));
}

reNX::NXNode *reNX::NXNode::_nullnode = new reNX::NXNode(0);

const reNX::NXNode& reNX::NXNode::child(const ::std::string& n) const {
	uint32_t l = _data->FirstChildID;
	uint32_t m = _data->FirstChildID+_data->ChildCount-1;
	while (m >= l) {
		uint32_t c = l + ((m - l) / 2);
		switch(s(_file->_nodes[c].name().compare(n))) {
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

template <> int64_t reNX::NXNode::value<int64_t>() const {
	return _data->Type1;
}

template <> double reNX::NXNode::value<double>() const {
	return _data->Type2;
}

template <> std::string reNX::NXNode::value<std::string>() const {
	return _file->get_string(_data->Type3);
}

template <> reNX::point reNX::NXNode::value<reNX::point>() const {
	return point(_data->Type4.X, _data->Type4.Y);
}

template <> char *reNX::NXNode::value<char *>() const {
	// TODO
	return nullptr;
}

template <typename T> T reNX::NXNode::value() const {
	throw "Invalid node type.";
}

uint32_t reNX::NXNode::length() const {
	switch(static_cast<NXDataType>(_data->Type)) {
		case canvas:
		case mp3:
			// TODO
			return 0;
		default:
			throw "Invalid node type.";
	}
}