#pragma once
#include <array>
#include <type_traits>
#include <utility>

namespace traaxx
{
    struct NoOpFn
    {
        template<typename Input>
        void operator()(Input &&) const
        {
        }
    };

    template<typename FnPredicate, typename FnTrue, typename FnFalse = NoOpFn>
    class DispatchBool final
    {
    public:
        DispatchBool(FnPredicate fn_predicate, FnTrue fn_when_true)
            : fn_predicate_(std::move(fn_predicate)), fn_when_true_(std::move(fn_when_true)), fn_when_false_()
        {
        }

        DispatchBool(FnPredicate fn_predicate, FnTrue fn_when_true, FnFalse fn_when_false)
            : fn_predicate_(std::move(fn_predicate)), fn_when_true_(std::move(fn_when_true)), fn_when_false_(std::move(fn_when_false))
        {
        }

    public:
        template<typename Input>
        auto operator()(Input &&input) const
        {
            using ReturnType = std::invoke_result_t<FnTrue, Input>;
            static_assert(
                !std::is_same_v<FnFalse, NoOpFn> || std::is_void_v<ReturnType>,
                "the two-argument DispatchBool (no explicit fn_when_false) only supports a void-returning fn_when_true, "
                "since there is no sensible default value for the false branch to return");
            using FF = ReturnType (*)(DispatchBool const *, Input &&);
            static constexpr auto table = std::array<FF, 2>{
                [](DispatchBool const *self, Input &&value) -> ReturnType { return self->fn_when_false_(std::forward<Input>(value)); },
                [](DispatchBool const *self, Input &&value) -> ReturnType { return self->fn_when_true_(std::forward<Input>(value)); }
            };
            return table[fn_predicate_(input)](this, std::forward<Input>(input));
        }

    private:
        FnPredicate fn_predicate_;
        FnTrue fn_when_true_;
        FnFalse fn_when_false_;
    };
}
