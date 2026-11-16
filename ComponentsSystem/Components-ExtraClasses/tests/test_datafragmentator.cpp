#include <gtest/gtest.h>

#include <Components/ExtraClasses/DataFragmentator.h>

#include <algorithm>
#include <random>

uint64_t generateNumber(uint64_t min, uint64_t max)
{
    if (min >= max) throw std::invalid_argument("generateNumber: max >= min");

    // Create a random number generator
    std::random_device rd;
    std::mt19937 rng(rd());

    // Define the distribution
    std::uniform_int_distribution<uint64_t> uni(min, max);

    // Generate and return the random number
    return uni(rng);
}

TEST(Components_ExtraClasses, DataFragmentatorRegular) {
    ExtraClasses::DataFragmentator df;

    const uint64_t dataSize = 2000;
    std::vector<uint8_t> data;
    data.reserve(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        data.push_back(generateNumber(0, 255));
    }

    df.registerData(1, dataSize);
    auto pData = df.getData(1);
    ASSERT_TRUE(pData != nullptr);
    ASSERT_FALSE(pData->isValid());

    df.registerData(2, dataSize);
    df.unregisterData(2);
    auto pDataTest = df.getData(2);
    ASSERT_TRUE(pDataTest == nullptr);

    std::vector< std::vector<uint8_t> > splittedData;
    uint64_t curpos {};
    while (curpos < dataSize) {
        std::vector<uint8_t> splittedDataPart;
        auto deltaSize = std::min(generateNumber(1, 500), dataSize - curpos);
        splittedDataPart.reserve(deltaSize);
        std::copy_n(data.data() + curpos, deltaSize, std::back_inserter(splittedDataPart));
        curpos += deltaSize;
        splittedData.emplace_back(std::move(splittedDataPart));
    }

    curpos = 0;
    for (auto& splD : splittedData) {
        auto prevSize = pData->getActualSize();
        ASSERT_TRUE(df.addFragment(1, ExtraClasses::DataFragment{curpos, splD}));
        ASSERT_NE(prevSize, pData->getActualSize());
        ASSERT_EQ(prevSize + splD.size(), pData->getActualSize());
        curpos += splD.size();
    }
    ASSERT_TRUE(pData->isValid()) << "Actual size: " << pData->getActualSize() << " target size: " << pData->getSize();

    curpos = 0;
    for (auto& dataPart : pData->getFragments()) {
        ASSERT_EQ(splittedData[curpos], dataPart.data);
        ++curpos;
    }

    // Compile check
    ASSERT_TRUE(pData->compile());
    ASSERT_EQ(data, pData->getFragments().begin()->data);

    // Split check
    const auto splitChunkSize {100};
    ASSERT_TRUE(pData->split(splitChunkSize));
    ASSERT_EQ(dataSize / splitChunkSize, pData->getFragments().size());

    // Compile after splitting check (data equality check)
    ASSERT_TRUE(pData->compile());
    ASSERT_EQ(data, pData->getFragments().begin()->data);
}


TEST(Components_ExtraClasses, DataFragmentatorStaticFunctions) {
    const uint64_t dataSize = 2000;
    std::vector<uint8_t> data;
    data.reserve(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        data.push_back(generateNumber(0, 255));
    }

    std::vector< std::vector<uint8_t> > splittedData;
    uint64_t curpos {};
    while (curpos < dataSize) {
        std::vector<uint8_t> splittedDataPart;
        auto deltaSize = std::min(generateNumber(1, 500), dataSize - curpos);
        splittedDataPart.reserve(deltaSize);
        std::copy_n(data.data() + curpos, deltaSize, std::back_inserter(splittedDataPart));
        curpos += deltaSize;
        splittedData.emplace_back(std::move(splittedDataPart));
    }

    // Compile check
    auto compiledData = ExtraClasses::DataInfo::compile(splittedData);
    ASSERT_EQ(dataSize, compiledData.size());
    ASSERT_EQ(data, compiledData);

    // Split check
    const auto splitChunkSize {100};
    auto splittedCheckData = ExtraClasses::DataInfo::split(data, splitChunkSize);
    ASSERT_EQ(data.size() /  splitChunkSize, splittedCheckData.size());

    // Compile after splitting check (data equality check)
    compiledData = ExtraClasses::DataInfo::compile(splittedCheckData);
    ASSERT_EQ(dataSize, compiledData.size());
    ASSERT_EQ(data, compiledData);
}
