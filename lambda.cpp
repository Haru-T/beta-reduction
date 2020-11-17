#include "lambda.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

namespace Lambda
{
const std::array<std::string, 27> symbol_candidate = {
    "x",
    "y",
    "z",
    "w",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "<No remaining symbol candidate>",
};

Abstr::Abstr(LambdaPtr id, LambdaPtr expr) : id(id),
                                             expr(expr), symbol_set()
{
    refresh_symbol();
}

App::App(LambdaPtr left, LambdaPtr right)
    : left(left), right(right), symbol_set()
{
    refresh_symbol();
}

void Abstr::refresh_symbol()
{
    symbol_set.insert(std::get<Id>(*id).symbol);
    std::visit([&](auto& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, Id>) {
            symbol_set.insert(x.symbol);
        } else {
            x.refresh_symbol();
            symbol_set.merge(x.symbol_set);
        }
    },
        *expr);
    /*
    if (std::holds_alternative<Id>(*expr)) {
        auto& p = std::get<Id>(*expr);
        symbol_set = {p.symbol};
    } else if (std::holds_alternative<Abstr>(*expr)) {
        auto& p = std::get<Abstr>(*expr);
        p.refresh_symbol();
        symbol_set = p.symbol_set;
    } else {
        auto& p = std::get<App>(*expr);
        p.refresh_symbol();
        symbol_set = p.symbol_set;
    }
    */
}

void App::refresh_symbol()
{
    for (auto& sp : std::array<LambdaPtr, 2>{left, right}) {
        std::visit([&](auto& x) {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, Id>) {
                symbol_set.insert(x.symbol);
            } else {
                x.refresh_symbol();
                symbol_set.merge(x.symbol_set);
            }
        },
            *sp);
        /*
        if (std::holds_alternative<Id>(*sp)) {
            auto& p = std::get<Id>(*sp);
            symbol_set.insert(p.symbol);
        } else if (std::holds_alternative<Abstr>(*sp)) {
            auto& p = std::get<Abstr>(*sp);
            p.refresh_symbol();
            symbol_set.merge(p.symbol_set);
        } else {
            auto& p = std::get<App>(*sp);
            p.refresh_symbol();
            symbol_set.merge(p.symbol_set);
        }
        */
    }
}

Util::UnorderedWeakPtrSet<LambdaPtr::element_type> Id::childs() const { return {}; }

Util::UnorderedWeakPtrSet<LambdaPtr::element_type> Abstr::childs() const
{
    auto set = std::visit([](auto& x) -> Util::UnorderedWeakPtrSet<LambdaPtr::element_type> { return x.childs(); }, *expr);
    set.insert(std::weak_ptr(expr));
    return set;
}

Util::UnorderedWeakPtrSet<LambdaPtr::element_type> App::childs() const
{
    auto set = std::visit([](auto& x) -> Util::UnorderedWeakPtrSet<LambdaPtr::element_type> { return x.childs(); }, *left);
    set.merge(std::visit([](auto& x) -> Util::UnorderedWeakPtrSet<LambdaPtr::element_type> { return x.childs(); }, *right));
    set.insert(std::weak_ptr(left));
    set.insert(std::weak_ptr(right));
    return set;
}


std::string Id::to_string() const
{
    return symbol;
}

std::string Abstr::to_string() const
{
    return "\\" + ::Lambda::to_string(*id) + ". " + ::Lambda::to_string(*expr);
}

std::string App::to_string() const
{
    std::string l, r;
    if (std::holds_alternative<Abstr>(*left))
        l = "(" + ::Lambda::to_string(*left) + ")";
    else
        l = ::Lambda::to_string(*left);

    if (std::holds_alternative<Id>(*right))
        r = ::Lambda::to_string(*right);
    else
        r = "(" + ::Lambda::to_string(*right) + ")";

    return l + " " + r;
}

std::string to_string(Lambda l)
{
    if (std::holds_alternative<Id>(l))
        return std::get<Id>(l).to_string();
    else if (std::holds_alternative<Abstr>(l))
        return std::get<Abstr>(l).to_string();
    else
        return std::get<App>(l).to_string();
}

LambdaPtr make_id(std::string symbol)
{
    return std::make_shared<Lambda>(Id(symbol));
}

LambdaPtr make_abstr(LambdaPtr id, LambdaPtr expr)
{
    if (!std::holds_alternative<Id>(*id))
        return nullptr;
    auto ptr = std::make_shared<Lambda>(Abstr(id, expr));
    if (std::holds_alternative<Id>(*expr))
        std::get<Id>(*expr).refs.emplace_back(1, ptr);
    return ptr;
}

LambdaPtr make_app(LambdaPtr left, LambdaPtr right)
{
    auto ptr = std::make_shared<Lambda>(App(left, right));
    if (std::holds_alternative<Id>(*left))
        std::get<Id>(*left).refs.emplace_back(0, ptr);
    if (std::holds_alternative<Id>(*right))
        std::get<Id>(*right).refs.emplace_back(1, ptr);
    return ptr;
}

LambdaPtr Id::clone(const Util::UnorderedWeakPtrSet<LambdaPtr::element_type>& refs_candidate)
{
#ifndef NDEBUG
    static int cnt = 0;
    auto ptr = make_id(symbol + "cl" + std::to_string(cnt++));
#else
    auto ptr = make_id(symbol);
#endif
    auto& new_id = std::get<Id>(*ptr);
    for (auto& ref : refs) {
        if (refs_candidate.count(ref.second) > 0) {
            new_id.refs.push_back(ref);
            ref.second.reset();
        }
    }
    new_id.substitute(ptr);
    return ptr;
}

