#ifndef CARSTORE_EXTENSIONS_H
#define CARSTORE_EXTENSIONS_H

#include "Includes.h"

template <typename T>
inline bool contains(const vector<T>& v, const T& i) {
    return std::find(v.begin(), v.end(), i) != v.cend();
}

template <typename T, typename F>
inline std::string join(const vector<T>& v, F f, const std::string& s = " ") {
    std::string ss;
    for (const auto& i: v) {
        ss += f(i) + (&i != &v.back() ? s : "");
    }
    return ss;
}
template <typename T, typename F>
inline void join(const vector<T>& v, std::string& ss, F f, const std::string& s = " ") {
    ss = join(v, f, s);
}

template <typename K, typename T, typename F>
inline std::string join(const map<K, T>& m, F f, const std::string& s = " ") {
    std::string ss;
    for (const auto& i: m) {
        ss += f(i) + s;
    }
    return ss.substr(0, ss.size() - s.size());
}
template <typename K, typename T, typename F>
inline void join(const map<K, T>& v, std::string& ss, F f, const std::string& s = " ") {
    ss =  join(v, f, s);
}

template <typename T, typename F>
inline void separate(const std::string& str, vector<T>& v, const std::string& s, F f) {
    boost::char_separator<char> sep{s.data()};
    tokenizer tok(str, sep);
    for (const auto& i: tok) {
        v.emplace_back(f(i));
    }
}
template <typename T>
inline void separate(const std::string& str, vector<T>& v, const std::string& s = " ") {
    boost::char_separator<char> sep{s.data()};
    tokenizer tok(str, sep);
    for (const auto& i: tok) {
        v.emplace_back(i);
    }
}

template <typename T, typename T2>
inline vector<T2> convert(const vector<T>& v, std::function<T2(T)> f) {
    vector<T2> vv;
    for (const auto& i: v) {
        vv.emplace_back(f(i));
    }
    return vv;
}

#endif //CARSTORE_EXTENSIONS_H
