#pragma once

#include "sdkconfig.h"

#if defined(CONFIG_APP_FEATURE_MQTT) && CONFIG_APP_FEATURE_MQTT
#define APP_FEATURE_MQTT 1
#else
#define APP_FEATURE_MQTT 0
#endif

#if defined(CONFIG_APP_FEATURE_BLE) && CONFIG_APP_FEATURE_BLE
#define APP_FEATURE_BLE 1
#else
#define APP_FEATURE_BLE 0
#endif

#if defined(CONFIG_APP_FEATURE_MESH) && CONFIG_APP_FEATURE_MESH
#define APP_FEATURE_MESH 1
#else
#define APP_FEATURE_MESH 0
#endif

