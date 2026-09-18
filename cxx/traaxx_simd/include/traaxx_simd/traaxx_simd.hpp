#pragma once
#include <cstdint>
#include <expected>
#include <traaxx_simd/propagate.hpp>
#include <traaxx_simd/reorder_tables.hpp>
#include <utility>
#include <vector>

namespace traaxx_simd
{
    template<typename IndexT>
    struct MoveError
    {
        enum class Kind
        {
            would_create_cycle,
            tree_corrupted
        };
        Kind kind;
        IndexT iterations = 0;
    };

    template<typename T, typename IndexT = std::uint32_t>
    struct Tree
    {
        Tree() = default;
        Tree(std::vector<IndexT> parent, std::vector<IndexT> sibling, std::vector<T> data)
            : parent_(std::move(parent)), sibling_(std::move(sibling)), data_(std::move(data))
        {
        }
        const std::vector<IndexT> &parent() const
        {
            return parent_;
        }
        const std::vector<IndexT> &sibling() const
        {
            return sibling_;
        }
        const std::vector<T> &data() const
        {
            return data_;
        }
        IndexT nodes_count() const
        {
            return static_cast<IndexT>(parent_.size());
        }
        std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> bfs_reorder();
        std::expected<ReorderTables<IndexT>, ConvergenceError<IndexT>> dfs_reorder();
        IndexT append(IndexT p, T value = T{});
        void deleteLeaf(IndexT x);
        void splice(IndexT x);
        std::expected<void, ConvergenceError<IndexT>> deleteSubtree(IndexT x);
        std::expected<void, MoveError<IndexT>> move(IndexT x, IndexT q);

    private:
        std::vector<IndexT> parent_;
        std::vector<IndexT> sibling_;
        std::vector<T> data_;
    };
}
