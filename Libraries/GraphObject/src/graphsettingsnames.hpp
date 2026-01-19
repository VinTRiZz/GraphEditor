#pragma once

#include <QString>

#include <vector>
#include <map>

namespace Graph::SettingsNames {

using settingType_t = QString;
using settingConstType_t = const settingType_t;


settingConstType_t SYSTEM_PREFIX {"GES_"};

settingConstType_t DEBUGCONFIG {SYSTEM_PREFIX + "debug"};
settingConstType_t DEBUGCONFIG_JSON_SAVE_MODE {SYSTEM_PREFIX + "gsj_save_mode"};
const std::vector<settingType_t> DEBUGCONFIG_SETTINGS {
    DEBUGCONFIG_JSON_SAVE_MODE,
};

settingConstType_t CANVASCONFIG {SYSTEM_PREFIX + "canvas"};
settingConstType_t CANVASCONFIG_SIZE_TYPE {"size_type"};
settingConstType_t CANVASCONFIG_SIZE {"size"};
settingConstType_t CANVASCONFIG_ORIENTATION {"orientation"};
const std::vector<settingType_t> CANVASCONFIG_SETTINGS {
    CANVASCONFIG_SIZE_TYPE,
    CANVASCONFIG_SIZE,
    CANVASCONFIG_ORIENTATION,
};

// Для упрощения кода
const std::map<settingType_t, std::vector<settingType_t> > SETTING_SECTIONS {
    {DEBUGCONFIG, DEBUGCONFIG_SETTINGS},
    {CANVASCONFIG, CANVASCONFIG_SETTINGS}
};

}
