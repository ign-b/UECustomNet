#pragma once

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
class CQuaternion {
protected:
    T m_fW, m_fX, m_fY, m_fZ;

public:
    CQuaternion()
        : m_fW(1.0f), m_fX(0.0f), m_fY(0.0f), m_fZ(0.0f) {
    }

    CQuaternion(T const &fInW, T const &fInX, T const &fInY, T const &fInZ)
        : m_fW(fInW), m_fX(fInX), m_fY(fInY), m_fZ(fInZ) {
    }

    void Set(T const &fInW, T const &fInX, T const &fInY, T const &fInZ) {
        m_fW = fInW;
        m_fX = fInX;
        m_fY = fInY;
        m_fZ = fInZ;
    }

    T &GetW() { return m_fW; }
    T const &GetW() const { return m_fW; }
    T &GetX() { return m_fX; }
    T const &GetX() const { return m_fX; }
    T &GetY() { return m_fY; }
    T const &GetY() const { return m_fY; }
    T &GetZ() { return m_fZ; }
    T const &GetZ() const { return m_fZ; }

    void Clear() {
        m_fW = static_cast<T>(1.0f);
        m_fX = static_cast<T>(0.0f);
        m_fY = static_cast<T>(0.0f);
        m_fZ = static_cast<T>(0.0f);
    }

    T Length() const {
        return std::sqrt(m_fW * m_fW + m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
    }

    bool IsNearlyEqual(CQuaternion const &quatOther, T ErrorTolerance = static_cast<T>(1e-6)) const {
        return std::abs(m_fW - quatOther.m_fW) <= ErrorTolerance &&
               std::abs(m_fX - quatOther.m_fX) <= ErrorTolerance &&
               std::abs(m_fY - quatOther.m_fY) <= ErrorTolerance &&
               std::abs(m_fZ - quatOther.m_fZ) <= ErrorTolerance;
    }

    bool IsNearlyZero(T ErrorTolerance = static_cast<T>(1e-6)) const {
        return IsNearlyEqual(
            CQuaternion(
                static_cast<T>(1),
                static_cast<T>(0),
                static_cast<T>(0),
                static_cast<T>(0)
            ),
            ErrorTolerance
        );
    }

    void Normalize() {
        T length = Length();
        if (length > static_cast<T>(0)) {
            m_fW /= length;
            m_fX /= length;
            m_fY /= length;
            m_fZ /= length;
        }
    }

    CQuaternion operator*(const CQuaternion &quatOther) const {
        return CQuaternion(
            m_fW * quatOther.m_fW - m_fX * quatOther.m_fX - m_fY * quatOther.m_fY - m_fZ * quatOther.m_fZ,
            m_fW * quatOther.m_fX + m_fX * quatOther.m_fW + m_fY * quatOther.m_fZ - m_fZ * quatOther.m_fY,
            m_fW * quatOther.m_fY - m_fX * quatOther.m_fZ + m_fY * quatOther.m_fW + m_fZ * quatOther.m_fX,
            m_fW * quatOther.m_fZ + m_fX * quatOther.m_fY - m_fY * quatOther.m_fX + m_fZ * quatOther.m_fW
        );
    }

    CQuaternion &operator=(CQuaternion const &quatOther) {
        m_fW = quatOther.m_fW;
        m_fX = quatOther.m_fX;
        m_fY = quatOther.m_fY;
        m_fZ = quatOther.m_fZ;
        return *this;
    }

    bool operator==(const CQuaternion &quatOther) const {
        return m_fW == quatOther.m_fW && m_fX == quatOther.m_fX &&
               m_fY == quatOther.m_fY && m_fZ == quatOther.m_fZ;
    }

    bool operator!=(const CQuaternion &quatOther) const {
        return !(*this == quatOther);
    }
};

using CQuaternionf = CQuaternion<float>;
