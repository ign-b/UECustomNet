#pragma once

#include <cmath>
#include <type_traits>

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
class CVector3 {
protected:
    T m_fX, m_fY, m_fZ;

public:
    CVector3()
        : m_fX(0), m_fY(0), m_fZ(0) {
    }

    explicit CVector3(T const &fX, T const &fY, T const &fZ)
        : m_fX(fX), m_fY(fY), m_fZ(fZ) {
    }

    void Set(T const &fX, T const &fY, T const &fZ) {
        m_fX = fX; m_fY = fY; m_fZ = fZ;
    }

    T &GetX() { return m_fX; }
    T const &GetX() const { return m_fX; }
    T &GetY() { return m_fY; }
    T const &GetY() const { return m_fY; }
    T &GetZ() { return m_fZ; }
    T const &GetZ() const { return m_fZ; }

    void Clear() {
        m_fX = m_fY = m_fZ = static_cast<T>(0);
    }

    CVector3 operator+(CVector3 const &vecOther) const {
        return CVector3(m_fX + vecOther.m_fX, m_fY + vecOther.m_fY, m_fZ + vecOther.m_fZ);
    }

    CVector3 operator-(CVector3 const &vecOther) const {
        return CVector3(m_fX - vecOther.m_fX, m_fY - vecOther.m_fY, m_fZ - vecOther.m_fZ);
    }

    CVector3 operator*(T nScalar) const {
        return CVector3(m_fX * nScalar, m_fY * nScalar, m_fZ * nScalar);
    }

    CVector3 operator/(T nScalar) const {
        return CVector3(m_fX / nScalar, m_fY / nScalar, m_fZ / nScalar);
    }

    CVector3 &operator+=(CVector3 const &vecOther) {
        m_fX += vecOther.m_fX;
        m_fY += vecOther.m_fY;
        m_fZ += vecOther.m_fZ;
        return *this;
    }

    CVector3 &operator-=(CVector3 const &vecOther) {
        m_fX -= vecOther.m_fX;
        m_fY -= vecOther.m_fY;
        m_fZ -= vecOther.m_fZ;
        return *this;
    }

    CVector3 &operator=(CVector3 const &vecOther) {
        m_fX = vecOther.m_fX;
        m_fY = vecOther.m_fY;
        m_fZ = vecOther.m_fZ;
        return *this;
    }

    T Length() const {
        return std::sqrtf(m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
    }

    bool IsNearlyEqual(CVector3 const &vecOther, T ErrorTolerance = static_cast<T>(1e-6)) const {
        return std::abs(m_fX - vecOther.m_fX) <= ErrorTolerance &&
               std::abs(m_fY - vecOther.m_fY) <= ErrorTolerance &&
               std::abs(m_fZ - vecOther.m_fZ) <= ErrorTolerance;
    }

    bool IsNearlyZero(T ErrorTolerance = static_cast<T>(1e-6)) const {
        return IsNearlyEqual(
            CVector3(
                static_cast<T>(0),
                static_cast<T>(0),
                static_cast<T>(0)
            ),
            ErrorTolerance
        );
    }

    void Normalize() {
        T nLength = Length();
        if (nLength > static_cast<T>(0)) {
            m_fX /= nLength;
            m_fY /= nLength;
            m_fZ /= nLength;
        }
    }

    T Dot(CVector3 const &vecOther) const {
        return m_fX * vecOther.m_fX + m_fY * vecOther.m_fY + m_fZ * vecOther.m_fZ;
    }

    CVector3 Cross(CVector3 const &vecOther) const {
        return {
            m_fY * vecOther.m_fZ - m_fZ * vecOther.m_fY,
            m_fZ * vecOther.m_fX - m_fX * vecOther.m_fZ,
            m_fX * vecOther.m_fY - m_fY * vecOther.m_fX
        };
    }

    bool operator==(CVector3 const &vecOther) const {
        return m_fX == vecOther.m_fX && m_fY == vecOther.m_fY && m_fZ == vecOther.m_fZ;
    }

    bool operator!=(CVector3 const &vecOther) const {
        return !(*this == vecOther);
    }
};

using CVector3f = CVector3<float>;
