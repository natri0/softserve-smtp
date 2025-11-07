#pragma once

#include <concepts>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#ifndef RESULT_CONCAT
    #define RESULT_CONCAT_IMPL(a, b) a##b
    #define RESULT_CONCAT(a, b) RESULT_CONCAT_IMPL(a, b)
#endif

#ifndef RESULT_THROW_IMPL
    #if __cpp_exceptions
        #define RESULT_THROW_IMPL(expr) throw expr
    #else
        #define RESULT_THROW_IMPL(expr) std::terminate()
    #endif
#endif

#ifndef RESULT_ASSUME
    #if defined(__GNUC__) || defined(__clang__)
        #define RESULT_ASSUME(cond) __builtin_assume(cond)
    #elif defined(_MSC_VER)
        #define RESULT_ASSUME(cond) __assume(cond)
    #else
        #define RESULT_ASSUME(cond) ((void)0)
    #endif
#endif

#ifndef RESULT_DEBUG_UNWRAP_CHECK
    #ifdef NDEBUG
        #define RESULT_DEBUG_UNWRAP_CHECK(cond, expr) RESULT_ASSUME(cond)
    #else
        #define RESULT_DEBUG_UNWRAP_CHECK(cond, expr) \
            do {                                      \
                if (!(cond)) {                        \
                    RESULT_THROW_IMPL(expr);          \
                }                                     \
            } while (0)
    #endif
#endif

#ifndef RESULT_UNWRAP
    #define RESULT_UNWRAP(...) \
        if (auto res = __VA_ARGS__; res.isErr()) return std::move(res).asErr()
#endif

#ifndef RESULT_UNWRAP_INTO
    #define RESULT_UNWRAP_INTO(var, ...) \
        auto RESULT_CONCAT(_res_, __LINE__) = __VA_ARGS__; \
        if (RESULT_CONCAT(_res_, __LINE__).isErr()) \
            return std::move(RESULT_CONCAT(_res_, __LINE__)).asErr(); \
        var = std::move(RESULT_CONCAT(_res_, __LINE__)).unwrapUnchecked()
#endif

#ifndef RESULT_UNWRAP_ERR_INTO
    #define RESULT_UNWRAP_ERR_INTO(var, ...) \
        auto RESULT_CONCAT(_res_, __LINE__) = __VA_ARGS__; \
        if (RESULT_CONCAT(_res_, __LINE__).isErr()) \
            var = std::move(RESULT_CONCAT(_res_, __LINE__)).unwrapErrUnchecked()
#endif

#ifndef RESULT_UNWRAP_INTO_EITHER
    #define RESULT_UNWRAP_INTO_EITHER(okVar, errVar, ...) \
        auto RESULT_CONCAT(_res_, __LINE__) = __VA_ARGS__; \
        RESULT_CONCAT(_res_, __LINE__).isOk() && \
            (okVar = std::move(RESULT_CONCAT(_res_, __LINE__)).unwrapUnchecked(), true) || \
            (errVar = std::move(RESULT_CONCAT(_res_, __LINE__)).unwrapErrUnchecked(), false)
#endif

#ifndef RESULT_UNWRAP_RETURN
    #define RESULT_UNWRAP_RETURN(...) \
        auto RESULT_CONCAT(_res_, __LINE__) = __VA_ARGS__; \
        if (RESULT_CONCAT(_res_, __LINE__).isErr()) \
            return std::move(RESULT_CONCAT(_res_, __LINE__)).asErr(); \
        return std::move(RESULT_CONCAT(_res_, __LINE__)).asOk()
#endif

template <class OkType, class ErrType>
class Result;

namespace detail {
    template <class OkType>
    class OkWrapper;

    template <class ErrType>
    class ErrWrapper;

    template <class OkType, class ErrType>
    class ResultData;

    template <class OkType, class ErrType>
    class ResultDataWrapper;
}

template <class OkType>
constexpr detail::OkWrapper<OkType> Ok(OkType&& value);

template <class ErrType>
constexpr detail::ErrWrapper<ErrType> Err(ErrType&& error);

template <class OkType>
constexpr detail::OkWrapper<OkType&> Ok(OkType& value);

template <class ErrType>
constexpr detail::ErrWrapper<ErrType&> Err(ErrType& error);

constexpr detail::OkWrapper<void> Ok();

constexpr detail::ErrWrapper<void> Err();

namespace detail {
    template <class OkType>
    class OkWrapper final {
    protected:
        OkType m_value;

        constexpr explicit OkWrapper(OkType&& value) noexcept(std::is_nothrow_move_constructible_v<OkType>)
            : m_value(std::forward<OkType>(value)) {}

