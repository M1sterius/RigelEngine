#pragma once

#include "ErrorCodes.hpp"
#include "Assert.hpp"

#include <optional>
#include <utility>

namespace Rigel
{
    template <typename T>
    class Result
    {
    public:
        static Result Ok(T value)
        {
            return Result(std::move(value), ErrorCode::OK);
        }

        static Result Error(const ErrorCode errorCode)
        {
            return Result(std::nullopt, errorCode);
        }

        [[nodiscard]] bool IsOk() const { return m_ErrorCode == ErrorCode::OK; }
        [[nodiscard]] bool IsError() const { return m_ErrorCode != ErrorCode::OK; }

        [[nodiscard]] ErrorCode GetError() const { return m_ErrorCode; }

        T& Value()
        {
            ASSERT(IsOk(), "Attempting to access value on error Result");
            return *m_Value;
        }

        const T& Value() const
        {
            ASSERT(IsOk(),  "Attempting to access value on error Result");
            return *m_Value;
        }

        std::optional<T> TryGetValue() const
        {
            return m_Value;
        }
    private:
        std::optional<T> m_Value;
        ErrorCode m_ErrorCode;

        Result(std::optional<T> value, const ErrorCode errorCode)
            : m_Value(std::move(value)), m_ErrorCode(errorCode) { }
    };

    // explicit specialization for void makes it possible to return an error code withput any real data
    template <>
    class Result<void>
    {
    public:
        static Result Ok()
        {
            return Result(ErrorCode::OK);
        }

        static Result Error(const ErrorCode errorCode)
        {
            return Result(errorCode);
        }

        [[nodiscard]] bool IsOk() const { return m_ErrorCode == ErrorCode::OK; }
        [[nodiscard]] bool IsError() const { return m_ErrorCode != ErrorCode::OK; }
        [[nodiscard]] ErrorCode GetError() const { return m_ErrorCode; }
    private:
        explicit Result(const ErrorCode errorCode)
            : m_ErrorCode(errorCode) { }

        ErrorCode m_ErrorCode;
    };

}
