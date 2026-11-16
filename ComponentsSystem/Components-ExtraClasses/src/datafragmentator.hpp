#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <mutex>
#include <memory>
#include <optional>
#include <set>
#include <list>
#include <atomic>

namespace ExtraClasses
{

using DataId_t = uint64_t;

/**
 * @brief The DataFragment class Fragment of data
 */
struct DataFragment
{
    DataId_t beginBytePos {};
    std::vector<uint8_t> data;

    bool operator <(const DataFragment& odf) const;
};

/**
 * @brief The DataInfo class Info about data (file, frame, etc.)
 */
class DataInfo
{
public:
    DataInfo(DataId_t id, uint64_t dataSize);

    DataId_t getId() const;
    uint64_t getSize() const;
    uint64_t getActualSize() const;

    void addFragment(DataFragment&& data);
    const std::set<DataFragment>& getFragments() const;

    /**
     * @brief isValid   Check if all data parts got
     * @return
     */
    bool isValid() const;

    /**
     * @brief compile   Connects all data fragments into one item
     * @return          true if succeed
     * @note            After succeed, there will be only one fragment of data, if it exist
     */
    bool compile();
    static std::vector<uint8_t> compile(const std::vector<std::vector<uint8_t> >& dataVectors);
    static std::string compile(const std::vector<std::string >& dataVectors);

    /**
     * @brief split         Reverse of compile method. Splits data into fragments
     * @param splitDataSize Size of one fragment
     * @return              true if succeed
     */
    bool split(uint64_t splitDataSize); // TODO: Modify using allocators? --> split without allocating extra memory
    static std::vector<std::vector<uint8_t> > split(const std::vector<uint8_t>& data, uint64_t splitDataSize);
    static std::vector<std::string > split(const std::string& data, uint64_t splitDataSize);

    bool operator <(const DataInfo& odi) const;
    bool operator ==(DataId_t id) const;

private:
    DataId_t m_id {};

    uint64_t m_size {};
    std::atomic<uint64_t> m_actualSize {};

    std::mutex m_fragmentMx;
    std::set<DataFragment> m_fragments;
};


using DataInfoPtr = std::shared_ptr<DataInfo>;

/**
 * @brief The DataFragmentator class Fragmentator to separate data into parts (for example, to collect big file, etc.)
 * @note Thread-safe
 */
class DataFragmentator
{
public:
    void registerData(DataId_t dataId, uint64_t dataSize);
    void unregisterData(DataId_t dataId);
    bool addFragment(DataId_t dataId, DataFragment&& data);
    DataInfoPtr getData(uint64_t dataId);

private:
    std::mutex m_dataRegisterMx;
    std::list<DataInfoPtr> m_data;
};

}