        constexpr explicit OkWrapper(OkType const& value) noexcept(std::is_nothrow_copy_constructible_v<OkType>)
            : m_value(value) {}

        friend constexpr OkWrapper Ok<OkType>(OkType&& value);

    public:
        constexpr OkType&& unwrap() && noexcept { return std::move(m_value); }
        constexpr OkType const& unwrap() const & noexcept { return m_value; }
        constexpr OkType& unwrap() & noexcept { return m_value; }
    };

    template <class OkType>
    class OkWrapper<OkType&> final {
    protected:
        OkType& m_value;

        constexpr explicit OkWrapper(OkType& value) noexcept : m_value(value) {}

        friend constexpr OkWrapper Ok<OkType>(OkType& value);

    public:
        constexpr std::reference_wrapper<OkType> unwrap() && noexcept { return m_value; }
        constexpr std::reference_wrapper<OkType> unwrap() const & noexcept { return m_value; }
    };

    template <>
    class OkWrapper<void> final {
    protected:
        std::monostate m_value;

        constexpr explicit OkWrapper() noexcept = default;

        friend constexpr OkWrapper (::Ok)();
    };

    template <class ErrType>
    class ErrWrapper final {
    protected:
        ErrType m_error;

        constexpr explicit ErrWrapper(ErrType&& error) noexcept(std::is_nothrow_move_constructible_v<ErrType>)
            : m_error(std::forward<ErrType>(error)) {}

        constexpr explicit ErrWrapper(ErrType const& error) noexcept(std::is_nothrow_copy_constructible_v<ErrType>)
            : m_error(error) {}

        friend constexpr ErrWrapper Err<ErrType>(ErrType&& error);

    public:
        constexpr ErrType&& unwrap() && noexcept { return std::move(m_error); }
        constexpr ErrType const& unwrap() const & noexcept { return m_error; }
        constexpr ErrType& unwrap() & noexcept { return m_error; }
    };

    template <class ErrType>
    class ErrWrapper<ErrType&> final {
    protected:
        ErrType& m_error;

        constexpr explicit ErrWrapper(ErrType& error) noexcept : m_error(error) {}

        friend constexpr ErrWrapper Err<ErrType>(ErrType& error);

    public:
        constexpr std::reference_wrapper<ErrType> unwrap() && noexcept { return m_error; }
        constexpr std::reference_wrapper<ErrType> unwrap() const & noexcept { return m_error; }
    };

    template <>
    class ErrWrapper<void> final {
    protected:
        std::monostate m_error;

        constexpr explicit ErrWrapper() noexcept = default;

        friend constexpr ErrWrapper (::Err)();
    };

    template <class T>
    concept IsStringifiable = requires(std::stringstream ss, T t) { ss << t; };

    struct OkTag {};

    struct ErrTag {};
}

template <class OkType>
constexpr detail::OkWrapper<OkType> Ok(OkType&& value) {
    return detail::OkWrapper<OkType>(std::forward<OkType>(value));
}

template <class ErrType>
constexpr detail::ErrWrapper<ErrType> Err(ErrType&& error) {
    return detail::ErrWrapper<ErrType>(std::forward<ErrType>(error));
}

template <class OkType>
constexpr detail::OkWrapper<OkType&> Ok(OkType& value) {
    return detail::OkWrapper<OkType&>(value);
}

template <class ErrType>
constexpr detail::ErrWrapper<ErrType&> Err(ErrType& error) {
    return detail::ErrWrapper<ErrType&>(error);
}

constexpr detail::OkWrapper<void> Ok() {
    return detail::OkWrapper<void>();
}

constexpr detail::ErrWrapper<void> Err() {
    return detail::ErrWrapper<void>();
}

class UnwrapException final : public std::runtime_error {
public:
    template <class T> requires(detail::IsStringifiable<T>)
    UnwrapException(detail::ErrTag, T&& err) : std::runtime_error(
        (std::stringstream{} << "Called .unwrap() on an Err value: " << std::forward<T>(err)).str()
    ) {}

    template <class T> requires(!detail::IsStringifiable<T>)
    UnwrapException(detail::ErrTag, T&&) : std::runtime_error("Called .unwrap() on an Err value") {}

    template <class T> requires(detail::IsStringifiable<T>)
    UnwrapException(detail::OkTag, T&& ok) : std::runtime_error(
        (std::stringstream{} << "Called .unwrapErr() on an Ok value: " << std::forward<T>(ok)).str()
    ) {}

    template <class T> requires(!detail::IsStringifiable<T>)
    UnwrapException(detail::OkTag, T&&) : std::runtime_error("Called .unwrapErr() on an Ok value") {}

