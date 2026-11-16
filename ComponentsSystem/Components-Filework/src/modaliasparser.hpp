#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

/// Modalias file located in /sys/class/drm/card0/device

namespace Filework
{
class ModaliasParser
{
  public:
    /** @brief ModaliasParser constructor
     *
     * @param modaliasInfo const std::string Got from modalias file data
     * std::string
     *
     */
    inline ModaliasParser(const std::string modaliasInfo);

    ~ModaliasParser() = default;

    /** @brief Gets type of device interface
     * @return const std::string Type of device interface represented like "pci"
     */
    inline const std::string getType() const noexcept
    {
        return m_deviceInfo["type"];
    }

    /** @brief Gets VID of device
     * @return const std::string Vendor ID represented like "1002"
     */
    inline const std::string getVID() const noexcept
    {
        return m_deviceInfo["vid"];
    }

    /** @brief Gets DID of device
     * @return const std::string Device ID represented like "67df"
     */
    inline const std::string getDID() const noexcept
    {
        return m_deviceInfo["did"];
    }

    /** @brief Gets VID of subdevice
     * @return const std::string Vendor ID of subdevice represented like "148c"
     */
    inline const std::string getSubVID() const noexcept
    {
        return m_deviceInfo["subsystemVid"];
    }

    /** @brief Gets DID of device
     * @return const std::string Device ID of subdevice represented like "2379"
     */
    inline const std::string getSubDID() const noexcept
    {
        return m_deviceInfo["subsystemDid"];
    }

    /** @brief Gets class code of device
     * @return const std::string Class code device represented like "3"
     */
    inline const std::string getClassCode() const noexcept
    {
        return m_deviceInfo["classCode"];
    }

    /** @brief Gets class code of subdevice
     * @return const std::string Class code subdevice represented like "2"
     */
    inline const std::string getSubClassCode() const noexcept
    {
        return m_deviceInfo["subClassCode"];
    }

    /** @brief Gets protocol code of device
     * @return const std::string Protocol code device represented like "1"
     */
    inline const std::string getProtocolCode() const noexcept
    {
        return m_deviceInfo["protocolCode"];
    }

  protected:
    void parse(std::string::const_iterator& currentPos,
               std::string::const_iterator& nextPos,
               const std::string parameterName);

    mutable std::map<const std::string, std::string> m_deviceInfo = {
        {"type", ""},      {"protocolCode", ""}, {"vid", ""},
        {"did", ""},       {"subsystemVid", ""}, {"subsystemDid", ""},
        {"classCode", ""}, {"subClassCode", ""}};
};

inline void ModaliasParser::parse(std::string::const_iterator& currentPos,
                                  std::string::const_iterator& nextPos,
                                  const std::string parameterName)
{
    try
    {
        std::string& paramRef = m_deviceInfo[parameterName];

        paramRef.resize(10);
        std::copy(currentPos, nextPos, paramRef.begin());
        paramRef.shrink_to_fit();
        paramRef.erase(std::remove_if(paramRef.begin(), paramRef.end(),
                                      [](char c) { return (c < 32); }),
                       paramRef.end()); // Remove garbage

        for (; (*currentPos == '0') and (currentPos != nextPos); currentPos++)
        {
            // Clear from useless zeros
            paramRef.erase(paramRef.begin(), paramRef.begin() + 1);
        }
        for (std::string::iterator didPos = paramRef.begin();
             didPos != paramRef.end(); didPos++)
        {
            // Set to lower symbols
            *didPos = std::tolower(*didPos);
        }
        if (paramRef[0] == '\0')
        {
            paramRef = "0";
        }
    } catch (...)
    {
        return;
    }
}

inline ModaliasParser::ModaliasParser(const std::string modaliasInfo)
{
    // Device type parsing
    std::string::const_iterator currentPos =
        std::find(modaliasInfo.begin(), modaliasInfo.end(), ':');
    m_deviceInfo["type"].resize(10);
    std::copy(modaliasInfo.begin(), currentPos, m_deviceInfo["type"].begin());
    m_deviceInfo["type"].shrink_to_fit();

    std::string::const_iterator nextPos;

    // Vendor ID parsing
    currentPos += 2;
    nextPos = currentPos + 8;
    parse(currentPos, nextPos, "vid");

    // Device ID parsing
    currentPos = nextPos + 1;
    nextPos    = currentPos + 8;
    parse(currentPos, nextPos, "did");

    // Subsystem VID parsing
    currentPos = nextPos + 2;
    nextPos    = currentPos + 8;
    parse(currentPos, nextPos, "subsystemVid");

    // Subsystem DID parsing
    currentPos = nextPos + 2;
    nextPos    = currentPos + 8;
    parse(currentPos, nextPos, "subsystemDid");

    // Class code parsing
    currentPos = nextPos + 2;
    nextPos    = currentPos + 2;
    parse(currentPos, nextPos, "classCode");

    // Subsystem class code parsing
    currentPos = nextPos + 2;
    nextPos    = currentPos + 2;
    parse(currentPos, nextPos, "subClassCode");

    // Protocol code parsing
    currentPos = nextPos + 1;
    nextPos    = currentPos + 2;
    parse(currentPos, nextPos, "protocolCode");
}
} // namespace Hardware::GPU
