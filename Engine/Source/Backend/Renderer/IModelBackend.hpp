#pragma once

#include "Core.hpp"

#include <filesystem>
#include <utility>

namespace Rigel::Backend
{
    class IModelBackend
    {
    public:
        explicit IModelBackend(std::filesystem::path path)
            : m_Path(std::move(path)) { }
        virtual ~IModelBackend() = default;

        IModelBackend(const IModelBackend& other) = delete;
        IModelBackend& operator = (const IModelBackend&) = delete;
    protected:
        const std::filesystem::path m_Path;
    };
}