    UnwrapException(UnwrapException&&) = default;
    UnwrapException(UnwrapException const&) = default;
    ~UnwrapException() override = default;
};

namespace detail {
    template <class OkType, class ErrType>
    class ResultData {
    protected:
        using OkTypeWrapper = std::conditional_t<
            std::is_reference_v<OkType>,
            std::reference_wrapper<std::remove_reference_t<OkType>>, OkType>;
        using ErrTypeWrapper = std::conditional_t<
            std::is_reference_v<ErrType>,
            std::reference_wrapper<std::remove_reference_t<ErrType>>, ErrType>;

        union {
            OkTypeWrapper m_ok;
            ErrTypeWrapper m_err;
        };

        bool m_isOk;

        template <class T>
        constexpr explicit ResultData(OkTag&&, T&& ok) noexcept(std::is_nothrow_constructible_v<OkTypeWrapper, T>)
            : m_ok(std::forward<T>(ok)), m_isOk(true) {}

        template <class T>
        constexpr explicit ResultData(ErrTag&&, T&& err) noexcept(std::is_nothrow_constructible_v<ErrTypeWrapper, T>)
            : m_err(std::forward<T>(err)), m_isOk(false) {}

        constexpr ResultData(
            ResultData&& other
        ) noexcept(
            std::is_nothrow_move_constructible_v<OkTypeWrapper> &&
            std::is_nothrow_move_constructible_v<ErrTypeWrapper>)
            : m_isOk(other.m_isOk) {
            if (m_isOk) {
                std::construct_at(std::addressof(m_ok), std::move(other.m_ok));
            } else {
                std::construct_at(std::addressof(m_err), std::move(other.m_err));
            }
        }

        constexpr ResultData(
            ResultData const& other
        ) noexcept(
            std::is_nothrow_copy_constructible_v<OkTypeWrapper> &&
            std::is_nothrow_copy_constructible_v<ErrTypeWrapper>)
            : m_isOk(other.m_isOk) {
            if (m_isOk) {
                std::construct_at(std::addressof(m_ok), other.m_ok);
            } else {
                std::construct_at(std::addressof(m_err), other.m_err);
            }
        }

        constexpr ResultData& operator=(
            ResultData&& other
        ) noexcept(
            std::is_nothrow_move_constructible_v<OkTypeWrapper> &&
            std::is_nothrow_move_constructible_v<ErrTypeWrapper> &&
            std::is_nothrow_move_assignable_v<OkTypeWrapper> &&
            std::is_nothrow_move_assignable_v<ErrTypeWrapper>) {
            if (this == &other) return *this;
            if (m_isOk && other.m_isOk) {
                m_ok = std::move(other.m_ok);
            } else if (!m_isOk && !other.m_isOk) {
                m_err = std::move(other.m_err);
            } else {
                destroyActive();
                m_isOk = other.m_isOk;
                if (m_isOk) {
                    std::construct_at(std::addressof(m_ok), std::move(other.m_ok));
                } else {
                    std::construct_at(std::addressof(m_err), std::move(other.m_err));
                }
            }
            return *this;
        }

        constexpr ResultData& operator=(
            ResultData const& other
        ) noexcept(
            std::is_nothrow_copy_constructible_v<OkTypeWrapper> &&
            std::is_nothrow_copy_constructible_v<ErrTypeWrapper> &&
            std::is_nothrow_copy_assignable_v<OkTypeWrapper> &&
            std::is_nothrow_copy_assignable_v<ErrTypeWrapper>) {
            if (this == &other) return *this;
            if (m_isOk && other.m_isOk) {
                m_ok = other.m_ok;
            } else if (!m_isOk && !other.m_isOk) {
                m_err = other.m_err;
            } else {
                destroyActive();
                m_isOk = other.m_isOk;
                if (m_isOk) {
                    std::construct_at(std::addressof(m_ok), other.m_ok);
                } else {
                    std::construct_at(std::addressof(m_err), other.m_err);
                }
            }
            return *this;
        }

        constexpr void destroyActive() noexcept {
            if (m_isOk) {
                if constexpr (!std::is_trivially_destructible_v<OkTypeWrapper>)
                    std::destroy_at(std::addressof(m_ok));
            } else {
                if constexpr (!std::is_trivially_destructible_v<ErrTypeWrapper>)
                    std::destroy_at(std::addressof(m_err));
            }
        }

        ~ResultData() {
            destroyActive();
        }

    public:
        [[nodiscard]] constexpr OkWrapper<OkType> asOk() && noexcept
            requires(!std::is_reference_v<OkType>) { return Ok(std::move(m_ok)); }

