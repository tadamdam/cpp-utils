#include <iostream>
#include <tuple>
#include <optional>

#include "named_type.hpp"
#include "TypeTraits.h"

using FirstName = fluent::NamedType<std::string, struct FirstNameTag>;
static const FirstName::argument firstName;
using LastName = fluent::NamedType<std::string, struct LastNameTag>;
static const LastName::argument lastName;
// TODO use a compile-time-generated UID to differentiate between tags instead of specifying them manually

namespace {
template<class TypeToPick, class... Types,
    std::enable_if_t<not is_exactly_one_of<TypeToPick, Types...>, struct EnableIfTypeNotFound *> = nullptr
>
constexpr std::optional<TypeToPick> pickFrom(Types&&... args) {
    return std::nullopt;
}

template<class TypeToPick, class FirstType, class... Types, 
    std::enable_if_t<is_exactly_one_of<TypeToPick, FirstType, Types...> && std::is_same_v<TypeToPick, FirstType>, struct EnableIfFirstTypeIsOK *> = nullptr
>
constexpr std::optional<TypeToPick> pickFrom(FirstType&& firstArg, Types&&... args) {
    return firstArg;
}

template<class TypeToPick, class FirstType, class... Types, 
    std::enable_if_t<is_exactly_one_of<TypeToPick, FirstType, Types...> && not std::is_same_v<TypeToPick, FirstType>, struct EnableIfLookingUpOtherTypes *> = nullptr
>
constexpr std::optional<TypeToPick> pickFrom(FirstType&& firstArg, Types&&... args) {
    return pickFrom<TypeToPick, Types...>(std::forward<Types>(args)...);
}
}

struct NamedArgs {
    template<class... Args>
    NamedArgs(Args&&... args)
        : firstName(pickFrom<FirstName>(std::forward<Args>(args)...).value_or(FirstName{ "<FirstName>" }))
        , lastName(pickFrom<LastName>(std::forward<Args>(args)...).value_or(LastName{ "<LastName>" }))
    {}

    FirstName firstName;
    LastName lastName;
};

std::ostream& operator<<(std::ostream& os, NamedArgs namedArgs) {
    return os
        << "First/last name: " << namedArgs.firstName.get() << " " << namedArgs.lastName.get()
        << std::endl;
}

int main() {
    std::cout << NamedArgs() << std::endl;
    std::cout << NamedArgs(LastName{ "Doe" }) << std::endl;

    std::cout << NamedArgs(LastName{ "Doe" }, FirstName{ "Joe" }) << std::endl;
    std::cout << NamedArgs(FirstName{ "Joe" }, LastName{ "Doe" }) << std::endl;
}

