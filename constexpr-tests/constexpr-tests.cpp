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

enum class CharCase { None, CaseInsensitive };

template<CharCase charCase = CharCase::CaseInsensitive>
constexpr int cexpr_tolower(int const c) {
    if (charCase != CharCase::CaseInsensitive) {
        return c;
    }
    if (c >= 'A' and c <= 'Z') {
        return c - ('A' - 'a');
    }
    return c;
}
static_assert(cexpr_tolower('a') == 'a', "");
static_assert(cexpr_tolower('O') == 'o', "");
static_assert(cexpr_tolower('O') == cexpr_tolower('o'), "");
static_assert(cexpr_tolower('!') == '!', "");
static_assert(cexpr_tolower(0) == 0, "");

static_assert(cexpr_tolower<CharCase::None>('O') == 'O', "");

template<CharCase charCase>
constexpr bool cexpr_streq(int const N, char const* lhs, char const* rhs) {
    return N <= 0 or (*lhs == 0 and *rhs == 0)
        or cexpr_tolower<charCase>(*lhs) == cexpr_tolower<charCase>(*rhs) and cexpr_streq<charCase>(N - 1, lhs + 1, rhs + 1);
}
template<CharCase charCase>
constexpr bool cexpr_streq(char const* lhs, char const* rhs) {
    return cexpr_strlen(lhs) == cexpr_strlen(rhs)
        and cexpr_streq<charCase>(cexpr_strlen(lhs), lhs, rhs);
}
static_assert(cexpr_streq<CharCase::None>("", ""), "");
static_assert(cexpr_streq<CharCase::None>("aaa", "aaa"), "");
static_assert(not cexpr_streq<CharCase::None>("aaa", "aaabbb"), "");

static_assert(cexpr_streq<CharCase::None>(3, "aaa", "aaabbb"), "");
static_assert(cexpr_streq<CharCase::None>(15, "", ""), "");
static_assert(cexpr_streq<CharCase::None>(0, "aaa", "bbb"), "");

static_assert(cexpr_streq<CharCase::CaseInsensitive>("", ""), "");
static_assert(cexpr_streq<CharCase::CaseInsensitive>("aaa", "AAa"), "");
static_assert(not cexpr_streq<CharCase::CaseInsensitive>("aaa", "aaabbb"), "");
static_assert(not cexpr_streq<CharCase::CaseInsensitive>("aaa", "AaaBbb"), "");

static_assert(cexpr_streq<CharCase::CaseInsensitive>(3, "aaa", "aAabbb"), "");
static_assert(cexpr_streq<CharCase::CaseInsensitive>(15, "", ""), "");
static_assert(cexpr_streq<CharCase::CaseInsensitive>(0, "aaa", "bbb"), "");

template<CharCase charCase>
constexpr int cexpr_find_first(char const* s, char const* sub) {
    auto const subLen = cexpr_strlen(sub);
    auto const sLen = cexpr_strlen(s);

    for (auto first = 0; first <= sLen - subLen; ++first) {
        if (cexpr_streq<charCase>(subLen, s + first, sub)) {
            return first;
        }
    }
    return -1;
}
static_assert(cexpr_find_first<CharCase::None>("AaBbCcDd", "Bb") == 2, "");
static_assert(cexpr_find_first<CharCase::None>("AaBbCcDd", "Dd") == 6, "");
static_assert(cexpr_find_first<CharCase::None>("AaBbCcDd", "dd") == -1, "");
static_assert(cexpr_find_first<CharCase::None>("AaBbCcDd", "DD") == -1, "");
static_assert(cexpr_find_first<CharCase::None>(__FILE__, "constexpr-tests") >= 0, "");

static_assert(cexpr_find_first<CharCase::None>("AaBbCcDd", "Dd ") == -1, "");
static_assert(cexpr_find_first<CharCase::None>("AaBbCcDd", "¥¹Ææ") == -1, "");

static_assert(cexpr_find_first<CharCase::CaseInsensitive>("AaBbCcDd", "Bb") == 2, "");
static_assert(cexpr_find_first<CharCase::CaseInsensitive>("AaBbCcDd", "Dd") == 6, "");
static_assert(cexpr_find_first<CharCase::CaseInsensitive>("AaBbCcDd", "dd") == 6, "");
static_assert(cexpr_find_first<CharCase::CaseInsensitive>("AaBbCcDd", "DD") == 6, "");
static_assert(cexpr_find_first<CharCase::CaseInsensitive>(__FILE__, "constexpr-tests") >= 0, "");

static_assert(cexpr_find_first<CharCase::CaseInsensitive>("AaBbCcDd", "Dd ") == -1, "");
static_assert(cexpr_find_first<CharCase::CaseInsensitive>("AaBbCcDd", "¥¹Ææ") == -1, "");

}

#ifdef _WIN32
static constexpr CharCase const defaultCharCaseStrategy = CharCase::CaseInsensitive;
#else
static constexpr CharCase const defaultCharCaseStrategy = CharCase::None;
#endif

template<CharCase charCase>
class ModuleName {
public:
    constexpr ModuleName(char const* path) : path(path) {}
    
    constexpr auto get() const {
        for (auto const& rootFolder : rootFolders) {
            int first = cexpr_find_first<charCase>(path, rootFolder);
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
static_assert(cexpr_streq<CharCase::None>(
    "constexpr-tests\\constexpr-tests.cpp", 
    ModuleName<CharCase::None>(__FILE__).get()
), "");

static_assert(not cexpr_streq<CharCase::None>(
    "constexpr-tests\\constexpr-tests.cpp", 
    ModuleName<CharCase::None>("C:\\some\\folders\\CPP-UTILS\\constexpr-tests\\constexpr-tests.cpp").get()
), "");
static_assert(cexpr_streq<CharCase::None>(
    "constexpr-tests\\constexpr-tests.cpp", 
    ModuleName<CharCase::CaseInsensitive>("C:\\some\\folders\\CPP-UTILS\\constexpr-tests\\constexpr-tests.cpp").get()
), "");

int main() {
    constexpr ModuleName<defaultCharCaseStrategy> moduleName(__FILE__);
    std::cout << "Module name: [" << moduleName.get() << "]\n";
    return 0;
}
