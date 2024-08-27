#pragma once

#include "Common.h"

class UNET_API CString {
protected:
    char *m_aData;
    un::Size m_nLength;

public:
    CString() : m_aData(nullptr),
                m_nLength(0) {
    }

    CString(char const *aText) : m_aData(nullptr),
                                 m_nLength(0) {
        if (aText) {
            Intl_CopyData(aText);
        }
    }

    CString(CString const &sOther) : m_aData(nullptr),
                                     m_nLength(0) {
        if (sOther.m_aData) {
            Intl_CopyData(sOther.m_aData);
        }
    }

    CString(CString &&sOther) noexcept : m_aData(sOther.m_aData),
                                         m_nLength(sOther.m_nLength) {
        sOther.m_aData = nullptr;
        sOther.m_nLength = 0;
    }

    ~CString() {
        Intl_DeallocateData();
    }

    char &operator[](un::Size nIndex);

    const char &operator[](un::Size nIndex) const;

    char *operator*();

    char const *operator*() const;

    [[nodiscard]] char *Data();

    [[nodiscard]] char const *Data() const;

    [[nodiscard]] un::Size Length() const;

    void Assign(const char *aText);

    void Assign(const CString &sOther);

    void Clear();

    void Resize(un::Size nNewSize);

    [[nodiscard]] bool IsEmpty() const;

protected:
    void Intl_CopyData(const char *aText);

    void Intl_DeallocateData();
};

inline un::Size StringLen(const char *aText) {
    un::Size nCount = 0;
    while (*aText++) {
        ++nCount;
    }
    return nCount;
}

inline char &CString::operator[](un::Size const nIndex) {
    return m_aData[nIndex];
}

inline const char &CString::operator[](un::Size const nIndex) const {
    return m_aData[nIndex];
}

inline char *CString::operator*() {
    return m_aData;
}

inline char const *CString::operator*() const {
    return m_aData;
}

inline const char *CString::Data() const {
    return m_aData;
}

inline un::Size CString::Length() const {
    return m_nLength;
}

inline void CString::Assign(const char *aText) {
    Intl_DeallocateData();
    if (aText) {
        Intl_CopyData(aText);
    }
}

inline void CString::Assign(CString const &sOther) {
    if (this == &sOther) {
        return;
    }

    Intl_DeallocateData();
    if (sOther.m_aData) {
        Intl_CopyData(sOther.m_aData);
    }
}

inline void CString::Clear() {
    Intl_DeallocateData();
}

inline char *CString::Data() {
    return m_aData;
}

inline bool CString::IsEmpty() const {
    return m_nLength == 0;
}
