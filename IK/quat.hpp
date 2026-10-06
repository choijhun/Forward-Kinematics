#ifndef QUAT_HPP
#define QUAT_HPP

#include <jm/jm.hpp>
struct quat {
    quat() :w(1), x(0), y(0), z(0) {}
    quat(float r, const jm::vec3& i) : _r(r), v(i) {}
    quat(float r, float vx, float vy, float vz) : w(r), x(vx), y(vy), z(vz) {}

    quat operator *(const quat& q) const {
        return quat(w * q.w - jm::dot(v, q.v), w * q.v + q.w * v + jm::cross(v, q.v));
    }

    friend quat inverse(const quat& q) {
        return quat(q.w, -q.v);
    }

    quat operator - () const {
        return quat(-w, -v);
    }
    float operator %(const quat& q) const {
        return w * q.w + x * q.x + y * q.y + z * q.z;
    }

    operator jm::mat4() const {
        return  toMat4();
    }
    jm::mat4 toMat4() const {
        return jm::mat4(
            w * w + x * x - y * y - z * z,
            2 * x * y + 2 * w * z,
            2 * x * z - 2 * w * y,
            0,
            2 * x * y - 2 * w * z,
            w * w - x * x + y * y - z * z,
            2 * y * z + 2 * w * x,
            0,
            2 * x * z + 2 * w * y,
            2 * y * z - 2 * w * x,
            w * w - x * x - y * y + z * z,
            0,
            0, 0, 0, 1);
    }


    union {
        struct {
            float w, x, y, z;
        };
        struct {
            float _r; jm::vec3 v;
        };
        struct {
            float p[4];
        };
    };
};

inline quat inverse() {
    return quat(0, 0, 0, 0);
}

inline quat qexp(const jm::vec3& v) {
    float angle = jm::length(v);
    if (angle < 1E-10) return quat(1, 0, 0, 0);
    jm::vec3 v_ = jm::normalize(v);
    return quat(cosf(angle / 2), v_ * sinf(angle / 2));
}
inline jm::vec3 qlog(const quat& q) {
    float l = jm::length(q.v);
    if (l < 1E-10) return jm::vec3(0, 0, 0);
    jm::vec3 v = jm::normalize(q.v);
    return atan2f(l, q.w) * 2 * v;
}
inline quat slerp(const quat& a, const quat& b, float t) {
    if (a % b > 0)
        return a * qexp(qlog(inverse(a) * b) * t);
    else
        return a * qexp(qlog(inverse(a) * -b) * t);
}

#endif