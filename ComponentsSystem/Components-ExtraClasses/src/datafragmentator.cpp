#include "datafragmentator.hpp"

#include <algorithm>
#include <numeric>

namespace ExtraClasses
{

bool DataFragment::operator <(const DataFragment &odf) const {
    return (beginBytePos < odf.beginBytePos);
}

DataInfo::DataInfo(DataId_t id, uint64_t dataSize) :
    m_id {id},
    m_size {dataSize}
{

}

DataId_t DataInfo::getId() const
{
    return m_id;
}

uint64_t DataInfo::getSize() const
{
    return m_size;
}

uint64_t DataInfo::getActualSize() const
{
    return m_actualSize.load(std::memory_order_acquire);
}

void DataInfo::addFragment(DataFragment &&data)
{
    m_fragmentMx.lock();
    m_actualSize.fetch_add(data.data.size(), std::memory_order_release);
    m_fragments.emplace(std::move(data));
    m_fragmentMx.unlock();
}

const std::set<DataFragment> &DataInfo::getFragments() const
{
    return m_fragments;
}

bool DataInfo::isValid() const
{
    return (m_size != 0) && (m_size == getActualSize());
}

bool DataInfo::compile()
{
    if (!isValid()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(m_fragmentMx);
    if (m_fragments.size() == 1) {
        return true;
    }
    auto firstPart = std::move(*m_fragments.begin());
    m_fragments.erase(firstPart);

    for (auto& fragment :  m_fragments) {
        firstPart.data.reserve(fragment.data.size());
        std::copy_n(fragment.data.data(), fragment.data.size(), std::back_inserter(firstPart.data));
    }
    m_fragments.clear();

    auto isCompilationSucceed = (firstPart.data.size() == getActualSize());
    m_fragments.emplace(std::move(firstPart));
    return isCompilationSucceed;
}

std::vector<uint8_t> DataInfo::compile(const std::vector<std::vector<uint8_t> > &dataVectors)
{
    std::vector<uint8_t> res;

    auto dataBeg = dataVectors.data();
    auto currentVect = dataBeg;
    while ((currentVect - dataBeg) < dataVectors.size()) {
        res.reserve(currentVect->size());
        std::copy_n(currentVect->data(), currentVect->size(), std::back_inserter(res));
        ++currentVect;
    }

    auto totalSize = std::accumulate(dataVectors.begin(), dataVectors.end(), 0, [](auto curVal, auto& vect){
        return (curVal + vect.size());
    });
    if (res.size() != totalSize) {
        res.clear();
    }

    return res;
}

std::string DataInfo::compile(const std::vector<std::string> &dataVectors)
{
    std::string res;

    auto dataBeg = dataVectors.data();
    auto currentVect = dataBeg;
    while ((currentVect - dataBeg) < dataVectors.size()) {
        res.reserve(currentVect->size());
        std::copy_n(currentVect->data(), currentVect->size(), std::back_inserter(res));
        ++currentVect;
    }

    auto totalSize = std::accumulate(dataVectors.begin(), dataVectors.end(), 0, [](auto curVal, auto& vect){
        return (curVal + vect.size());
    });
    if (res.size() != totalSize) {
        res.clear();
    }

    return res;
}

bool DataInfo::split(uint64_t splitDataSize)
{
    if (!isValid()) {
        return false;
    }
    if (!compile()) {
        return false;
    }

    auto firstPart = std::move(*m_fragments.begin());
    m_fragments.erase(firstPart);

    uint64_t curpos {};
    const auto dataSize = firstPart.data.size();
    while (curpos < dataSize) {
        auto deltaSize = std::min(splitDataSize, dataSize - curpos);

        DataFragment splittedDataPart;
        splittedDataPart.data.reserve(deltaSize);
        std::copy_n(firstPart.data.data() + curpos, deltaSize, std::back_inserter(splittedDataPart.data));
        splittedDataPart.beginBytePos = curpos;
        m_fragments.emplace(std::move(splittedDataPart));

        curpos += deltaSize;
    }

    std::size_t totalDataSize {0};
    for (const auto& fragment : m_fragments) {
        totalDataSize += fragment.data.size();
    }
    return (totalDataSize == getActualSize());
}

std::vector<std::vector<uint8_t> > DataInfo::split(const std::vector<uint8_t> &data, uint64_t splitDataSize)
{
    std::vector< std::vector<uint8_t> > res;
    res.reserve(data.size() / splitDataSize);

    const auto dataSize = data.size();
    auto dataEnd = data.data() + dataSize;
    auto curDataStart = data.data();

    while (curDataStart < dataEnd) {
        auto deltaSize = std::min(splitDataSize, uint64_t(dataEnd - curDataStart));

        std::vector<uint8_t> dataPart;
        dataPart.reserve(deltaSize);
        std::copy_n(curDataStart, deltaSize, std::back_inserter(dataPart));
        curDataStart += deltaSize;

        res.push_back(dataPart);
    }

    std::size_t totalDataSize {0};
    for (const auto& fragment : res) {
        totalDataSize += fragment.size();
    }
    if (totalDataSize != data.size()) {
        res.clear();
    }
    return res;
}

std::vector<std::string> DataInfo::split(const std::string &data, uint64_t splitDataSize)
{
    std::vector< std::string > res;
    res.reserve(data.size() / splitDataSize);

    const auto dataSize = data.size();
    auto dataEnd = data.data() + dataSize;
    auto curDataStart = data.data();

    while (curDataStart < dataEnd) {
        auto deltaSize = std::min(splitDataSize, uint64_t(dataEnd - curDataStart));

        std::string dataPart;
        dataPart.reserve(deltaSize);
        std::copy_n(curDataStart, deltaSize, std::back_inserter(dataPart));
        curDataStart += deltaSize;

        res.push_back(dataPart);
    }

    std::size_t totalDataSize {0};
    for (const auto& fragment : res) {
        totalDataSize += fragment.size();
    }
    if (totalDataSize != data.size()) {
        res.clear();
    }
    return res;
}

bool DataInfo::operator <(const DataInfo &odi) const {
    return (m_id < odi.m_id);
}

bool DataInfo::operator ==(DataId_t id) const
{
    return (m_id == id);
}

void DataFragmentator::registerData(DataId_t dataId, uint64_t dataSize)
{
    m_dataRegisterMx.lock();
    m_data.push_front(std::make_shared<DataInfo>(dataId, dataSize));
    m_dataRegisterMx.unlock();
}

void DataFragmentator::unregisterData(DataId_t dataId)
{
    std::lock_guard<std::mutex> lock(m_dataRegisterMx);
    auto targetData = std::find_if(m_data.begin(), m_data.end(), [dataId](auto& dataInfo){
        return (dataInfo->getId() == dataId);
    });
    m_data.erase(targetData);
}

bool DataFragmentator::addFragment(DataId_t dataId, DataFragment &&data)
{
    decltype(m_data)::iterator targetData;
    {
        std::lock_guard<std::mutex> lock(m_dataRegisterMx);
        targetData = std::find_if(m_data.begin(), m_data.end(), [dataId](auto& dataInfo){
            return (dataInfo->getId() == dataId);
        });
        if (targetData == m_data.end()) {
            return false;
        }
    }
    (*targetData)->addFragment(std::move(data));
    return true;
}

DataInfoPtr DataFragmentator::getData(uint64_t dataId)
{
    decltype(m_data)::iterator targetData;
    {
        std::lock_guard<std::mutex> lock(m_dataRegisterMx);
        targetData = std::find_if(m_data.begin(), m_data.end(), [dataId](auto& dataInfo){
            return (dataInfo->getId() == dataId);
        });
        if (targetData == m_data.end()) {
            return {};
        }
    }
    return *targetData;
}

}
