#pragma once
#include <stdint.h>
#include <cstring>
#include <string>
#include "mmap.hpp"

#ifndef RENXCPPH
#define RENXCPPH

namespace reNX {
    class NXNode;
    class NXFile;

    enum NXDataType {
        nothing,
        int64,
        fpoint,
        string,
        vector,
        canvas,
        mp3
    };

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
    typedef std::pair<uint16_t, const char *> nxstring;

    class NXFile final {
        const uint64_t* _stable;
        const NodeData* _ntable;
        NXNode* _nodes;
        MemoryMappedFile* _mmap;

        inline std::string get_string(uint32_t id) const {
            const char* p = _mmap->base() + _stable[id];
            return std::string(p + 2, *reinterpret_cast<const uint16_t *>(p));
        }

        inline nxstring get_nxstring(uint32_t id) const {
            const char* p = _mmap->base() + _stable[id];
            return std::make_pair(*reinterpret_cast<uint16_t *>(const_cast<char *>(p)), p + 2);
        }

    public:
        NXFile(const char* fn);
        ~NXFile();

        inline const NXNode& base() const {
            return *_nodes;
        }

        friend class NXNode;
    };

    class NXNode final {
        static NXNode* _nullnode;
        const NXFile* _file;
        const NodeData* _data;

        inline NXNode() {}

        inline NXNode(int) {
            _file = nullptr;
            _data = nullptr;
        }

    public:
        inline std::string name() const {
            return _file->get_string(_data->NodeNameID);
        }

        inline nxstring nxname() const {
            return _file->get_nxstring(_data->NodeNameID);
        }

        inline NXDataType type() const {
            return static_cast<NXDataType>(_data->Type);
        }

        inline size_t size() const {
            return _data->ChildCount;
        }

        inline bool exists() const {
            return _data != nullptr;
        }

        template <typename T>
        T value() const;
        uint32_t length() const;

        inline const NXNode& operator[](const char* n) const {
            return child(n, static_cast<uint16_t>(std::strlen(n)));
        }

        inline const NXNode& operator[](const ::std::string& n) const {
            return child(n.c_str());
        }

        inline const NXNode& operator[](const nxstring& n) const {
            return child(n.second, n.first);
        }

        inline const NXNode& child(const char* n) const {
            return child(n, static_cast<uint16_t>(std::strlen(n)));
        }

        inline const NXNode& child(const ::std::string& n) const {
            return child(n.c_str());
        }

        inline const NXNode& child(const nxstring& n) const {
            return child(n.second, n.first);
        }

        const NXNode& child(const char* n, uint16_t nl) const;

        inline const NXNode* begin() const {
            return _file->_nodes + _data->FirstChildID;
        }

        inline const NXNode* end() const {
            return _file->_nodes + _data->FirstChildID + _data->ChildCount;
        }

        inline bool operator ==(NXNode n) const {
            return n._data == _data && n._file == _file;
        }

        inline bool operator !=(NXNode n) const {
            return n._data != _data || n._file != _file;
        }

        friend class NXFile;
    };
}
#endif
