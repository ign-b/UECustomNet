#include "String.h"

#include <cstring>

void CString::Resize(un::Size nNewSize) {
    if (nNewSize == m_nLength) {
        return;
    }

    auto *const nNewData = new char[nNewSize + 1];
    std::memset(nNewData, 0, nNewSize + 1);

    if (m_aData) {
        un::Size const nCopyLength = nNewSize < m_nLength ? nNewSize : m_nLength;
        std::strncpy(nNewData, m_aData, nCopyLength);

        Intl_DeallocateData();
    }

    m_aData = nNewData;
    m_nLength = nNewSize;
}

void CString::Intl_CopyData(const char *aText) {
    if (!aText) {
        m_aData = nullptr;
        m_nLength = 0;
        return;
    }

    un::Size const nSize = StringLen(aText);

    m_aData = new char[nSize + 1];
    std::memset(m_aData, 0, nSize + 1);

    std::strncpy(m_aData, aText, nSize);

    m_nLength = nSize;
}

void CString::Intl_DeallocateData() {
    if (m_aData) {
        delete[] m_aData;
    }

    m_aData = nullptr;
    m_nLength = 0;
}
