#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <type_traits>

namespace Carbon {
    template <std::size_t D, class T> union Vector;

    template <class T>
    union Vector<2, T> {
        T data[2];
        struct { T x, y; };
        struct { T l, a; };
        struct { T r, g; };
        struct { T less, last; };

        Vector() = default;
        constexpr Vector(T _x, T _y) noexcept: x(_x), y(_y) {}
        template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int> = 0>
        constexpr Vector(const Vector<2, U>& rhs) noexcept : x(T(rhs.x)), y(T(rhs.y)) {}

        constexpr Vector operator+(const Vector& r) const noexcept { return Vector(x + r.x, y + r.y); }
        constexpr Vector operator-(const Vector& r) const noexcept { return Vector(x - r.x, y - r.y); }
        constexpr Vector operator-() const noexcept { return Vector(-x, -y); }
        template <class U>
        constexpr Vector operator*(U&& r) const noexcept { return Vector(x * std::forward<U>(r), y * std::forward<U>(r)); }
        template <class U>
        constexpr Vector operator/(U&& r) const noexcept { return Vector(x / std::forward<U>(r), y / std::forward<U>(r)); }
        constexpr T& operator[](std::size_t index) noexcept { return data[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return data[index]; }
        constexpr Vector& operator+=(const Vector& r) noexcept {
            x += r.x, y += r.y;
            return *this;
        }
        constexpr Vector& operator-=(const Vector& r) noexcept {
            x -= r.x, y -= r.y;
            return *this;
        }
        template <class U>
        constexpr Vector& operator*=(U&& r) noexcept {
            x *= std::forward<U>(r), y *= std::forward<U>(r);
            return *this;
        }
        template <class U>
        constexpr Vector& operator/=(U&& r) noexcept {
            x /= std::forward<U>(r), y /= std::forward<U>(r);
            return *this;
        }

        constexpr T lengthSqr() const noexcept { return x * x + y * y; }
        T length() noexcept { return std::sqrt(lengthSqr()); }
        void normalize() noexcept { (*this) /= length(); }
        constexpr T dot(const Vector& r) const noexcept { return x * r.x + y * r.y; }

        constexpr bool operator==(const Vector& r) const noexcept { return x == r.x && y == r.y; }
        constexpr bool operator!=(const Vector& r) const noexcept { return x != r.x || y != r.y; }
        constexpr T euclideanDistance(const Vector& rhs) const noexcept { return (*this - rhs).length(); }
        constexpr T chebyshevDistance(const Vector& rhs) const noexcept { return std::max(std::abs(x - rhs.x), std::abs(y - rhs.y)); }
        constexpr T manhattanDistance(const Vector& rhs) const noexcept { return std::abs(x - rhs.x) + std::abs(y - rhs.y); }
    };

    template <class T>
    union Vector<3, T> {
        T data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { Vector<2, T> less; T last; };
        Vector<2, T> xy;
        Vector<2, T> rg;

        Vector() = default;
        constexpr Vector(T _x, T _y, T _z) noexcept : x(_x), y(_y), z(_z) {}
        constexpr Vector(const Vector<2, T>& ls, T arg) noexcept : less(ls), last(arg) {}
        template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int> = 0>
        constexpr Vector(const Vector<3, U>& rhs) noexcept : x(T(rhs.x)), y(T(rhs.y)), z(T(rhs.z)) {}

