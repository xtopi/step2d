#pragma once
#include "icreature.h"
#include "saveload.h"

namespace NStep {

struct TCreatureMetadata {
    string Name;
    float X;
    size_t Data;
};

template <> inline size_t Add(TCreatureMetadata& s, char* buf, bool write) {
    // size_t reclen = sizeof(size_t) + s.Genealogy.size() + sizeof(size_t) + sizeof(float);
    size_t version = 1;
    size_t n = 0; // Add(reclen, buf, write);
    n += Add(version, buf + n, write);
    assert(version == 1);
    n += Add(s.Name, buf + n, write);
    n += Add(s.X, buf + n, write);
    return n;
    // return reclen;
}

}
