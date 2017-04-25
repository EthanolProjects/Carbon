# Carbon API Reference Index

[TOC]

## Concurrency Library

## Dynamic Libraries Library

## Endian Utilities

## Mathematics Library

### Classes

#### Vector

```c++
template <std::size_t D, class T> 
union Vector {
    T data[D];
    struct { Vector<D - 1, T> less; T last; };
    Vector() = default;
    template <class ...U>
    constexpr Vector(const T& a1, U&& ... args) noexcept;
    constexpr Vector(const Vector<D - 1, T>&  _less, const T& _last) noexcept;
    template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int> = 0>
    constexpr Vector(const Vector<D, U>& rhs) noexcept;
    template <std::size_t D2, class ...U>
    constexpr Vector(const Vector<D2, T>& ptr, const T& arg, const T& arg2, U&& ...args) noexcept
    constexpr Vector operator+(const Vector& r) const noexcept;
    constexpr Vector operator-(const Vector& r) const noexcept;
    constexpr Vector operator-() const noexcept;
    template <class U>
    constexpr Vector operator*(U&& r) const noexcept
    template <class U>
    constexpr Vector operator/(U&& r) const noexcept;
    constexpr T& operator[](std::size_t index) noexcept;
    constexpr T operator[](std::size_t index) const noexcept;
    constexpr Vector& operator+= (const Vector& r) noexcept
    constexpr Vector& operator-= (const Vector& r) noexcept
    template <class U>
    constexpr Vector& operator*= (U&& r) noexcept
    template <class U>
    constexpr Vector& operator/= (U&& r) noexcept
    constexpr T lengthSqr() const noexcept;
    T length();
    void normalize() noexcept;
    constexpr T dot(const Vector& r) const noexcept;
    constexpr bool operator==(const Vector& r) const noexcept;
    constexpr bool operator!=(const Vector& r) const noexcept;
    constexpr bool operator<(const Vector& r) const noexcept;
    constexpr bool operator>(const Vector& r) const noexcept;
    constexpr bool operator<=(const Vector& r) const noexcept;
    constexpr bool operator>=(const Vector& r) const noexcept;
    T euclideanDistance(const Vector& rhs) const noexcept;
    constexpr T chebyshevDistance(const Vector& rhs) const noexcept;
    constexpr T manhattanDistance(const Vector& rhs) const noexcept;
};
```

##### Complexity

All operations of class Vector has liner complexity about the Dimension, which is O(D)

##### Specialization

Vector<T, 2> adds the following union fields

```c++
struct { T x, y; };
struct { T l, a; };
struct { T r, g; };
```

Vector<T, 3>adds the following union fields and cross product functions and operators

```c++
struct { T x, y, z; };
struct { T r, g, b; };
Vector<2, T> xy;
Vector<2, T> rg;
constexpr Vector operator*(const Vector& r) noexcept;
constexpr Vector& operator*=(const Vector& r) noexcept;
```

##### Demo

```c++
#include <iostream>
#include <Carbon/Math.hpp>
```

## Maintenance Library

## Network Library
