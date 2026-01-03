#pragma once

#include <QString>

#include <vector>

namespace Graph::SettingsNames {

using settingType_t = QString;
using settingConstType_t = const settingType_t;


settingConstType_t SYSTEM_PREFIX {"GES_"};

settingConstType_t CANVAS {SYSTEM_PREFIX + "canvas"};
settingConstType_t CANVAS_SIZE {"c_size"};
const std::vector<settingType_t> CANVAS_SETTINGS {
    CANVAS_SIZE,
};

settingConstType_t SESSIONCACHE {SYSTEM_PREFIX + "session_cache"};
settingConstType_t RECENTFILES {"recent_files"};
const std::vector<settingType_t> SESSIONCACHE_SETTINGS {
    RECENTFILES,
};

}