        [[nodiscard]] constexpr OkWrapper<OkType&> asOk() && noexcept
            requires(std::is_reference_v<OkType>) { return Ok(m_ok); }

        [[nodiscard]] constexpr OkWrapper<OkType> asOk() const & noexcept
            requires(!std::is_reference_v<OkType>) { return Ok(m_ok); }

        [[nodiscard]] constexpr OkWrapper<OkType&> asOk() const & noexcept
            requires(std::is_reference_v<OkType>) { return Ok(m_ok); }

        [[nodiscard]] constexpr ErrWrapper<ErrType> asErr() && noexcept
            requires(!std::is_reference_v<ErrType>) { return Err(std::move(m_err)); }

        [[nodiscard]] constexpr ErrWrapper<ErrType&> asErr() && noexcept
            requires(std::is_reference_v<ErrType>) { return Err(m_err); }

        [[nodiscard]] constexpr ErrWrapper<ErrType> asErr() const & noexcept
            requires(!std::is_reference_v<ErrType>) { return Err(m_err); }

        [[nodiscard]] constexpr ErrWrapper<ErrType&> asErr() const & noexcept
            requires(std::is_reference_v<ErrType>) { return Err(m_err); }

        [[nodiscard]] constexpr bool isOk() const noexcept { return m_isOk; }
        [[nodiscard]] constexpr bool isErr() const noexcept { return !m_isOk; }

        [[nodiscard]] constexpr explicit(false) operator bool() const noexcept { return isOk(); }

        constexpr OkType&& unwrap() && {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
            return std::move(m_ok);
        }

        constexpr OkType& unwrap() & {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
            return m_ok;
        }

        constexpr OkType const& unwrap() const & {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
            return m_ok;
        }

        [[nodiscard]] constexpr OkType&& unwrapUnchecked() && {
            RESULT_DEBUG_UNWRAP_CHECK(m_isOk, UnwrapException(ErrTag{}, m_err));
            return std::move(m_ok);
        }

        [[nodiscard]] constexpr OkType& unwrapUnchecked() & {
            RESULT_DEBUG_UNWRAP_CHECK(m_isOk, UnwrapException(ErrTag{}, m_err));
            return m_ok;
        }

        [[nodiscard]] constexpr OkType const& unwrapUnchecked() const & {
            RESULT_DEBUG_UNWRAP_CHECK(m_isOk, UnwrapException(ErrTag{}, m_err));
            return m_ok;
        }

        constexpr ErrType&& unwrapErr() && {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
            return std::move(m_err);
        }

        constexpr ErrType& unwrapErr() & {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
            return m_err;
        }

        constexpr ErrType const& unwrapErr() const & {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
            return m_err;
        }

        [[nodiscard]] constexpr ErrType&& unwrapErrUnchecked() && {
            RESULT_DEBUG_UNWRAP_CHECK(!m_isOk, UnwrapException(OkTag{}, m_ok));
            return std::move(m_err);
        }

        [[nodiscard]] constexpr ErrType& unwrapErrUnchecked() & {
            RESULT_DEBUG_UNWRAP_CHECK(!m_isOk, UnwrapException(OkTag{}, m_ok));
            return m_err;
        }

        [[nodiscard]] constexpr ErrType const& unwrapErrUnchecked() const & {
            RESULT_DEBUG_UNWRAP_CHECK(!m_isOk, UnwrapException(OkTag{}, m_ok));
            return m_err;
        }

        [[nodiscard]] OkType unwrapOr(OkType&& defaultValue) && {
            if (this->isOk()) {
                return std::move(this->m_ok);
            }
            return std::forward<OkType>(defaultValue);
        }

        [[nodiscard]] OkType unwrapOr(OkType const& defaultValue) const & {
            if (this->isOk()) {
                return this->m_ok;
            }
            return defaultValue;
        }

        [[nodiscard]] OkType unwrapOrDefault() && requires std::default_initializable<OkType> {
            if (this->isOk()) {
                return std::move(this->m_ok);
            }
            return OkType{};
        }

        [[nodiscard]] OkType unwrapOrDefault() const & requires std::default_initializable<OkType> {
            if (this->isOk()) {
                return this->m_ok;
            }
            return OkType{};
        }

        [[nodiscard]] OkType unwrapOrElse(auto&& func) && {
            if (this->isOk()) {
                return std::move(this->m_ok);
            }
            return func();
        }

        [[nodiscard]] OkType unwrapOrElse(auto&& func) const & {
            if (this->isOk()) {
                return this->m_ok;
            }
            return func();
        }

