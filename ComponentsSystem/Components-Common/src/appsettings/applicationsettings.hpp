#pragma once

#include <Components/ExtraClasses/Utility/SingletonDecorator.h>

#include <string>
#include <memory>
#include <map>
#include <set>

#include "appsettingscommon.hpp"
#include "appsetting.hpp"


namespace Common {

class ApplicationSettings : public ExtraClasses::SingletonDecorator {
    ApplicationSettings();
public:
    ~ApplicationSettings();

    bool hasSetting(const std::string& section, const std::string& settingName);
    std::shared_ptr<AppSetting> addSetting(const std::string& section, const std::string& settingName);
    void addSetting(const std::string& section, const std::shared_ptr<AppSetting> &pSetting);
    std::shared_ptr<AppSetting> getSetting(const std::string& section, const std::string& settingName) const;

    // Работа с файлом настроек и классом
    static ApplicationSettings& getInstance();

    bool parseArguments(int argc, char* argv[]);
    std::shared_ptr<AppSetting> getArgument(const std::string& valName) const;

    // Работа с файлом настроек для внешних целей (загрузка профилей, например)
    void loadSettings(const std::string& configPath = {});
    void saveSettings(const std::string& configPath = {}) const;

private:
    std::map<std::string, std::set<std::shared_ptr<AppSetting> > > m_settingSections;
    std::string m_currentConfigsPath {"default.ini"};

    std::map<std::string, std::shared_ptr<AppSetting> > m_arguments;
};

} // namespace Common