std::vector<LambdaPtr> Id::clone_unique()
{
#ifndef NDEBUG
    static int cnt = 0;
#endif
    std::vector<LambdaPtr> clones;
    clones.reserve(refs.size());
    for (auto& ref : refs) {
#ifndef NDEBUG
        clones.push_back(make_id(symbol + "uq" + std::to_string(cnt++)));
#else
        clones.push_back(make_id(symbol));
#endif
        auto& clone = std::get<Id>(*clones.back());
        clone.refs = {ref};
        clone.substitute(clones.back());
        ref.second.reset();
    }
    return clones;
}

LambdaPtr Id::clone() const { return nullptr; }

LambdaPtr Abstr::clone() const
{
    auto ptr = make_abstr(id, ::Lambda::clone(expr));
    auto& new_abstr = std::get<Abstr>(*ptr);
    auto substitution_targets = new_abstr.childs();
    substitution_targets.emplace(ptr);
    new_abstr.id = std::get<Id>(*id).clone(std::move(substitution_targets));
#ifndef NDEBUG
    std::cerr << "clone abstr: " << to_string() << " -> " << new_abstr.to_string() << std::endl;
#endif
    return ptr;
}

LambdaPtr App::clone() const
{
#ifndef NDEBUG
    std::cerr << "clone app: " << to_string() << std::endl;
#endif
    return make_app(::Lambda::clone(left), ::Lambda::clone(right));
}

void Id::substitute(LambdaPtr l)
{
#ifndef NDEBUG
    std::cerr << "substituion query: " << symbol << " -> " << ::Lambda::to_string(*l) << std::endl;
#endif
    for (auto& [idx, wp] : refs) {
        if (auto sp = wp.lock()) {
#ifndef NDEBUG
            std::string before;
            bool was_abstr = std::holds_alternative<Abstr>(*sp);
            if (was_abstr)
                before = ::Lambda::to_string(*std::get<Abstr>(*sp).expr);
            else
                before = ::Lambda::to_string(*sp);
#endif
            if (std::holds_alternative<App>(*sp)) {
                if (idx == 0)
                    std::get<App>(*sp).left = l;
                else
                    std::get<App>(*sp).right = l;
            } else {
                std::get<Abstr>(*sp).expr = l;
            }
#ifndef NDEBUG
            std::string after;
            if (was_abstr)
                after = ::Lambda::to_string(*std::get<Abstr>(*sp).expr);
            else
                after = ::Lambda::to_string(*sp);
            std::cerr << "substitution result: " << before
                      << " (was " << (was_abstr ? "Abstr" : "App")
                      << ") -> "
                      << after << std::endl;
#endif
        }
    }
}

LambdaPtr clone(LambdaPtr l)
{
    if (std::holds_alternative<Id>(*l))
        return l;
    else
        return std::visit([](auto& x) { return x.clone(); }, *l);
}

LambdaPtr beta_reduction(LambdaPtr l)
{
#ifndef NDEBUG
    std::string before = to_string(*l);
    std::string type;
    if (std::holds_alternative<Id>(*l))
        type = "Id";
    else if (std::holds_alternative<Abstr>(*l))
        type = "Abstr";
    else
        type = "App";
    std::cerr << "beta reduction: " << before << " (is " << type << ")" << std::endl;
#endif
    while (std::holds_alternative<App>(*l)) {
        auto& app = std::get<App>(*l);
        if (std::holds_alternative<Abstr>(*app.left)) {
            auto& abstr = std::get<Abstr>(*app.left);
            auto& id = std::get<Id>(*abstr.id);
            if (std::holds_alternative<Abstr>(*abstr.expr)) {
                std::string& symbol = std::get<Id>(*std::get<Abstr>(*abstr.expr).id).symbol;
                if (std::visit([&](auto& x) { return x.contain_symbol(symbol); },
                        *app.right)) {
#ifndef NDEBUG
                    std::string before = abstr.to_string();
#endif
                    for (const auto& s : symbol_candidate) {
                        if (s == symbol_candidate.back()) {
                            symbol = s;
                            std::get<Abstr>(*abstr.expr).refresh_symbol();
#ifndef NDEBUG
                            std::cerr << "all symbol candidates conflicted: " << before << " -> " << abstr.to_string() << std::endl;
#endif
                            break;
                        }
                        if (s != id.symbol && !std::visit([&](auto& x) { return x.contain_symbol(s); }, *app.right)) {
                            symbol = s;
                            std::get<Abstr>(*abstr.expr).refresh_symbol();
#ifndef NDEBUG
                            std::cerr << "symbol conflict resolved: " << before << " -> " << abstr.to_string() << std::endl;
#endif
                            break;
                        }
                    }
                }
            }
            if (abstr.id.use_count() > 2 && std::holds_alternative<Abstr>(*app.right)) {
#ifndef NDEBUG
                std::cerr << abstr.id.use_count() - 1 << " substitution of abstractions: " << to_string(*l) << std::endl;
#endif
                auto clones = id.clone_unique();
                for (auto&& c : clones) {
                    auto& clone_id = std::get<Id>(*c);
                    clone_id.substitute(clone(app.right));
                }
            } else {
                id.substitute(app.right);
            }
            l = abstr.expr;
            continue;
        }
        app.left = beta_reduction(app.left);
        app.right = beta_reduction(app.right);
        if (!std::holds_alternative<Abstr>(*app.left))
            break;
    }
    if (std::holds_alternative<Abstr>(*l)) {
        auto& abstr = std::get<Abstr>(*l);
        abstr.expr = beta_reduction(abstr.expr);
    }
#ifndef NDEBUG
    std::cerr << "beta reduction result: " << before << " -> " << to_string(*l) << std::endl;
#endif
    return l;
}
}  // namespace Lambda
