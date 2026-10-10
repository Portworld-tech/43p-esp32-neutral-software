/**
 * @file aht20_calib.h
 * @brief Customer-editable AHT20 temperature / humidity adjustment macros.
 *
 * Edit the macros below, then rebuild + flash. Values from aht20_read() /
 * aht20_read_temperature_c() / aht20_read_humidity_rh() (and the Hub UI path
 * that uses them) will apply this linear calibration:
 *
 *   out = raw * SCALE + OFFSET
 *
 * Use aht20_read_raw() if you need the sensor value before calibration.
 *
 * Examples:
 *   - Display reads 1.5 °C high  →  #define AHT20_TEMP_OFFSET_C  (-1.5f)
 *   - Humidity reads 3 %RH low   →  #define AHT20_RH_OFFSET_PCT (3.0f)
 *   - Fine gain on temperature   →  #define AHT20_TEMP_SCALE    (1.02f)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Set to 0 to disable all calibration (raw sensor math only). */
#ifndef AHT20_CALIB_ENABLE
#define AHT20_CALIB_ENABLE 1
#endif

/**
 * Temperature (°C): out = raw * AHT20_TEMP_SCALE + AHT20_TEMP_OFFSET_C
 * Default: identity (no change).
 */
#ifndef AHT20_TEMP_OFFSET_C
#define AHT20_TEMP_OFFSET_C 0.0f
#endif

#ifndef AHT20_TEMP_SCALE
#define AHT20_TEMP_SCALE 1.0f
#endif

/**
 * Relative humidity (%RH): out = raw * AHT20_RH_SCALE + AHT20_RH_OFFSET_PCT
 * Default: identity (no change).
 */
#ifndef AHT20_RH_OFFSET_PCT
#define AHT20_RH_OFFSET_PCT 0.0f
#endif

#ifndef AHT20_RH_SCALE
#define AHT20_RH_SCALE 1.0f
#endif

/**
 * After RH calibration, clamp to 0..100 when enabled (1).
 * Temperature is not clamped so sub-zero / high values remain valid.
 */
#ifndef AHT20_RH_CLAMP
#define AHT20_RH_CLAMP 1
#endif

#ifdef __cplusplus
}
#endif