        void expect(std::string_view message) const {
            if (isErr()) {
                if constexpr (IsStringifiable<ErrType>) {
                    RESULT_THROW_IMPL(std::runtime_error(
                        std::string(message) + ": " +
                        (std::stringstream{} << m_err).str()
                    ));
                } else {
                    RESULT_THROW_IMPL(std::runtime_error(
                        std::string(message)
                    ));
                }
            }
        }
    };

    template <class ErrType>
    class ResultData<void, ErrType> {
    protected:
        using ErrTypeWrapper = std::conditional_t<
            std::is_reference_v<ErrType>,
            std::reference_wrapper<std::remove_reference_t<ErrType>>, ErrType>;

        union {
            std::monostate m_ok;
            ErrTypeWrapper m_err;
        };

        bool m_isOk;

        template <class T>
        constexpr explicit ResultData(
            ErrTag&&, T&& err
        ) noexcept(std::is_nothrow_constructible_v<ErrTypeWrapper, T>)
            : m_err(std::forward<T>(err)), m_isOk(false) {}

        constexpr explicit ResultData(OkTag&&) noexcept : m_isOk(true) {}

        constexpr ResultData(
            ResultData&& other
        ) noexcept(
            std::is_nothrow_move_constructible_v<ErrTypeWrapper>)
            : m_isOk(other.m_isOk) {
            if (m_isOk) {
                std::construct_at(std::addressof(m_ok));
            } else {
                std::construct_at(std::addressof(m_err), std::move(other.m_err));
            }
        }

        constexpr ResultData(
            ResultData const& other
        ) noexcept(
            std::is_nothrow_copy_constructible_v<ErrTypeWrapper>)
            : m_isOk(other.m_isOk) {
            if (m_isOk) {
                std::construct_at(std::addressof(m_ok));
            } else {
                std::construct_at(std::addressof(m_err), other.m_err);
            }
        }

        constexpr ResultData& operator=(
            ResultData&& other
        ) noexcept(
            std::is_nothrow_move_constructible_v<ErrTypeWrapper> &&
            std::is_nothrow_move_assignable_v<ErrTypeWrapper>) {
            if (this == &other) return *this;
            if (m_isOk && other.m_isOk) {
                // nothing
            } else if (!m_isOk && !other.m_isOk) {
                m_err = std::move(other.m_err);
            } else {
                destroyActive();
                m_isOk = other.m_isOk;
                if (m_isOk) {
                    std::construct_at(std::addressof(m_ok));
                } else {
                    std::construct_at(std::addressof(m_err), std::move(other.m_err));
                }
            }
            return *this;
        }

        constexpr ResultData& operator=(
            ResultData const& other
        ) noexcept(
            std::is_nothrow_copy_constructible_v<ErrTypeWrapper> &&
            std::is_nothrow_copy_assignable_v<ErrTypeWrapper>) {
            if (this == &other) return *this;
            if (m_isOk && other.m_isOk) {
                // nothing
            } else if (!m_isOk && !other.m_isOk) {
                m_err = other.m_err;
            } else {
                destroyActive();
                m_isOk = other.m_isOk;
                if (m_isOk) {
                    std::construct_at(std::addressof(m_ok));
                } else {
                    std::construct_at(std::addressof(m_err), other.m_err);
                }
            }
            return *this;
        }

        constexpr void destroyActive() noexcept {
            if (!m_isOk) {
                if constexpr (!std::is_trivially_destructible_v<ErrTypeWrapper>)
                    std::destroy_at(std::addressof(m_err));
            }
        }

        ~ResultData() { destroyActive(); }

    public:
        [[nodiscard]] constexpr OkWrapper<void> asOk() const noexcept { return Ok(); }

        [[nodiscard]] constexpr ErrWrapper<ErrType> asErr() && noexcept
            requires(!std::is_reference_v<ErrType>) { return Err(std::move(m_err)); }

        [[nodiscard]] constexpr ErrWrapper<ErrType&> asErr() && noexcept
            requires(std::is_reference_v<ErrType>) { return Err(m_err); }

        [[nodiscard]] constexpr ErrWrapper<ErrType> asErr() const & noexcept
            requires(!std::is_reference_v<ErrType>) { return Err(m_err); }

        [[nodiscard]] constexpr ErrWrapper<ErrType&> asErr() const & noexcept
            requires(std::is_reference_v<ErrType>) { return Err(m_err); }

        [[nodiscard]] constexpr bool isOk() const noexcept { return m_isOk; }
        [[nodiscard]] constexpr bool isErr() const noexcept { return !m_isOk; }

