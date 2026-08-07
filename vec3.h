#pragma once

#include <iostream>

class Vec3 {
public:
    double x;
    double y;
    double z;


    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 cross(const Vec3& other) const {
        return Vec3 {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalized() const {
        return *this / length();
    }

    double length_squared() const {
        return x * x + y * y + z * z;
    }

    Vec3 operator-() const {
        return Vec3 {-x, -y, -z};
    }

    Vec3 operator+(const Vec3& other) const {
        return Vec3 {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator+(double num) const {
        return Vec3 {x + num, y + num, z + num};
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3 {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator-(double num) const {
        return Vec3 {x - num, y - num, z - num};
    }

    Vec3 operator*(const Vec3& other) const {
        return Vec3 {x * other.x, y * other.y, z * other.z};
    }

    Vec3 operator*(double num) const {
        return Vec3 {x * num, y * num, z * num};
    }

    Vec3 operator/(double num) const {
        return Vec3 {x / num, y / num, z / num};
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;

        return *this;
    }

    Vec3& operator+=(double num) {
        x += num;
        y += num;
        z += num;

        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;

        return *this;
    }

    Vec3& operator-=(double num) {
        x -= num;
        y -= num;
        z -= num;

        return *this;
    }

    Vec3& operator*=(double num) {
        x *= num;
        y *= num;
        z *= num;

        return *this;
    }

    Vec3& operator/=(double num) {
        x /= num;
        y /= num;
        z /= num;

        return *this;
    }
};

using Point = Vec3;
using Color = Vec3;


std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    os << vec.x << ", " << vec.y << ", " << vec.z;
    return os;
}

inline Vec3 operator*(double num, const Vec3& vec) {
    return Vec3 {vec.x * num, vec.y * num, vec.z * num};
}