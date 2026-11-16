#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace Filework {

/**
 * @brief The IniFileParser class   Parser for configuration files in .ini format
 */
class IniFileParser
{
public:
    IniFileParser();
    ~IniFileParser();

    void reset();

    bool read(const std::string_view &filePath, bool ignoreInvalidValues = false);
    bool write(const std::string_view &filePath);
    std::string getLastErrorText() const;

    std::vector<std::string> getSections() const;

    void addSection(const std::string& sectionName);
    void addSection(const std::string& sectionName, const std::map<std::string, std::string>& values);
    void addSection(const std::string& sectionName, std::map<std::string, std::string>&& values);

    void setSectionComment(const std::string& section,
                           const std::string& setting,
                           const std::string& comment);

    /**
     * @brief getSection    Get values of section in KEY - VALUE format
     * @param sectionName   If empty, will return global section
     * @return              Section values (empty on error or if actually empty)
     */
    std::map<std::string, std::string> getSection(const std::string& sectionName = {}) const;
    std::string getSettingComment(const std::string& setting, const std::string& section) const;

private:
    struct Impl;
    std::shared_ptr<Impl> d;

    std::string getCommentId(const std::string& section,
                             const std::string& setting) const;
};

} // namespace Filework
