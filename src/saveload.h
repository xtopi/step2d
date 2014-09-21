#pragma once

namespace NStep {

inline string GetFile(const string& name)
{
    ifstream is(name.c_str());
    if (!is)
        return "";
    // size_t sz = is.seekoff(0, ios_base::end);
    is.seekg(0, ios_base::end);
    size_t sz = is.tellg();
    is.seekg(0, ios_base::beg);

    string ret(sz, ' ');
    is.read(&ret[0], sz);
    return ret;
}

template <typename T>
size_t Add(T& t, char* buf, bool write);

inline size_t AddMem(char* mem, size_t n, char* buf, bool write) {
    if (write) {
        memcpy(buf, mem, n);
    }
    else {
        memcpy(mem, buf, n);
    }
    return n;
}

template <typename T>
size_t AddPod(T& t, char* buf, bool write) {
    return AddMem((char*)&t, sizeof(T), buf, write);
}

inline size_t Add(int& i, char* buf, bool write) {
    return AddPod(i, buf, write);
}
inline size_t Add(float& i, char* buf, bool write) {
    return AddPod(i, buf, write);
}
inline size_t Add(size_t& i, char* buf, bool write) {
    return AddPod(i, buf, write);
}
inline size_t Add(bool& i, char* buf, bool write) {
    return AddPod(i, buf, write);
}
inline size_t Add(string& s, char* buf, bool write) {
    size_t len = s.size();
    size_t n = Add(len, buf, write);
    s.resize(len);
    n += AddMem(&s[0], s.size(), buf + n, write);
    return n;
}

template <typename T, typename T2>
size_t Add(pair<T, T2>& p, char* buf, bool write) {
    size_t s = Add(p.first, buf, write);
    s += Add(p.second, buf + s, write);
    return s;
}

template <typename T>
size_t Add(vector<T>& v, char* buf, bool write) {
    size_t n = v.size();
    size_t s = Add(n, buf, write);
    v.resize(n);
    // Dlog << "add vector of size " << n << endl;
    for (auto& t: v) {
        s += Add(t, buf + s, write);
    }
    return s;
}

}
