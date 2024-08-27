#pragma once

#include <unordered_map>
#include <functional>

template<typename IndexType, typename DataType>
class CDataStore {
public:
    using TDataProcessor = std::function<void(DataType &)>;

protected:
    std::unordered_map<IndexType, DataType *> m_DataMap;

public:
    bool Add(IndexType nIndex, DataType *pData);

    bool Remove(IndexType nIndex);

    DataType *Get(IndexType nIndex);

    bool Exists(IndexType nIndex) const;

    void Clear();

    [[nodiscard]] size_t Size() const;

    void ForEach(TDataProcessor const &fnExecute);
};

template<typename IndexType, typename DataType>
bool CDataStore<IndexType, DataType>::Add(IndexType const nIndex, DataType *pData) {
    auto result = m_DataMap.try_emplace(nIndex, pData);
    return result.second;
}

template<typename IndexType, typename DataType>
bool CDataStore<IndexType, DataType>::Remove(IndexType const nIndex) {
    return m_DataMap.erase(nIndex) > 0;
}

template<typename IndexType, typename DataType>
DataType *CDataStore<IndexType, DataType>::Get(IndexType const nIndex) {
    auto it = m_DataMap.find(nIndex);
    if (it != m_DataMap.end()) {
        return it->second;
    }
    return nullptr;
}

template<typename IndexType, typename DataType>
bool CDataStore<IndexType, DataType>::Exists(IndexType const nIndex) const {
    return m_DataMap.contains(nIndex);
}

template<typename IndexType, typename DataType>
void CDataStore<IndexType, DataType>::Clear() {
    m_DataMap.clear();
}

template<typename IndexType, typename DataType>
size_t CDataStore<IndexType, DataType>::Size() const {
    return m_DataMap.size();
}

template<typename IndexType, typename DataType>
void CDataStore<IndexType, DataType>::ForEach(TDataProcessor const &fnExecute) {
    for (auto &pair: m_DataMap) {
        fnExecute(*pair.second);
    }
}
