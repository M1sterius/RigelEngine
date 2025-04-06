#pragma once

#include "Core.hpp"

#include <filesystem>
#include <utility>

namespace rge::backend
{
    class BackendModel
    {
    public:
        explicit BackendModel(std::filesystem::path path)
            : m_Path(std::move(path)) { }
        virtual ~BackendModel() = default;

        BackendModel(const BackendModel& other) = delete;
        BackendModel& operator = (const BackendModel&) = delete;
    protected:
        const std::filesystem::path m_Path;
    };
}
