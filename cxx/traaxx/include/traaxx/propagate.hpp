#pragma once
#include <expected>
#include <utility>

namespace traaxx
{
    template<typename IndexT>
    struct ConvergenceError
    {
        IndexT iterations = 0;
    };

    template<typename StateT, typename IndexT>
    struct PropagationResult
    {
        StateT state;
        IndexT iterations = 0;
    };

    template<typename StateT, typename IndexT, typename StepFn>
    std::expected<PropagationResult<StateT, IndexT>, ConvergenceError<IndexT>> propagate(
        StateT initial, StepFn step, IndexT max_iterations)
    {
        auto current = std::move(initial);
        auto next = current;
        for (IndexT iteration = 0; iteration < max_iterations; ++iteration)
        {
            step(current, next);
            auto const changed = !(current == next);
            current.swap(next);
            if (!changed)
            {
                return PropagationResult<StateT, IndexT>{ std::move(current),
                    static_cast<IndexT>(iteration + IndexT{ 1 }) };
            }
        }
        return std::unexpected(ConvergenceError<IndexT>{ max_iterations });
    }
}
