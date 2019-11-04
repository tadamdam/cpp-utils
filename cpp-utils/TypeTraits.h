#pragma once

template<class TypeToCount>
constexpr std::size_t type_count() {
    return 0;
}

template<class TypeToCount, class FirstArg, class... Args>
constexpr std::size_t type_count() {
    return (std::is_same_v<TypeToCount, FirstArg> ? 1 : 0)
        + type_count<TypeToCount, Args...>();
}

static_assert(type_count<int>() == 0, "!");
static_assert(type_count<int, bool>() == 0, "!");
static_assert(type_count<int, int>() == 1, "!");
static_assert(type_count<int, bool, int>() == 1, "!");

static_assert(type_count<int, int, signed, long>() == 2, "!");
static_assert(type_count<char&, char*, char**>() == 0, "!");
static_assert(type_count<char*&, char*&, char&>() == 1, "!");

// Tricky! :)
static_assert(type_count<
    int, 
    signed, int, signed int, std::int32_t,  // == int
    std::size_t, size_t                     // != int
>() == 4, "!");


template<class CheckType, class... Types>
constexpr bool is_exactly_one_of = type_count<CheckType, Types...>() == 1;

static_assert(is_exactly_one_of<int, int>, "!");
static_assert(not is_exactly_one_of<int>, "!");
static_assert(not is_exactly_one_of<int, int, int>, "!");
