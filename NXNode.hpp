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

	class NXEmptyNode {
		static child_map empty_map;
		static NXEmptyNode *parse_node(char *, NXEmptyNode *, NXFile *);
		static NXEmptyNode *create_node(NodeData *, NXEmptyNode *, NXFile *);
		template <typename T> static NXEmptyNode *create_valued_node(NodeData *, NXEmptyNode *, NXFile *);
	protected:
		NXEmptyNode *_parent;
		NXFile *_file;
		NodeData *_data;
		NXNode *_wrapper;
		NXEmptyNode(NodeData *p, NXEmptyNode *parent, NXFile *f) : _parent(parent), _file(f), _data(p) { _wrapper = new NXNode(this); }
	public:
		virtual ~NXEmptyNode() { delete _wrapper; }
		virtual size_t size() const;
		
		virtual const NXEmptyNode *child(const ::std::string&);

		virtual child_map::const_iterator begin();
		virtual child_map::const_iterator end();
		
		bool operator ==(NXEmptyNode n) const { return n._data == _data && n._file == _file && n._parent == _parent; }
		bool operator !=(NXEmptyNode n) const { return !(*this == n); }

		friend class NXFile;
		friend class NXNode;
		friend class NXParentNode;
	};

	template <typename T> class NXValuedNode : public virtual NXEmptyNode {
	protected:
		NXValuedNode(NodeData *p, NXEmptyNode *parent, NXFile *f) : NXEmptyNode(p, parent, f) {}
	public:
		virtual T value() const;
		uint32_t length() const;
		friend class NXEmptyNode;
	};

	class NXParentNode : public virtual NXEmptyNode {
		child_map _children;
		void parse_children();
	protected:
		NXParentNode(NodeData *p, NXEmptyNode *parent, NXFile *f);
	public:
		virtual ~NXParentNode();
		size_t size() const override;
		const NXEmptyNode *child(const ::std::string&) override;

		child_map::const_iterator begin() override { if(_children.size() == 0) parse_children(); return _children.cbegin(); }
		child_map::const_iterator end() override { if(_children.size() == 0) parse_children(); return _children.cend(); }

		friend class NXEmptyNode;
	};

	template <typename T> class NXValuedParentNode final : public NXParentNode, public NXValuedNode<T> {
		NXValuedParentNode(NodeData *p, NXEmptyNode *parent, NXFile *f) : NXParentNode(p, parent, f), NXValuedNode(p, parent, f), NXEmptyNode(p, parent, f) {}
	public:
		friend class NXEmptyNode;

		#pragma warning(suppress : 4250)
	};
}