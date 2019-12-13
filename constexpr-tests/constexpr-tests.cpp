#include <iostream>

namespace {
constexpr int cexpr_strlen(char const* s) {
    int i = 0;
    while (s[i]) {
        ++i;
    }
    return i;
}
static_assert(cexpr_strlen("") == 0, "");
static_assert(cexpr_strlen("123\0abcdef") == 3, "");

constexpr bool cexpr_streq(int const N, char const* lhs, char const* rhs) {
    return N <= 0 or (*lhs == 0 and *rhs == 0)
        or *lhs == *rhs and cexpr_streq(N - 1, lhs + 1, rhs + 1);
}
constexpr bool cexpr_streq(char const* lhs, char const* rhs) {
    return cexpr_strlen(lhs) == cexpr_strlen(rhs)
        and cexpr_streq(cexpr_strlen(lhs), lhs, rhs);
}
static_assert(cexpr_streq("", ""), "");
static_assert(cexpr_streq("aaa", "aaa"), "");
static_assert(not cexpr_streq("aaa", "aaabbb"), "");

static_assert(cexpr_streq(3, "aaa", "aaabbb"), "");
static_assert(cexpr_streq(15, "", ""), "");
static_assert(cexpr_streq(0, "aaa", "bbb"), "");

constexpr int cexpr_find_first(char const* s, char const* sub) {
    auto const subLen = cexpr_strlen(sub);
    auto const sLen = cexpr_strlen(s);

    for (auto first = 0; first <= sLen - subLen; ++first) {
        if (cexpr_streq(subLen, s + first, sub)) {
            return first;
        }
    }
    return -1;
}
static_assert(cexpr_find_first("AaBbCcDd", "Bb") == 2, "");
static_assert(cexpr_find_first("AaBbCcDd", "Dd") == 6, "");
static_assert(cexpr_find_first(__FILE__, "constexpr-tests") >= 0, "");

static_assert(cexpr_find_first("AaBbCcDd", "Dd ") == -1, "");
static_assert(cexpr_find_first("AaBbCcDd", "¥¹Ææ") == -1, "");

}

class ModuleName {
public:
    constexpr ModuleName(char const* path) : path(path) {}
    
    constexpr auto get() const {
        for (auto const& rootFolder : rootFolders) {
            int first = cexpr_find_first(path, rootFolder);
            if(first != -1) {
                return path + first + cexpr_strlen(rootFolder);
            }
        }
        return path;
    }

private:
    static constexpr char const* rootFolders[] = { "\\cpp-utils\\", "/cpp-utils/" };
    
    char const* path;
};
static_assert(cexpr_streq("constexpr-tests\\constexpr-tests.cpp", ModuleName(__FILE__).get()), "");


int main() {
    constexpr ModuleName moduleName(__FILE__);
    std::cout << "Module name: [" << moduleName.get() << "]\n";
    return 0;
}
