#include <iostream>
#include <tuple>
#include <optional>

#include "named_type.hpp"
#include "TypeTraits.h"

template<typename ReturnType, typename... Types,
    std::enable_if_t<is_none_of<ReturnType, Types...>, struct TypeNotDefined*> = nullptr
>
constexpr std::optional<ReturnType> pickFrom(Types&&... args) {
    return std::nullopt;
}

template<class ReturnType, class FirstType, class... Types,
    std::enable_if_t<not std::is_same_v<ReturnType, FirstType>, struct EnableIfKeepSearching*> = nullptr
>
constexpr std::optional<ReturnType> pickFrom(FirstType&& firstArg, Types&&... args) {
    return pickFrom<ReturnType, Types...>(std::forward<Types>(args)...);
}

template<class ReturnType, class FirstType, class... Types,
    std::enable_if_t<std::is_same_v<ReturnType, FirstType>, struct EnableIfFirstTypeIsOK*> = nullptr
>
    constexpr std::optional<ReturnType> pickFrom(FirstType&& firstArg, Types&&... args) {
    static_assert(is_exactly_one_of<ReturnType, FirstType, Types...>, "*** Type defined more than once! ***");
    return firstArg;
}

using FirstName = fluent::NamedType<std::string, struct FirstNameTag>;
static const FirstName::argument firstName;
using LastName = fluent::NamedType<std::string, struct LastNameTag>;
static const LastName::argument lastName;
// TODO use a compile-time-generated UID to differentiate between tags instead of specifying them manually

struct NamedArgs {
    template<class... Args>
    NamedArgs(Args&&... args)
        : firstName(pickFrom<FirstName>(std::forward<Args>(args)...).value_or(FirstName{ "<FirstName>" }))
        , lastName(pickFrom<LastName>(std::forward<Args>(args)...).value_or(LastName{ "<LastName>" }))
    {}

    friend std::ostream& operator<<(std::ostream&, NamedArgs const&);
private:
    FirstName const firstName;
    LastName const lastName;
};

std::ostream& operator<<(std::ostream& os, NamedArgs const& namedArgs) {
    return os
        << "First/last name: " << namedArgs.firstName.get() << " " << namedArgs.lastName.get()
        << std::endl;
}

int main() {
    std::cout << NamedArgs() << std::endl;
    std::cout << NamedArgs(LastName{ "Doe" }) << std::endl;

    std::cout << NamedArgs(LastName{ "Doe" }, FirstName{ "Joe" }) << std::endl;
    std::cout << NamedArgs(FirstName{ "Joe" }, LastName{ "Doe" }) << std::endl;
    std::cout << NamedArgs(FirstName{ "Joe" }, LastName{ "Doe" }, LastName{ "Moe" }) << std::endl;
}

