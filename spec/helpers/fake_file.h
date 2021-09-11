#pragma once

#include <stdint.h>
#include <stdio.h>
#include <vector>

using std::vector;

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

extern void f_open_memory(const uint8_t *data, long size);
extern void f_open_memory(const vector<uint8_t> &data);
extern void f_print_memory();
extern ssize_t f_read(int fd, void *buf, size_t count);

#define FFILE(init) (f_open_memory(init, sizeof(init)))
#define FFILEV(init) const vector<uint8_t> m = (init); f_open_memory(m)

extern vector<uint8_t> operator+(const vector<uint8_t> &a, const vector<uint8_t> &b);

// a little DSL to concatenate vectors
// provides for constructing test files out of blocks
// doesn't have to be efficient
//  vector<uint8_t> inline operator+(const vector<uint8_t> &a, const vector<uint8_t> &b) {
//     vector<uint8_t> r = a;
//     r.insert(r.end(), b.begin(), b.end());
//     return r;
// }

// template <typename T>
// std::vector<uint8_t> inline operator+(const std::vector<uint8_t>& lhs, const std::vector<uint8_t>& rhs)
// {
//     if (lhs.empty()) return rhs;
//     if (rhs.empty()) return lhs;
//     std::vector<uint8_t> result {};
//     result.reserve(lhs.size() + rhs.size());
//     result.insert(result.cend(), lhs.cbegin(), lhs.cend());
//     result.insert(result.cend(), rhs.cbegin(), rhs.cend());
//     return result;
// }
