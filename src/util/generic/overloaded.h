#pragma once

/**
 * Construct an ad-hoc object with an overloaded `operator()`.
 *
 * Typically used with lambdas to construct type-matching visitors for e.g. std::variant:
 * ```
 * std::variant<int, void*, std::string> var;
 * Visit(TOverloaded{
 *     [](int val) { std::cerr << "int: " << val; },
 *     [](void* val) { std::cerr << "ptr: " << val; },
 *     [](const std::string& val) { std::cerr << "str: " << val; },
 * }, var);
 * ```
 *
 * *** IMPORTANT NOTE (IMPLICIT ARGUMENT CONVERSIONS) ***
 *
 * Since the resulting objects use regular overloaded method resolution rules,
 * methods may be called by inexact matches (causing implicit casts), hence this
 * implementation does not guarantee exhaustiveness of cases.
 *
 * For example, the following code will compile and run by casting all values to
 * double:
 * ```
 * std::variant<int, double, char> var;
 * Visit(TOverloaded{
 *     [](double val) { std::cerr << "dbl: " << val; },
 * }, var);
 * ```
 *
 * If cases may be ambigous or specific type-matching logic is required,
 * a verbose `if constexpr`-based version would be preferred:
 * ```
 * std::variant<int, double, char> var;
 * Visit([](auto&& val) {
 *     using T = std::decay_t<decltype(val)>;
 *     if constexpr (std::is_same_v<T, int>) {
 *         std::cerr << "int: " << val;
 *     } else if constexpr (std::is_same_v<T, double>) {
 *         std::cerr << "dbl: " << val;
 *     } else if constexpr (std::is_same_v<T, char>) {
 *         std::cerr << "chr: " << val;
 *     } else {
 *         static_assert(TDependentFalse<T>, "unexpected type");
 *     }
 * }, var);
 * ```
 */

template <class... Fs>
struct TOverloaded: Fs... {
    using Fs::operator()...;
};

template <class... Fs>
TOverloaded(Fs...) -> TOverloaded<Fs...>;
