#include "applicationsettings.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Filework/ConfigParsing/IniParser.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace Common {

ApplicationSettings::ApplicationSettings() {}

ApplicationSettings::~ApplicationSettings() {}

bool ApplicationSettings::hasSetting(const std::string &section, const std::string &settingName)
{
    return (getSetting(section, settingName).use_count() != 0);
}

std::shared_ptr<AppSetting> ApplicationSettings::addSetting(const std::string &section, const std::string &settingName)
{
    auto pSett = std::make_shared<AppSetting>();
    pSett->setName(settingName);
    m_settingSections[section].insert(pSett);
    return pSett;
}

void ApplicationSettings::addSetting(const std::string &section, const std::shared_ptr<AppSetting>& pSetting)
{
    m_settingSections[section].insert(pSetting);
}

std::shared_ptr<AppSetting> ApplicationSettings::getSetting(const std::string &section, const std::string &settingName) const
{
    if (!m_settingSections.count(section)) {
        return {};
    }

    for (const auto& pSetting : m_settingSections.at(section)) {
        if (pSetting->getName() == settingName) {
            return pSetting;
        }
    }
    return {};
}

ApplicationSettings& ApplicationSettings::getInstance() {
    static ApplicationSettings inst;
    return inst;
}

bool ApplicationSettings::parseArguments(int argc, char *argv[])
{
    std::string curargName;

    for (int i = 0; i < argc; ++i) {
        auto token = std::string(argv[i]);
        if (token[0] == '-') {
            curargName = token;
            curargName.erase(0, curargName.find_first_not_of('-'));
            continue;
        }
        auto& sett = m_arguments[curargName];
        if (sett->isSet()) {
            continue;
        }
        sett->setName(curargName);
        sett->setValue(token);
    }

    for (auto& [argName, argValue] : m_arguments) {
        if (argValue) {
            continue;
        }
        try {
            argValue->setValue(std::stoll(argValue->getValueString()));
        } catch (std::invalid_argument& ex) {
            try {
                argValue->setValue(std::stod(argValue->getValueString()));
            } catch (std::invalid_argument&) {
                // Do nothing
            }
        }
    }

    return true;
}

std::shared_ptr<AppSetting> ApplicationSettings::getArgument(const std::string &valName) const
{
    auto arg = m_arguments.find(valName);
    if (arg == m_arguments.end()) {
        return {};
    }
    return arg->second;
}

void ApplicationSettings::loadSettings(const std::string& configPath) {
    if (configPath.empty()) {
        return loadSettings(m_currentConfigsPath);
    }

    m_currentConfigsPath = configPath;
    COMPLOG_INFO("Loading settings from file:", configPath);

    if (!std::filesystem::exists(configPath)) {
        std::ofstream creator(configPath);
        creator.close();
        COMPLOG_INFO("Settings file not exist, created empty one");
    }

    Filework::IniFileParser iniParser;
    if (!iniParser.read(configPath)) {
        COMPLOG_ERROR("Failed to parse settings:", iniParser.getLastErrorText());
        return;
    }

    m_settingSections.clear();
    for (auto& groupName : iniParser.getSections()) {
        for (auto& [valueName, value] : iniParser.getSection(groupName)) {
            if (auto pSett = addSetting(groupName, valueName); pSett) {
                auto dotCount = std::count(value.begin(), value.end(), '.');
                if (dotCount == 0) {
                    try {
                        pSett->setValue(std::stoll(value.data()));
                    } catch ([[maybe_unused]] std::invalid_argument& ex) { // Ignore exception, it's normal
                        pSett->setValue(value.data());
                    }
                } else if (dotCount == 1) {
                    try {
                        pSett->setValue(std::stod(value.data()));
                    } catch ([[maybe_unused]] std::invalid_argument& ex) { // Ignore exception, it's normal
                        pSett->setValue(value.data());
                    }
                } else {
                    pSett->setValue(value.data());
                }
                continue;
            }
            addSetting(groupName, valueName);
            getSetting(groupName, valueName)->setValue(value.data());
        }
    }

    COMPLOG_OK("Settings loaded");
}

void ApplicationSettings::saveSettings(const std::string& configPath) const {
    if (configPath.empty()) {
        return saveSettings(m_currentConfigsPath);
    }

    COMPLOG_INFO("Saving settings to file:", configPath);

    Filework::IniFileParser iniParser;
    for (auto& [settGroup, setts] : m_settingSections) {
        std::map<std::string, std::string> sectionData;
        for (auto& pSett : setts) {
            sectionData[pSett->getName().data()] = pSett->getValueString();
        }
        iniParser.addSection(settGroup, std::move(sectionData));
    }

    if (!iniParser.write(configPath)) {
        COMPLOG_ERROR("Failed to save settings:", iniParser.getLastErrorText());
        return;
    }

    COMPLOG_OK("Settings saved");
}

}
