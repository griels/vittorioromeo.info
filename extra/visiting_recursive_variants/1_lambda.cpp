// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

// #define VR_USE_BOOST_VARIANT

#include <iostream>
#include <vector>
#include <type_traits>
#include <boost/hana.hpp>
#include "variant_aliases.hpp"

namespace impl
{
    struct vnum_wrapper;

    using varr = std::vector<vnum_wrapper>;
    using vnum = vr::variant<int, float, double, varr>;

    struct vnum_wrapper
    {
        vnum _data;

        template <typename... Ts>
        vnum_wrapper(Ts&&... xs)
            : _data{FWD(xs)...}
        {
        }
    };
}

using vnum = impl::vnum;
using impl::varr;

template <typename TReturn, typename... TFs>
auto make_recursive_visitor(TFs&&... fs)
{
    auto res = boost::hana::fix([&fs...](auto self, auto&& x) -> TReturn
        {
            return boost::hana::overload(FWD(fs)...)(
                [&self](auto&& v)
                {
                    vr::visit_recursively(self, v);
                },
                FWD(x));
        });

    return res;
}

int main()
{
    // clang-format off
    auto vnp = make_recursive_visitor<void>
    (
        [](auto, int x)    { std::cout << x << "i\n"; },
        [](auto, float x)  { std::cout << x << "f\n"; },
        [](auto, double x) { std::cout << x << "d\n"; },

        [](auto visit_self, const varr& arr)
        {
            for(const auto& x : arr)
            {
                visit_self(x);
            }
        }
    );
    // clang-format on

    vnum v0{0};
    vr::visit(vnp, v0);

    v0 = 5.f;
    vr::visit(vnp, v0);

    v0 = 33.51;
    vr::visit(vnp, v0);

    v0 = varr{vnum{1}, vnum{2.0}, vnum{3.f}};
    vr::visit(vnp, v0);

    v0 = varr{vnum{5}, varr{vnum{7}, vnum{8.0}, vnum{9.}}, vnum{4.f}};
    vr::visit(vnp, v0);
}
