#pragma once

#include <iterator>

template<typename T>
struct SNode {
    T m_Data;
    SNode *m_pNext;
    SNode *m_pPrev;

    explicit SNode(T const &InData) : m_Data(InData),
                                      m_pNext(nullptr),
                                      m_pPrev(nullptr) {
    }
};

template<typename T>
class UNET_API CList {
public:
    class Iterator;

protected:
    SNode<T> *m_pHead;
    SNode<T> *m_pTail;

public:
    CList();

    ~CList();

    void PushBack(T const &Value);

    void Remove(T const &Value);

    Iterator Begin();

    Iterator End();

    void Clear();
};

template<typename T>
class UNET_API CList<T>::Iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

protected:
    SNode<T> *m_pCurrent;

public:
    explicit Iterator(SNode<T> *pInNode) : m_pCurrent(pInNode) {
    }

    reference operator*() const { return m_pCurrent->m_Data; }
    pointer operator->() const { return &(m_pCurrent->m_Data); }

    Iterator &operator++() {
        m_pCurrent = m_pCurrent->m_pNext;
        return *this;
    }

    Iterator operator++(int) {
        Iterator itTmp = *this;
        ++*this;
        return itTmp;
    }

    Iterator &operator--() {
        m_pCurrent = m_pCurrent->m_pPrev;
        return *this;
    }

    Iterator operator--(int) {
        Iterator itTmp = *this;
        --*this;
        return itTmp;
    }

    bool operator==(Iterator const &itOther) const { return m_pCurrent == itOther.m_pCurrent; }
    bool operator!=(Iterator const &itOther) const { return m_pCurrent != itOther.m_pCurrent; }

    Iterator &operator=(Iterator const &itOther) {
        if (this != &itOther) {
            m_pCurrent = itOther.m_pCurrent;
        }
        return *this;
    }
};

template<typename T>
CList<T>::CList() : m_pHead(nullptr), m_pTail(nullptr) {
}

template<typename T>
CList<T>::~CList() {
    Clear();
}

template<typename T>
void CList<T>::PushBack(T const &Value) {
    auto *pNewNode = new SNode<T>(Value);
    if (!m_pTail) {
        m_pHead = m_pTail = pNewNode;
    } else {
        m_pTail->m_pNext = pNewNode;
        pNewNode->m_pPrev = m_pTail;
        m_pTail = pNewNode;
    }
}

template<typename T>
void CList<T>::Remove(T const &Value) {
    SNode<T> *pHead = m_pHead;
    while (pHead) {
        if (pHead->m_Data == Value) {
            if (pHead->m_pPrev) {
                pHead->m_pPrev->m_pNext = pHead->m_pNext;
            } else {
                m_pHead = pHead->m_pNext;
            }

            if (pHead->m_pNext) {
                pHead->m_pNext->m_pPrev = pHead->m_pPrev;
            } else {
                m_pTail = pHead->m_pPrev;
            }

            delete pHead;
            return;
        }

        pHead = pHead->m_pNext;
    }
}

template<typename T>
typename CList<T>::Iterator CList<T>::Begin() {
    return Iterator{m_pHead};
}

template<typename T>
typename CList<T>::Iterator CList<T>::End() {
    return Iterator{nullptr};
}

template<typename T>
void CList<T>::Clear() {
    SNode<T> *pHead = m_pHead;
    while (pHead) {
        SNode<T> *pNext = pHead->m_pNext;
        delete pHead;
        pHead = pNext;
    }

    m_pHead = m_pTail = nullptr;
}