        [[nodiscard]] constexpr explicit(false) operator bool() const noexcept { return isOk(); }

        constexpr void unwrap() const {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
        }

        constexpr void unwrap() {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
        }

        constexpr ErrType&& unwrapErr() && {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
            return std::move(m_err);
        }

        constexpr ErrType& unwrapErr() & {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
            return m_err;
        }

        constexpr ErrType const& unwrapErr() const & {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
            return m_err;
        }

        [[nodiscard]] constexpr ErrType&& unwrapErrUnchecked() && {
            RESULT_DEBUG_UNWRAP_CHECK(!m_isOk, UnwrapException(OkTag{}, m_ok));
            return std::move(m_err);
        }

        [[nodiscard]] constexpr ErrType& unwrapErrUnchecked() & {
            RESULT_DEBUG_UNWRAP_CHECK(!m_isOk, UnwrapException(OkTag{}, m_ok));
            return m_err;
        }

        [[nodiscard]] constexpr ErrType const& unwrapErrUnchecked() const & {
            RESULT_DEBUG_UNWRAP_CHECK(!m_isOk, UnwrapException(OkTag{}, m_ok));
            return m_err;
        }

        void expect(std::string_view message) const {
            if (isErr()) {
                if constexpr (IsStringifiable<ErrType>) {
                    RESULT_THROW_IMPL(std::runtime_error(
                        std::string(message) + ": " +
                        (std::stringstream{} << m_err).str()
                    ));
                } else {
                    RESULT_THROW_IMPL(std::runtime_error(
                        std::string(message)
                    ));
                }
            }
        }
    };

    template <class OkType>
    class ResultData<OkType, void> {
    protected:
        using OkTypeWrapper = std::conditional_t<
            std::is_reference_v<OkType>,
            std::reference_wrapper<std::remove_reference_t<OkType>>, OkType>;

        union {
            OkTypeWrapper m_ok;
            std::monostate m_err;
        };

        bool m_isOk;

        template <class T>
        constexpr explicit ResultData(
            OkTag&&, T&& ok
        )
            noexcept(std::is_nothrow_constructible_v<OkTypeWrapper, T>)
            : m_ok(std::forward<T>(ok)), m_isOk(true) {}

        constexpr explicit ResultData(ErrTag&&) noexcept : m_isOk(false) {}

        constexpr ResultData(
            ResultData&& other
        ) noexcept(
            std::is_nothrow_move_constructible_v<OkTypeWrapper>)
            : m_isOk(other.m_isOk) {
            if (m_isOk) {
                std::construct_at(std::addressof(m_ok), std::move(other.m_ok));
            } else {
                std::construct_at(std::addressof(m_err));
            }
        }

        constexpr ResultData(
            ResultData const& other
        ) noexcept(
            std::is_nothrow_copy_constructible_v<OkTypeWrapper>)
            : m_isOk(other.m_isOk) {
            if (m_isOk) {
                std::construct_at(std::addressof(m_ok), other.m_ok);
            } else {
                std::construct_at(std::addressof(m_err));
            }
        }

        constexpr ResultData& operator=(
            ResultData&& other
        ) noexcept(
            std::is_nothrow_move_constructible_v<OkTypeWrapper> &&
            std::is_nothrow_move_assignable_v<OkTypeWrapper>) {
            if (this == &other) return *this;
            if (m_isOk && other.m_isOk) {
                m_ok = std::move(other.m_ok);
            } else if (!m_isOk && !other.m_isOk) {
                // nothing
            } else {
                destroyActive();
                m_isOk = other.m_isOk;
                if (m_isOk) std::construct_at(std::addressof(m_ok), std::move(other.m_ok));
                else std::construct_at(std::addressof(m_err));
            }
            return *this;
        }

        constexpr ResultData& operator=(
            ResultData const& other
        ) noexcept(
            std::is_nothrow_copy_constructible_v<OkTypeWrapper> &&
            std::is_nothrow_copy_assignable_v<OkTypeWrapper>) {
            if (this == &other) return *this;
            if (m_isOk && other.m_isOk) {
                m_ok = other.m_ok;
            } else if (!m_isOk && !other.m_isOk) {
                // nothing
            } else {
                destroyActive();
                m_isOk = other.m_isOk;
                if (m_isOk) std::construct_at(std::addressof(m_ok), other.m_ok);
                else std::construct_at(std::addressof(m_err));
            }
            return *this;
        }

        constexpr void destroyActive() noexcept {
            if (m_isOk) {
                if constexpr (!std::is_trivially_destructible_v<OkTypeWrapper>)
                    std::destroy_at(std::addressof(m_ok));
            }
        }

