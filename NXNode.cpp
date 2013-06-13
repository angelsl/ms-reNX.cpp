#include "reNX.cpp.hpp"

namespace reNX {
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
}

size_t reNX::NXNode::size() const {
	return _data->ChildCount;
}

const reNX::NXNode& reNX::NXNode::operator[] (const char *n) const {
	return child(n);
}

const reNX::NXNode& reNX::NXNode::operator[] (const ::std::string& n) const {
	return child(n);
}

const reNX::NXNode& reNX::NXNode::child(const char *n) const {
	return child(std::string(n));
}

const reNX::NXNode& reNX::NXNode::child(const ::std::string& n) const {
	// TODO: Initialise children
	throw "Not implemented";
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

reNX::NXNode *const reNX::NXNode::begin() const {
	throw "Not implemented"; // TODO: Initialise children
	return _file->_nodes + _data->FirstChildID;
}

reNX::NXNode *const reNX::NXNode::end() const {
	throw "Not implemented"; // TODO: Initialise children
	return _file->_nodes + _data->FirstChildID + _data->ChildCount;
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