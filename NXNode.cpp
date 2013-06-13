#include "reNX.cpp.hpp"
#include "NXNode.hpp"
#include <unordered_map>

reNX::NXNode *reNX::NXNode::_nullnode = new NXNode(nullptr);

bool reNX::NXNode::const_iterator::operator ==(const reNX::NXNode::const_iterator& n) const {
	return n._actual == _actual;
}

bool reNX::NXNode::const_iterator::operator !=(const reNX::NXNode::const_iterator& n) const {
	return n._actual != _actual;
}

reNX::NXNode::const_iterator& reNX::NXNode::const_iterator::operator ++() {
	++_actual;
	return *this;
}

reNX::NXNode::const_iterator reNX::NXNode::const_iterator::operator ++(int) {
	auto old = *this;
	++_actual;
	return old;
}

reNX::NXNode::const_iterator::reference reNX::NXNode::const_iterator::operator *() const {
	return *_actual->second->_wrapper;
}

reNX::NXNode::const_iterator::pointer reNX::NXNode::const_iterator::operator ->() const {
	return _actual->second->_wrapper;
}

std::string reNX::NXNode::name() const {
	return _node->_file->get_string(_node->_data->NodeNameID);
}

const reNX::NXFile *const reNX::NXNode::file() const {
	return const_cast<const NXFile *const>(_node->_file);
}

const reNX::NXNode reNX::NXNode::parent() const {
	return *(_node->_parent != nullptr ? _node->_parent->_wrapper : _nullnode);
}

reNX::NXDataType reNX::NXNode::type() const {
	return static_cast<NXDataType>(_node->_data->Type);
}

size_t reNX::NXNode::size() const {
	return _node->size();
}

uint32_t reNX::NXNode::length() const {
	switch(type()) {
		case canvas:
		case mp3:
			return reinterpret_cast<NXValuedNode<char *> *>(_node)->length();
		default:
			return 0;
	}
}

bool reNX::NXNode::exists() const {
	return nullptr != _node;
}

const reNX::NXNode reNX::NXNode::operator [](const char *n) const {
	return child(n);
}

const reNX::NXNode reNX::NXNode::operator [](const ::std::string& n) const {
	return child(n);
}

const reNX::NXNode reNX::NXNode::child(const char *n) const {
	return child(std::string(n));
}

const reNX::NXNode reNX::NXNode::child(const ::std::string& n) const {
	const NXEmptyNode *r = _node->child(n);
	return *(r == nullptr ? _nullnode : r->_wrapper);
}

reNX::NXNode::const_iterator reNX::NXNode::begin() const {
	return const_iterator(_node->begin());
}

reNX::NXNode::const_iterator reNX::NXNode::end() const {
	return const_iterator(_node->end());
}

template <typename T> T reNX::NXNode::value() const {
	return reinterpret_cast<NXValuedNode<T> *>(_node)->value();
}

bool reNX::NXNode::operator ==(reNX::NXNode n) const {
	return n._node == _node;
}

bool reNX::NXNode::operator !=(reNX::NXNode n) const {
	return n._node != _node;
}

reNX::child_map reNX::NXEmptyNode::empty_map = reNX::child_map();

reNX::NXEmptyNode *reNX::NXEmptyNode::parse_node(char *p, NXEmptyNode *pn, NXFile *f) {
	NodeData *n = reinterpret_cast<NodeData *>(p);
	switch(static_cast<NXDataType>(n->Type)) {
		case nothing:
			return create_node(n, pn, f);
		case int64:
			return create_parent_node<int64_t>(n, pn, f);
		case fpoint:
			return create_parent_node<double>(n, pn, f);
		case string:
			return create_parent_node<std::string>(n, pn, f);
		case vector:
			return create_parent_node<point>(n, pn, f);
		case canvas:
		case mp3:
			// TODO
			return create_node(n, pn, f);
		default:
			throw "Invalid node type.";
	}
}

reNX::NXEmptyNode *reNX::NXEmptyNode::create_node(NodeData * n, NXEmptyNode *pn, NXFile *f)
{
	if(n->ChildCount > 0) return new NXParentNode(n, pn, f);
	return new NXEmptyNode(n, pn, f);
}

template <typename T> reNX::NXEmptyNode *reNX::NXEmptyNode::create_parent_node(NodeData * n, NXEmptyNode *pn, NXFile *f)
{
	if(n->ChildCount > 0) return reinterpret_cast<NXEmptyNode *>(new NXValuedParentNode<T>(n, pn, f));
	return reinterpret_cast<NXEmptyNode *>(new NXValuedNode<T>(n, pn, f));
}

size_t reNX::NXEmptyNode::size() const {
	return 0;
}

const reNX::NXEmptyNode *reNX::NXEmptyNode::child(const ::std::string&) {
	return nullptr;
}
reNX::child_map::const_iterator reNX::NXEmptyNode::begin() {
	return empty_map.cbegin();
}

reNX::child_map::const_iterator reNX::NXEmptyNode::end() {
	return empty_map.cend();
}

template <> int64_t reNX::NXValuedNode<int64_t>::value() const {
	return _data->Type1;
}

template <> double reNX::NXValuedNode<double>::value() const {
	return _data->Type2;
}

template <> std::string reNX::NXValuedNode<std::string>::value() const {
	return _file->get_string(_data->Type3);
}

template <> reNX::point reNX::NXValuedNode<reNX::point>::value() const {
	return point(_data->Type4.X, _data->Type4.Y);
}

template <> char *reNX::NXValuedNode<char *>::value() const {
	// TODO
	return nullptr;
}

template <typename T> T reNX::NXValuedNode<T>::value() const {
	throw "Invalid node type.";
}

template <> uint32_t reNX::NXValuedNode<char *>::length() const {
	switch(static_cast<NXDataType>(_data->Type)) {
		case canvas:
		case mp3:
			// TODO
			return 0;
		default:
			throw "Invalid node type.";
	}
}

template <typename T> uint32_t reNX::NXValuedNode<T>::length() const {
	return 0;
}

void reNX::NXParentNode::parse_children() {
	for(uint32_t n = 0; n < _data->ChildCount; ++n)
	{
		NodeData *p = _file->_base->_data + _data->FirstChildID + n;
		_children.insert(std::make_pair<std::string, NXEmptyNode *>(_file->get_string(p->NodeNameID), NXEmptyNode::parse_node(reinterpret_cast<char *>(p), this, _file)));
	}
}

reNX::NXParentNode::NXParentNode(NodeData *p, NXEmptyNode *parent, NXFile *f) : reNX::NXEmptyNode(p, parent, f) {
	if(p->ChildCount == 0) throw "NXParentNode constructor called for node with 0 children!";
	_children.reserve(p->ChildCount);
}

reNX::NXParentNode::~NXParentNode() {
	for(auto i : _children)
		delete i.second;
}

size_t reNX::NXParentNode::size() const {
	return _data->ChildCount;
}

const reNX::NXEmptyNode *reNX::NXParentNode::child(const std::string& n) {
	if(_children.size() == 0) parse_children();
	auto got = _children.find(n);
	return got == _children.end() ? nullptr : got->second;
}