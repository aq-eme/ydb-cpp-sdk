#pragma once
#ifndef ENUM_INL_H_
#error "Direct inclusion of this file is not allowed, include enum.h"
// For the sake of sane code completion.
#include "enum.h"
#endif

#include <src/util/string/escape.h>
#include <src/util/string/cast.h>

#include <src/util/generic/cast.h>

#include <algorithm>
#include <format>
#include <stdexcept>

namespace NYT {

////////////////////////////////////////////////////////////////////////////////

#define ENUM__CLASS(enumType, underlyingType, seq) \
    enum class enumType : underlyingType \
    { \
        PP_FOR_EACH(ENUM__DOMAIN_ITEM, seq) \
    };

#define ENUM__DOMAIN_ITEM(item) \
    PP_IF( \
        PP_IS_SEQUENCE(item), \
        ENUM__DOMAIN_ITEM_SEQ, \
        ENUM__DOMAIN_ITEM_ATOMIC \
    )(item)()

#define ENUM__DOMAIN_ITEM_ATOMIC(item) \
    item PP_COMMA

#define ENUM__DOMAIN_ITEM_SEQ(seq) \
    PP_ELEMENT(seq, 0) = PP_ELEMENT(seq, 1) PP_COMMA

////////////////////////////////////////////////////////////////////////////////

namespace NDetail {

template <typename TValues>
constexpr bool CheckValuesMonotonic(const TValues& values)
{
    return std::adjacent_find(values.begin(), values.end(), std::greater_equal<>()) == values.end();
}

template <typename TValues>
constexpr bool CheckValuesUnique(const TValues& values)
{
    for (size_t i = 0; i < std::size(values); ++i) {
        for (size_t j = i + 1; j < std::size(values); ++j) {
            if (values[i] == values[j]) {
                return false;
            }
        }
    }
    return true;
}

template <typename TNames>
constexpr bool CheckDomainNames(const TNames& names)
{
    for (size_t i = 0; i < std::size(names); ++i) {
        if (std::size(names[i]) == 0) {
            return false;
        }
        for (size_t j = 1; j < std::size(names[i]); ++j) {
            // If name does not start with a capital letter, all the others must be in lowercase.
            if (('A' <= names[i][j] && names[i][j] <= 'Z') && ('A' > names[i][0] || names[i][0] > 'Z')) {
                return false;
            }
        }
    }
    return true;
}

} // namespace NDetail

////////////////////////////////////////////////////////////////////////////////

#define ENUM__BEGIN_TRAITS(enumType, underlyingType, isBit, isStringSerializable, seq) \
    struct TEnumTraitsImpl_##enumType \
    { \
        using T = enumType; \
        \
        [[maybe_unused]] static constexpr bool IsBitEnum = isBit; \
        [[maybe_unused]] static constexpr bool IsStringSerializableEnum = isStringSerializable; \
        static constexpr int DomainSize = PP_COUNT(seq); \
        \
        static constexpr int GetDomainSize() \
        { \
            return DomainSize; \
        } \
        \
        static constexpr std::array<std::string_view, DomainSize> Names{{ \
            PP_FOR_EACH(ENUM__GET_DOMAIN_NAMES_ITEM, seq) \
        }}; \
        static_assert(::NYT::NDetail::CheckDomainNames(Names), \
            "Enumeration " #enumType " contains names in wrong format"); \
        static constexpr std::array<T, DomainSize> Values{{ \
            PP_FOR_EACH(ENUM__GET_DOMAIN_VALUES_ITEM, seq) \
        }}; \
        \
        [[maybe_unused]] static constexpr bool IsMonotonic = \
            ::NYT::NDetail::CheckValuesMonotonic(Values); \
        \
        static std::string_view GetTypeName() \
        { \
            static constexpr std::string_view Result = PP_STRINGIZE(enumType); \
            return Result; \
        } \
        \
        static const std::optional<std::string_view> FindLiteralByValue(T value) \
        { \
            for (int i = 0; i < GetDomainSize(); ++i) { \
                if (Values[i] == value) { \
                    return Names[i]; \
                } \
            } \
            return std::nullopt; \
        } \
        \
        static std::optional<T> FindValueByLiteral(std::string_view literal) \
        { \
            for (int i = 0; i < GetDomainSize(); ++i) { \
                if (Names[i] == literal) { \
                    return Values[i]; \
                } \
            } \
            return std::nullopt; \
        } \
        \
        static constexpr const std::array<std::string_view, DomainSize>& GetDomainNames() \
        { \
            return Names; \
        } \
        \
        static constexpr const std::array<T, DomainSize>& GetDomainValues() \
        { \
            return Values; \
        }

#define ENUM__GET_DOMAIN_VALUES_ITEM(item) \
    PP_IF( \
        PP_IS_SEQUENCE(item), \
        ENUM__GET_DOMAIN_VALUES_ITEM_SEQ, \
        ENUM__GET_DOMAIN_VALUES_ITEM_ATOMIC \
    )(item)

#define ENUM__GET_DOMAIN_VALUES_ITEM_SEQ(seq) \
    ENUM__GET_DOMAIN_VALUES_ITEM_ATOMIC(PP_ELEMENT(seq, 0))

#define ENUM__GET_DOMAIN_VALUES_ITEM_ATOMIC(item) \
    T::item,

#define ENUM__GET_DOMAIN_NAMES_ITEM(item) \
    PP_IF( \
        PP_IS_SEQUENCE(item), \
        ENUM__GET_DOMAIN_NAMES_ITEM_SEQ, \
        ENUM__GET_DOMAIN_NAMES_ITEM_ATOMIC \
    )(item)

#define ENUM__GET_DOMAIN_NAMES_ITEM_SEQ(seq) \
    PP_IF( \
       ENUM__ITEM_SEQ_HAS_DOMAIN_NAME(seq), \
       ENUM__GET_DOMAIN_NAMES_ITEM_SEQ_CUSTOM, \
       ENUM__GET_DOMAIN_NAMES_ITEM_SEQ_AUTO \
    )(seq)

#define ENUM__ITEM_SEQ_HAS_DOMAIN_NAME(seq) \
    PP_CONCAT(ENUM__ITEM_SEQ_HAS_DOMAIN_NAME_, PP_COUNT(seq))

#define ENUM__ITEM_SEQ_HAS_DOMAIN_NAME_2 PP_FALSE
#define ENUM__ITEM_SEQ_HAS_DOMAIN_NAME_3 PP_TRUE

#define ENUM__GET_DOMAIN_NAMES_ITEM_SEQ_CUSTOM(seq) \
    std::string_view(PP_ELEMENT(seq, 2)),

#define ENUM__GET_DOMAIN_NAMES_ITEM_SEQ_AUTO(seq) \
    ENUM__GET_DOMAIN_NAMES_ITEM_ATOMIC(PP_ELEMENT(seq, 0))

#define ENUM__GET_DOMAIN_NAMES_ITEM_ATOMIC(item) \
    std::string_view(PP_STRINGIZE(item)),

#define ENUM__VALIDATE_UNIQUE(enumType) \
    static_assert(IsMonotonic || ::NYT::NDetail::CheckValuesUnique(Values), \
        "Enumeration " #enumType " contains duplicate values");

#define ENUM__END_TRAITS(enumType) \
    }; \
    \
    [[maybe_unused]] inline TEnumTraitsImpl_##enumType GetEnumTraitsImpl(enumType) \
    { \
        return {}; \
    } \
    \
    using ::ToString; \
    [[maybe_unused]] inline std::string ToString(enumType value) \
    { \
        return ::NYT::TEnumTraits<enumType>::ToString(value); \
    }

////////////////////////////////////////////////////////////////////////////////

template <class T>
constexpr int TEnumTraitsWithKnownDomain<T, true>::GetDomainSize()
{
    return TEnumTraitsImpl<T>::GetDomainSize();
}

template <class T>
constexpr auto TEnumTraitsWithKnownDomain<T, true>::GetDomainNames() -> const std::array<std::string_view, GetDomainSize()>&
{
    return TEnumTraitsImpl<T>::GetDomainNames();
}

template <class T>
constexpr auto TEnumTraitsWithKnownDomain<T, true>::GetDomainValues() -> const std::array<T, GetDomainSize()>&
{
    return TEnumTraitsImpl<T>::GetDomainValues();
}

template <class T>
constexpr T TEnumTraitsWithKnownDomain<T, true>::GetMinValue()
    requires (!TEnumTraitsImpl<T>::IsBitEnum)
{
    const auto& values = GetDomainValues();
    static_assert(!values.empty()); \
    return *std::min_element(std::begin(values), std::end(values));
}

template <class T>
constexpr T TEnumTraitsWithKnownDomain<T, true>::GetMaxValue()
    requires (!TEnumTraitsImpl<T>::IsBitEnum)
{
    const auto& values = GetDomainValues();
    static_assert(!values.empty()); \
    return *std::max_element(std::begin(values), std::end(values));
}

template <class T>
std::vector<T> TEnumTraitsWithKnownDomain<T, true>::Decompose(T value)
    requires (TEnumTraitsImpl<T>::IsBitEnum)
{
    std::vector<T> result;
    for (auto domainValue : GetDomainValues()) {
        if (Any(value & domainValue)) {
            result.push_back(domainValue);
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

template <class T>
std::string_view TEnumTraits<T, true>::GetTypeName()
{
    return TEnumTraitsImpl<T>::GetTypeName();
}

template <class T>
std::optional<T> TEnumTraits<T, true>::FindValueByLiteral(std::string_view literal)
{
    return TEnumTraitsImpl<T>::FindValueByLiteral(literal);
}

template <class T>
std::optional<std::string_view> TEnumTraits<T, true>::FindLiteralByValue(T value)
{
    return TEnumTraitsImpl<T>::FindLiteralByValue(value);
}

template <class T>
std::string TEnumTraits<T, true>::ToString(T value)
{
    using ::ToString;
    if (auto optionalLiteral = TEnumTraits<T>::FindLiteralByValue(value)) {
        return ToString(*optionalLiteral);
    }
    std::string result;
    result = TEnumTraits<T>::GetTypeName();
    result += "(";
    result += ToString(ToUnderlying(value));
    result += ")";
    return result;
}

template <class T>
T TEnumTraits<T, true>::FromString(std::string_view literal)
{
    auto optionalValue = FindValueByLiteral(literal);
    if (!optionalValue) {
        throw ::NYT::TSimpleException(std::format("Error parsing {} value {}",
            GetTypeName().data(),
            NUtils::Quote(literal)));
    }
    return *optionalValue;
}

////////////////////////////////////////////////////////////////////////////////

template <class E, class T, E Min, E Max>
constexpr TEnumIndexedVector<E, T, Min, Max>::TEnumIndexedVector()
    : Items_{}
{ }

template <class E, class T, E Min, E Max>
constexpr TEnumIndexedVector<E, T, Min, Max>::TEnumIndexedVector(std::initializer_list<T> elements)
    : Items_{}
{
    Y_ASSERT(std::distance(elements.begin(), elements.end()) <= N);
    size_t index = 0;
    for (const auto& element : elements) {
        Items_[index++] = element;
    }
}

template <class E, class T, E Min, E Max>
T& TEnumIndexedVector<E, T, Min, Max>::operator[] (E index)
{
    Y_ASSERT(index >= Min && index <= Max);
    return Items_[ToUnderlying(index) - ToUnderlying(Min)];
}

template <class E, class T, E Min, E Max>
const T& TEnumIndexedVector<E, T, Min, Max>::operator[] (E index) const
{
    return const_cast<TEnumIndexedVector&>(*this)[index];
}

template <class E, class T, E Min, E Max>
T* TEnumIndexedVector<E, T, Min, Max>::begin()
{
    return Items_.data();
}

template <class E, class T, E Min, E Max>
const T* TEnumIndexedVector<E, T, Min, Max>::begin() const
{
    return Items_.data();
}

template <class E, class T, E Min, E Max>
T* TEnumIndexedVector<E, T, Min, Max>::end()
{
    return begin() + N;
}

template <class E, class T, E Min, E Max>
const T* TEnumIndexedVector<E, T, Min, Max>::end() const
{
    return begin() + N;
}

template <class E, class T, E Min, E Max>
bool TEnumIndexedVector<E, T, Min, Max>::IsDomainValue(E value)
{
    return value >= Min && value <= Max;
}

////////////////////////////////////////////////////////////////////////////////

#define ENUM__BINARY_BITWISE_OPERATOR(T, assignOp, op) \
    [[maybe_unused]] inline constexpr T operator op (T lhs, T rhs) \
    { \
        return T(ToUnderlying(lhs) op ToUnderlying(rhs)); \
    } \
    \
    [[maybe_unused]] inline T& operator assignOp (T& lhs, T rhs) \
    { \
        lhs = T(ToUnderlying(lhs) op ToUnderlying(rhs)); \
        return lhs; \
    }

#define ENUM__UNARY_BITWISE_OPERATOR(T, op) \
    [[maybe_unused]] inline constexpr T operator op (T value) \
    { \
        return T(op ToUnderlying(value)); \
    }

#define ENUM__BIT_SHIFT_OPERATOR(T, assignOp, op) \
    [[maybe_unused]] inline constexpr T operator op (T lhs, size_t rhs) \
    { \
        return T(ToUnderlying(lhs) op rhs); \
    } \
    \
    [[maybe_unused]] inline T& operator assignOp (T& lhs, size_t rhs) \
    { \
        lhs = T(ToUnderlying(lhs) op rhs); \
        return lhs; \
    }

#define ENUM__BITWISE_OPS(enumType) \
    ENUM__BINARY_BITWISE_OPERATOR(enumType, &=, &)  \
    ENUM__BINARY_BITWISE_OPERATOR(enumType, |=, | ) \
    ENUM__BINARY_BITWISE_OPERATOR(enumType, ^=, ^)  \
    ENUM__UNARY_BITWISE_OPERATOR(enumType, ~)       \
    ENUM__BIT_SHIFT_OPERATOR(enumType, <<=, << )    \
    ENUM__BIT_SHIFT_OPERATOR(enumType, >>=, >> )

////////////////////////////////////////////////////////////////////////////////

template <typename E>
    requires TEnumTraits<E>::IsBitEnum
constexpr bool Any(E value) noexcept
{
    return ToUnderlying(value) != 0;
}

template <typename E>
    requires TEnumTraits<E>::IsBitEnum
constexpr bool None(E value) noexcept
{
    return ToUnderlying(value) == 0;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT