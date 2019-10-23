#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

namespace Lambda
{
//! utils to use std::unordered_set with std::weak_ptr.
namespace Util
{
template <typename T>
struct MyWeakPtrHash {
    std::size_t operator()(const std::weak_ptr<T>& wp) const
    {
        auto sp = wp.lock();
        return std::hash<decltype(sp)>()(sp);
    }
};

template <typename T>
struct MyWeakPtrEq {
    bool operator()(const std::weak_ptr<T>& left, const std::weak_ptr<T>& right) const
    {
        return !left.owner_before(right) && !right.owner_before(left);
    }
};

template <typename T>
using UnorderedWeakPtrSet = std::unordered_set<std::weak_ptr<T>, MyWeakPtrHash<T>, MyWeakPtrEq<T>>;
}  // namespace Util

struct Id;
struct Abstr;
struct App;

using LambdaPtr = std::shared_ptr<std::variant<Id, Abstr, App>>;

//! identifire
struct Id {
    std::string to_string() const;
    void substitute(LambdaPtr l);
    void refresh_symbol() {}
    bool contain_symbol(std::string s) const { return s == symbol; }
    Util::UnorderedWeakPtrSet<LambdaPtr::element_type> childs() const;
    LambdaPtr clone(const Util::UnorderedWeakPtrSet<LambdaPtr::element_type>& refs_candidate);
    std::vector<LambdaPtr> clone_unique();
    LambdaPtr clone() const;

    std::string symbol;

    friend LambdaPtr make_id(std::string symbol);
    friend LambdaPtr make_abstr(LambdaPtr id, LambdaPtr expr);
    friend LambdaPtr make_app(LambdaPtr left, LambdaPtr right);

private:
    Id(std::string symbol) : symbol(symbol), refs()
    {
    }

    std::vector<std::pair<std::uint8_t, LambdaPtr::weak_type>> refs;
};

LambdaPtr make_id(std::string symbol);


struct Abstr {
    std::string to_string() const;
    void refresh_symbol();
    bool contain_symbol(std::string s) const { return symbol_set.count(s) > 0; }
    Util::UnorderedWeakPtrSet<LambdaPtr::element_type> childs() const;
    LambdaPtr clone() const;

    LambdaPtr id;
    LambdaPtr expr;

    friend LambdaPtr make_abstr(LambdaPtr id, LambdaPtr expr);
    friend void Id::substitute(LambdaPtr l);
    friend class App;

private:
    Abstr(LambdaPtr id, LambdaPtr expr);

    std::unordered_set<std::string> symbol_set;
};

LambdaPtr make_abstr(LambdaPtr id, LambdaPtr expr);


struct App {
    std::string to_string() const;
    void refresh_symbol();
    bool contain_symbol(std::string s) const { return symbol_set.count(s) > 0; }
    Util::UnorderedWeakPtrSet<LambdaPtr::element_type> childs() const;
    LambdaPtr clone() const;

    LambdaPtr left;
    LambdaPtr right;

    friend LambdaPtr make_app(LambdaPtr left, LambdaPtr right);
    friend void Id::substitute(LambdaPtr l);
    friend void Abstr::refresh_symbol();

private:
    App(LambdaPtr left, LambdaPtr right);

    std::unordered_set<std::string> symbol_set;
};

LambdaPtr make_app(LambdaPtr left, LambdaPtr right);

using Lambda = LambdaPtr::element_type;

std::string to_string(Lambda l);
LambdaPtr clone(LambdaPtr l);
LambdaPtr beta_reduction(LambdaPtr l);
}  // namespace Lambda
