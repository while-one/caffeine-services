/**
 * @file cfn_sal_light_sensor.h
 * @brief Abstract Ambient Light Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_LIGHT_SENSOR_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_LIGHT_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_LIGHT_SENSOR CFN_SAL_TYPE('L', 'U', 'X')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_LIGHT_EVENT_NONE           = 0,
    CFN_SAL_LIGHT_EVENT_DATA_READY     = CFN_HAL_BIT(0),
    CFN_SAL_LIGHT_EVENT_THRESHOLD_HIGH = CFN_HAL_BIT(1),
    CFN_SAL_LIGHT_EVENT_THRESHOLD_LOW  = CFN_HAL_BIT(2),
} cfn_sal_light_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_sal_light_config_t;

typedef struct cfn_sal_light_sensor_s     cfn_sal_light_sensor_t;
typedef struct cfn_sal_light_sensor_api_s cfn_sal_light_sensor_api_t;

typedef void (*cfn_sal_light_callback_t)(cfn_sal_light_sensor_t *driver,
                                         uint32_t                event,
                                         uint32_t                error,
                                         void                   *user_arg);

/**
 * @brief Light Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_light_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_lux)(cfn_sal_light_sensor_t *driver, float *lux_out);
    cfn_hal_error_code_t (*read_raw)(cfn_sal_light_sensor_t *driver, uint32_t *raw_out);
    cfn_hal_error_code_t (*read_channels)(cfn_sal_light_sensor_t *driver, uint16_t *ch1, uint16_t *ch2);

    /* Configuration */
    cfn_hal_error_code_t (*set_gain)(cfn_sal_light_sensor_t *driver, uint32_t gain);
    cfn_hal_error_code_t (*set_integration_time)(cfn_sal_light_sensor_t *driver, uint32_t ms);
    cfn_hal_error_code_t (*set_thresholds)(cfn_sal_light_sensor_t *driver, uint32_t low_lux, uint32_t high_lux);
    cfn_hal_error_code_t (*set_persistence)(cfn_sal_light_sensor_t *driver, uint8_t consec_hits);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_light_sensor_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_light_sensor, cfn_sal_light_config_t, cfn_sal_light_sensor_api_t, cfn_sal_phy_t, cfn_sal_light_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_light_sensor_populate(cfn_sal_light_sensor_t           *driver,
                                                  uint32_t                          peripheral_id,
                                                  void                             *dependency,
                                                  const cfn_sal_light_sensor_api_t *api,
                                                  const cfn_sal_phy_t              *phy,
                                                  const cfn_sal_light_config_t     *config,
                                                  cfn_sal_light_callback_t          callback,
                                                  void                             *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_LIGHT_SENSOR, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_light_sensor_construct(cfn_sal_light_sensor_t       *driver,
                                                    const cfn_sal_light_config_t *config,
                                                    const cfn_sal_phy_t          *phy,
                                                    void                         *dependency,
                                                    cfn_sal_light_callback_t      callback,
                                                    void                         *user_arg);
cfn_hal_error_code_t cfn_sal_light_sensor_destruct(cfn_sal_light_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_init(cfn_sal_light_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_deinit(cfn_sal_light_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_config_set(cfn_sal_light_sensor_t       *driver,
                                                                    const cfn_sal_light_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_config_get(cfn_sal_light_sensor_t *driver,
                                                                    cfn_sal_light_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_callback_register(cfn_sal_light_sensor_t  *driver,
                                                                           cfn_sal_light_callback_t callback,
                                                                           void                    *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_power_state_set(cfn_sal_light_sensor_t *driver,
                                                                         cfn_hal_power_state_t   state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_event_enable(cfn_sal_light_sensor_t *driver,
                                                                      uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_event_disable(cfn_sal_light_sensor_t *driver,
                                                                       uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_event_get(cfn_sal_light_sensor_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_error_enable(cfn_sal_light_sensor_t *driver,
                                                                      uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_error_disable(cfn_sal_light_sensor_t *driver,
                                                                       uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_error_get(cfn_sal_light_sensor_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_LIGHT_SENSOR, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_read_lux(cfn_sal_light_sensor_t *driver, float *lux_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, read_lux, driver, error, lux_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_read_raw(cfn_sal_light_sensor_t *driver, uint32_t *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, read_raw, driver, error, raw_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_read_channels(cfn_sal_light_sensor_t *driver,
                                                                       uint16_t               *ch1,
                                                                       uint16_t               *ch2)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, read_channels, driver, error, ch1, ch2);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_set_gain(cfn_sal_light_sensor_t *driver, uint32_t gain)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, set_gain, driver, error, gain);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_set_integration_time(cfn_sal_light_sensor_t *driver,
                                                                              uint32_t                ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, set_integration_time, driver, error, ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_set_thresholds(cfn_sal_light_sensor_t *driver,
                                                                        uint32_t                low_lux,
                                                                        uint32_t                high_lux)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, set_thresholds, driver, error, low_lux, high_lux);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_light_sensor_set_persistence(cfn_sal_light_sensor_t *driver,
                                                                         uint8_t                 consec_hits)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_LIGHT_SENSOR, set_persistence, driver, error, consec_hits);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_LIGHT_SENSOR_H */