        constexpr Vector operator+(const Vector& r) const noexcept { return Vector(x + r.x, y + r.y, z + r.z); }
        constexpr Vector operator-(const Vector& r) const noexcept { return Vector(x - r.x, y - r.y, z - r.z); }
        constexpr Vector operator-() const noexcept { return Vector(-x, -y, -z); }
        template <class U>
        constexpr Vector operator*(U&& r) const noexcept { return Vector(x * std::forward<U>(r), y * std::forward<U>(r), z * std::forward<U>(r)); }
        constexpr Vector operator*(const Vector& r) const noexcept { return Vector(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x); }
        template <class U>
        constexpr Vector operator/(U&& r) const noexcept { return Vector(x / std::forward<U>(r), y / std::forward<U>(r), z / std::forward<U>(r)); }
        constexpr T& operator[](std::size_t index) noexcept { return data[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return data[index]; }

        constexpr Vector& operator+=(const Vector& r) noexcept {
            x += r.x, y += r.y, z += r.z;
            return *this;
        }
        constexpr Vector& operator-=(const Vector& r) noexcept {
            x -= r.x, y -= r.y, z -= r.z;
            return *this;
        }
        template <class U>
        constexpr Vector& operator*=(U&& r) noexcept {
            x *= std::forward<U>(r), y *= std::forward<U>(r), z *= std::forward<U>(r);
            return *this;
        }
        constexpr Vector& operator*=(const Vector& r) noexcept {
            *this = Vector(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
            return *this;
        }
        template <class U>
        constexpr Vector& operator/=(U&& r) noexcept {
            x /= std::forward<U>(r), y /= std::forward<U>(r), z /= std::forward<U>(r);
            return *this;
        }

        constexpr T lengthSqr() const noexcept { return x * x + y * y + z * z; }
        T length() noexcept { return std::sqrt(lengthSqr()); }
        void normalize() noexcept { (*this) /= length(); }
        constexpr T dot(const Vector& r) const noexcept { return x * r.x + y * r.y + z * r.z; }

        constexpr bool operator==(const Vector& r) const noexcept { return x == r.x && y == r.y && z == r.z; }
        constexpr bool operator!=(const Vector& r) const noexcept { return x != r.x || y != r.y || z != r.z; }

        constexpr T euclideanDistance(const Vector& rhs) const noexcept { return (*this - rhs).length(); }
        constexpr T chebyshevDistance(const Vector& rhs) const noexcept { return std::max(std::max(std::abs(x - rhs.x), std::abs(y - rhs.y)), std::abs(z - rhs.z)); }
        constexpr T manhattanDistance(const Vector& rhs) const noexcept { return std::abs(x - rhs.x) + std::abs(y - rhs.y) + std::abs(z - rhs.z); }
    };

    template <std::size_t D, class T>
    union Vector {
        T data[D] = { T(0) };
        struct { Vector<D - 1, T> less; T last; };

        Vector() = default;
        template <class ...U>
        constexpr Vector(const T& a1, U&& ... args) noexcept: data{ a1, std::forward<U>(args)... } {}
        constexpr Vector(const Vector<D - 1, T>&  _less, const T& _last) noexcept : less(_less), last(_last) {}
        template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int> = 0>
        constexpr Vector(const Vector<D, U>& rhs) noexcept : less(Vector(rhs.less)), last(T(rhs.last)) {}
        template <std::size_t D2, class ...U>
        constexpr Vector(const Vector<D2, T>& ptr, const T& arg, const T& arg2, U&& ...args) noexcept :
            Vector(Vector<D2 + 1, T>(ptr, arg), arg2, std::forward<U>(args)...) {}

        constexpr Vector operator+(const Vector& r) const noexcept { return Vector(less + r.less, last + r.last); }
        constexpr Vector operator-(const Vector& r) const noexcept { return Vector(less - r.less, last - r.last); }
        constexpr Vector operator-() const noexcept { return Vector(-less, -last); }
        template <class U>
        constexpr Vector operator*(U&& r) const noexcept { return Vector(less * std::forward<U>(r), last * std::forward<U>(r)); }
        template <class U>
        constexpr Vector operator/(U&& r) const noexcept { return Vector(less / std::forward<U>(r), last / std::forward<U>(r)); }
        constexpr T& operator[](std::size_t index) noexcept { return data[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return data[index]; }

        constexpr Vector& operator+= (const Vector& r) noexcept {
            less += r.less, last += r.last;
            return *this;
        }
        constexpr Vector& operator-= (const Vector& r) noexcept {
            less -= r.less, last -= r.last;
            return *this;
        }
        template <class U>
        constexpr Vector& operator*= (U&& r) noexcept {
            less *= std::forward<U>(r), last *= std::forward<U>(r);
            return *this;
        }
        template <class U>
        constexpr Vector& operator/= (U&& r) noexcept {
            less /= std::forward<U>(r), last /= std::forward<U>(r);
            return *this;
        }

        constexpr T lengthSqr() const noexcept { return less.lengthSqr() + last * last; }
        T length() { return std::sqrt(lengthSqr()); }
        void normalize() noexcept { (*this) /= length(); }
        constexpr T dot(const Vector& r) const noexcept { return less.dot(r.less) + last * r.last; }

        constexpr bool operator==(const Vector& r) const noexcept { return less == r.less && last == r.last; }
        constexpr bool operator!=(const Vector& r) const noexcept { return less != r.less || last != r.last; }

        constexpr T euclideanDistance(const Vector& rhs) const noexcept { return (*this - rhs).length(); }
        constexpr T chebyshevDistance(const Vector& rhs) const noexcept { return std::max(less.chebyshevDistance(rhs.less), std::abs(last - rhs.last)); }
        constexpr T manhattanDistance(const Vector& rhs) const noexcept { return less.manhattanDistance(rhs.less) + std::abs(last - rhs.last); }
    };

    // Relational Operations
    template <std::size_t D, class T>
    Vector<D, T> dot(const Vector<D, T>& l, const Vector<D, T>& r) { return l.dot(r); }

    template <std::size_t D, class T>
    Vector<D, T> operator * (size_t scale, const Vector<D, T>& r) { return r * scale; }

    // Compare Operations
#define _CB_CEXPR_VECRELOP(x) template <std::size_t D, class T> constexpr bool operator x \
(const Vector<D, T>& l, const Vector<D, T>& r) noexcept { return l.lengthSqr() x r.lengthSqr(); }
    _CB_CEXPR_VECRELOP(<=) _CB_CEXPR_VECRELOP(>=) _CB_CEXPR_VECRELOP(<) _CB_CEXPR_VECRELOP(>)
#undef _CB_CEXPR_VEC_REL_OP

#define _CB_CEXPR_VEC_S(s) template <class T> using Vec##s = Vector<s, T>;
#define _CB_CEXPR_VEC_E(s)  _CB_CEXPR_VEC_S(s) \
    using Vec##s##i = Vec##s<int>; using Vec##s##u = Vec##s<unsigned int>; \
    using Vec##s##f = Vec##s<float>; using Vec##s##d = Vec##s<double>;
    _CB_CEXPR_VEC_E(2) _CB_CEXPR_VEC_E(3) _CB_CEXPR_VEC_E(4)
#undef _CB_CEXPR_VEC_S

    namespace Math {
        template <class ContT>
        ContT vecAdd(const ContT& l, const ContT& r) { 
            ContT res; for (size_t i = 0; i < l.size(); ++i) res[i] = l[i] + r[i]; return res;
        }

        template <class ContT>
        ContT£¦ vecAddEq(ContT& l, const ContT& r) {
            for (size_t i = 0; i < l.size(); ++i) l[i] += r[i]; return l;
        }

        template <class ContT>
        ContT vecMinus(const ContT& l, const ContT& r) {
            ContT res; for (size_t i = 0; i < l.size(); ++i) res[i] = l[i] - r[i]; return res;
        }

        template <class ContT>
        ContT£¦ vecMinusEq(ContT& l, const ContT& r) {
            for (size_t i = 0; i < l.size(); ++i) l[i] -= r[i]; return l;
        }

        template <class ContT, class T>
        ContT vecScaler(const ContT& l, const T& r) {
            ContT res; for (size_t i = 0; i < l.size(); ++i) res[i] = l[i] * r; return res;
        }

        template <class ContT, class T>
        ContT£¦ vecScalerEq(ContT& l, const T& r) {
            for (size_t i = 0; i < l.size(); ++i) l[i] *= r[i]; return l;
        }

        template <class ContT>
        auto vecDot(const ContT& l, const ContT& r) {
            ContT::value_type res = 0;
            for (size_t i = 0; i < l.size(); ++i) res += l[i] * r[i]; return res;
        }
    }

    template<class T, std::size_t rows, std::size_t cols>
    class Matrix {
    public:
        Matrix() = default;
        template<class... Ts>
        constexpr Matrix(const T& arg1, Ts&&...args) noexcept : mData{ arg1, std::forward<Ts>(args)... } {}
        constexpr T* operator[](std::size_t row) noexcept { return mData + row * cols; }
        constexpr const T* operator[](std::size_t row) const noexcept { return mData + row * cols; }
    private:
        T mData[cols * rows] = { T(0) };
    };

    template<class T, std::size_t d>
    class Matrix<T, d, d> {
    public:
        Matrix() = default;
        template<class... Ts>
        constexpr Matrix(const T& arg1, Ts&&...args) noexcept : mData{ arg1, std::forward<Ts>(args)... } {}
        constexpr T* operator[](std::size_t row) noexcept { return mData + row * d; }
        constexpr const T* operator[](std::size_t row) const noexcept { return mData + row * d; }
        Matrix& operator*= (const Matrix& rhs) noexcept {
            (*this) = (*this) * rhs;
            return *this;
        }
        constexpr static auto identity() noexcept {
            Matrix ans{};
            for (std::size_t i = 0; i < d; ++i)
                ans[i][i] = 1.0;
            return ans;
        }
        void loadIdentity() noexcept {
            (*this) = identity();
        }
    private:
        T mData[d * d] = { T(0) };
    };

    template <class T, std::size_t cols, std::size_t rows>
    constexpr auto operator + (const Matrix<T, rows, cols>& lhs, const Matrix<T, rows, cols>& rhs) noexcept {
        Matrix<T, rows, cols> ret{};
        for (std::size_t x = 0; x < cols; ++x)
            for (std::size_t y = 0; y < rows; ++y)
                ret[x][y] = lhs[x][y] + rhs[x][y];
        return ret;
    }

    template <class T, std::size_t cols, std::size_t rows>
    constexpr auto operator - (const Matrix<T, rows, cols>& lhs, const Matrix<T, rows, cols>& rhs) noexcept {
        Matrix<T, rows, cols> ret{};
        for (std::size_t x = 0; x < cols; ++x)
            for (std::size_t y = 0; y < rows; ++y)
                ret[x][y] = lhs[x][y] - rhs[x][y];
        return ret;
    }

    template <class T, std::size_t cols, std::size_t rows>
    auto& operator += (Matrix<T, rows, cols>& lhs, const Matrix<T, rows, cols>& rhs) noexcept {
        for (std::size_t x = 0; x < cols; ++x)
            for (std::size_t y = 0; y < rows; ++y)
                lhs[x][y] += rhs[x][y];
        return lhs;
    }

    template <class T, std::size_t cols, std::size_t rows>
    auto& operator -= (Matrix<T, rows, cols>& lhs, const Matrix<T, rows, cols>& rhs) noexcept {
        for (std::size_t x = 0; x < cols; ++x)
            for (std::size_t y = 0; y < rows; ++y)
                lhs[x][y] -= rhs[x][y];
        return lhs;
    }

    template <class T, std::size_t cols, std::size_t rows>
    constexpr auto operator - (const Matrix<T, rows, cols>& lhs) noexcept {
        Matrix<T, rows, cols> ret{};
        for (std::size_t x = 0; x < cols; ++x)
            for (std::size_t y = 0; y < rows; ++y)
                ret[x][y] = -lhs[x][y];
        return ret;
    }

    template<class T, std::size_t cols, std::size_t rows, std::size_t cols2>
    constexpr Matrix<T, rows, cols2> operator * (const Matrix<T, rows, cols>& lhs, const Matrix<T, cols, cols2>& rhs) noexcept {
        Matrix<T, rows, cols2> ans{};
        for (std::size_t i = 0; i < rows; ++i)
            for (std::size_t j = 0; j < cols2; ++j)
                for (std::size_t k = 0; k < cols; ++k)
                    ans[i][j] += lhs[i][k] * rhs[k][j];
        return ans;
    }

    template <class T, std::size_t cols, std::size_t rows>
    constexpr Vector<cols, T> operator * (const Matrix<T, rows, cols>& lhs, const Vector<cols, T>& rhs) noexcept {
        Vector<cols, T> ans{};
        for (std::size_t i = 0; i < rows; ++i)
            for (std::size_t k = 0; k < cols; ++k)
                ans[i] += lhs[i][k] * rhs[k];
        return ans;
    }

    template <class T>
    using Mat33 = Matrix<T, 3, 3>;
    using Mat33d = Mat33<double>;

}