        ~ResultData() { destroyActive(); }

    public:
        [[nodiscard]] constexpr OkWrapper<OkType> asOk() && noexcept
            requires(!std::is_reference_v<OkType>) { return Ok(std::move(m_ok)); }

        [[nodiscard]] constexpr OkWrapper<OkType&> asOk() && noexcept
            requires(std::is_reference_v<OkType>) { return Ok(m_ok); }

        [[nodiscard]] constexpr OkWrapper<OkType> asOk() const & noexcept
            requires(!std::is_reference_v<OkType>) { return Ok(m_ok); }

        [[nodiscard]] constexpr OkWrapper<OkType&> asOk() const & noexcept
            requires(std::is_reference_v<OkType>) { return Ok(m_ok); }

        [[nodiscard]] constexpr ErrWrapper<void> asErr() const noexcept { return Err(); }

        [[nodiscard]] constexpr bool isOk() const noexcept { return m_isOk; }
        [[nodiscard]] constexpr bool isErr() const noexcept { return !m_isOk; }

        [[nodiscard]] constexpr explicit(false) operator bool() const noexcept { return isOk(); }

        constexpr OkType&& unwrap() && {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
            return std::move(m_ok);
        }

        constexpr OkType& unwrap() & {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
            return m_ok;
        }

        constexpr OkType const& unwrap() const & {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, m_err));
            }
            return m_ok;
        }

        [[nodiscard]] constexpr OkType&& unwrapUnchecked() && {
            RESULT_DEBUG_UNWRAP_CHECK(m_isOk, UnwrapException(ErrTag{}, m_err));
            return std::move(m_ok);
        }

        [[nodiscard]] constexpr OkType& unwrapUnchecked() & {
            RESULT_DEBUG_UNWRAP_CHECK(m_isOk, UnwrapException(ErrTag{}, m_err));
            return m_ok;
        }

        [[nodiscard]] constexpr OkType const& unwrapUnchecked() const & {
            RESULT_DEBUG_UNWRAP_CHECK(m_isOk, UnwrapException(ErrTag{}, m_err));
            return m_ok;
        }

        constexpr void unwrapErr() const {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
        }

        constexpr void unwrapErr() {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, m_ok));
            }
        }

        [[nodiscard]] OkType unwrapOr(OkType&& defaultValue) && {
            if (this->isOk()) {
                return std::move(this->m_ok);
            }
            return std::forward<OkType>(defaultValue);
        }

        [[nodiscard]] OkType unwrapOr(OkType const& defaultValue) const & {
            if (this->isOk()) {
                return this->m_ok;
            }
            return defaultValue;
        }

        [[nodiscard]] OkType unwrapOrDefault() && requires std::default_initializable<OkType> {
            if (this->isOk()) {
                return std::move(this->m_ok);
            }
            return OkType{};
        }

        [[nodiscard]] OkType unwrapOrDefault() const & requires std::default_initializable<OkType> {
            if (this->isOk()) {
                return this->m_ok;
            }
            return OkType{};
        }

        [[nodiscard]] OkType unwrapOrElse(auto&& func) && {
            if (this->isOk()) {
                return std::move(this->m_ok);
            }
            return func();
        }

        [[nodiscard]] OkType unwrapOrElse(auto&& func) const & {
            if (this->isOk()) {
                return this->m_ok;
            }
            return func();
        }

        void expect(std::string_view message) const {
            if (isErr()) {
                RESULT_THROW_IMPL(std::runtime_error(
                    std::string(message)
                ));
            }
        }
    };

    template <>
    class ResultData<void, void> {
    protected:
        bool m_isOk;

        constexpr explicit ResultData(bool isOk) noexcept : m_isOk(isOk) {}

        constexpr ResultData(ResultData&& other) noexcept : m_isOk(other.m_isOk) {}
        constexpr ResultData(ResultData const& other) noexcept = default;

    public:
        [[nodiscard]] constexpr OkWrapper<void> asOk() const noexcept { return Ok(); }
        [[nodiscard]] constexpr ErrWrapper<void> asErr() const noexcept { return Err(); }

        [[nodiscard]] constexpr bool isOk() const noexcept { return m_isOk; }
        [[nodiscard]] constexpr bool isErr() const noexcept { return !m_isOk; }

        [[nodiscard]] constexpr explicit(false) operator bool() const noexcept { return isOk(); }

        constexpr void unwrap() const {
            if (isErr()) {
                RESULT_THROW_IMPL(UnwrapException(ErrTag{}, std::monostate{}));
            }
        }

        constexpr void unwrapErr() const {
            if (isOk()) {
                RESULT_THROW_IMPL(UnwrapException(OkTag{}, std::monostate{}));
            }
        }
    };

    template <class OkType, class ErrType>
    class ResultDataWrapper : public ResultData<OkType, ErrType> {
    public:
        template <class OkType2> requires std::constructible_from<OkType, OkType2>
        constexpr explicit(false) ResultDataWrapper(
            OkWrapper<OkType2>&& ok
        ) noexcept(std::is_nothrow_constructible_v<OkType, OkType2>)
            : ResultData<OkType, ErrType>(OkTag{}, std::move(ok).unwrap()) {}

        template <class ErrType2> requires std::constructible_from<ErrType, ErrType2>
        constexpr explicit(false) ResultDataWrapper(
            ErrWrapper<ErrType2>&& err
        ) noexcept(std::is_nothrow_constructible_v<ErrType, ErrType2>)
            : ResultData<OkType, ErrType>(ErrTag{}, std::move(err).unwrap()) {}

        constexpr ResultDataWrapper(
            ResultDataWrapper&& other
        ) noexcept(std::is_nothrow_move_constructible_v<ResultData<OkType, ErrType>>)
            : ResultData<OkType, ErrType>(std::move(other)) {}

        constexpr ResultDataWrapper(
            ResultDataWrapper const& other
        ) noexcept(std::is_nothrow_copy_constructible_v<ResultData<OkType, ErrType>>)
            : ResultData<OkType, ErrType>(other) {}
    };

    template <class OkType>
    class ResultDataWrapper<OkType, void> : public ResultData<OkType, void> {
    public:
        template <class OkType2> requires std::constructible_from<OkType, OkType2>
        constexpr explicit(false) ResultDataWrapper(
            OkWrapper<OkType2>&& ok
        ) noexcept(std::is_nothrow_constructible_v<OkType, OkType2>)
            : ResultData<OkType, void>(OkTag{}, std::move(ok).unwrap()) {}

        constexpr explicit(false) ResultDataWrapper(ErrWrapper<void>&&) noexcept
            : ResultData<OkType, void>(ErrTag{}) {}

        constexpr ResultDataWrapper(
            ResultDataWrapper&& other
        ) noexcept(std::is_nothrow_move_constructible_v<ResultData<OkType, void>>)
            : ResultData<OkType, void>(std::move(other)) {}

        constexpr ResultDataWrapper(
            ResultDataWrapper const& other
        ) noexcept(std::is_nothrow_copy_constructible_v<ResultData<OkType, void>>)
            : ResultData<OkType, void>(other) {}
    };

    template <class ErrType>
    class ResultDataWrapper<void, ErrType> : public ResultData<void, ErrType> {
    public:
        constexpr explicit(false) ResultDataWrapper(OkWrapper<void>&&) noexcept
            : ResultData<void, ErrType>(OkTag{}) {}

        template <class ErrType2> requires std::constructible_from<ErrType, ErrType2>
        constexpr explicit(false) ResultDataWrapper(
            ErrWrapper<ErrType2>&& err
        ) noexcept(std::is_nothrow_constructible_v<ErrType, ErrType2>)
            : ResultData<void, ErrType>(ErrTag{}, std::move(err).unwrap()) {}

        constexpr ResultDataWrapper(
            ResultDataWrapper&& other
        ) noexcept(std::is_nothrow_move_constructible_v<ResultData<void, ErrType>>)
            : ResultData<void, ErrType>(std::move(other)) {}

        constexpr ResultDataWrapper(
            ResultDataWrapper const& other
        ) noexcept(std::is_nothrow_copy_constructible_v<ResultData<void, ErrType>>)
            : ResultData<void, ErrType>(other) {}
    };

    template <>
    class ResultDataWrapper<void, void> : public ResultData<void, void> {
    public:
        constexpr explicit(false) ResultDataWrapper(OkWrapper<void>&&) noexcept : ResultData(true) {}
        constexpr explicit(false) ResultDataWrapper(ErrWrapper<void>&&) noexcept : ResultData(false) {}
        constexpr ResultDataWrapper(ResultDataWrapper&& other) noexcept = default;
        constexpr ResultDataWrapper(ResultDataWrapper const& other) noexcept = default;
    };
}


template <class OkType = void, class ErrType = std::string>
class [[nodiscard]] Result final : public detail::ResultDataWrapper<OkType, ErrType> {
public:
    using detail::ResultDataWrapper<OkType, ErrType>::ResultDataWrapper;

    Result() = delete;
    Result(Result const&) = default;
    Result(Result&&) = default;
    Result& operator=(Result const&) = default;
    Result& operator=(Result&&) = default;
};