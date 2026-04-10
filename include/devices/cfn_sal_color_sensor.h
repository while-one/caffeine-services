/**
 * @file cfn_sal_color_sensor.h
 * @brief Abstract Color Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_COLOR_SENSOR_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_COLOR_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_COLOR_SENSOR CFN_SAL_TYPE('C', 'O', 'L')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_COLOR_EVENT_NONE           = 0,
    CFN_SAL_COLOR_EVENT_DATA_READY     = CFN_HAL_BIT(0),
    CFN_SAL_COLOR_EVENT_THRESHOLD_HIGH = CFN_HAL_BIT(1),
    CFN_SAL_COLOR_EVENT_THRESHOLD_LOW  = CFN_HAL_BIT(2),
} cfn_sal_color_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_sal_color_config_t;

typedef struct cfn_sal_color_sensor_s     cfn_sal_color_sensor_t;
typedef struct cfn_sal_color_sensor_api_s cfn_sal_color_sensor_api_t;

typedef void (*cfn_sal_color_callback_t)(cfn_sal_color_sensor_t *driver,
                                         uint32_t                event,
                                         uint32_t                error,
                                         void                   *user_arg);

/**
 * @brief Color Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_color_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_xyz)(cfn_sal_color_sensor_t *driver, float *x, float *y, float *z);
    cfn_hal_error_code_t (*read_cct)(cfn_sal_color_sensor_t *driver, float *cct_k);
    cfn_hal_error_code_t (*read_raw_channels)(
        cfn_sal_color_sensor_t *driver, uint32_t *ch0, uint32_t *ch1, uint32_t *ch2, uint32_t *ch3);

    /* Configuration */
    cfn_hal_error_code_t (*set_integration_time)(cfn_sal_color_sensor_t *driver, uint32_t ms);
    cfn_hal_error_code_t (*set_auto_range)(cfn_sal_color_sensor_t *driver, bool enable);
    cfn_hal_error_code_t (*set_thresholds)(cfn_sal_color_sensor_t *driver, uint32_t low, uint32_t high);
    cfn_hal_error_code_t (*get_status)(cfn_sal_color_sensor_t *driver, uint32_t *status_flags);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_color_sensor_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_color_sensor, cfn_sal_color_config_t, cfn_sal_color_sensor_api_t, cfn_sal_phy_t, cfn_sal_color_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_color_sensor_populate(cfn_sal_color_sensor_t           *driver,
                                                  uint32_t                          peripheral_id,
                                                  void                             *dependency,
                                                  const cfn_sal_color_sensor_api_t *api,
                                                  const cfn_sal_phy_t              *phy,
                                                  const cfn_sal_color_config_t     *config,
                                                  cfn_sal_color_callback_t          callback,
                                                  void                             *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_COLOR_SENSOR, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_color_sensor_construct(cfn_sal_color_sensor_t       *driver,
                                                    const cfn_sal_color_config_t *config,
                                                    const cfn_sal_phy_t          *phy,
                                                    void                         *dependency,
                                                    cfn_sal_color_callback_t      callback,
                                                    void                         *user_arg);
cfn_hal_error_code_t cfn_sal_color_sensor_destruct(cfn_sal_color_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_init(cfn_sal_color_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_deinit(cfn_sal_color_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_config_set(cfn_sal_color_sensor_t       *driver,
                                                                    const cfn_sal_color_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_config_get(cfn_sal_color_sensor_t *driver,
                                                                    cfn_sal_color_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_callback_register(cfn_sal_color_sensor_t  *driver,
                                                                           cfn_sal_color_callback_t callback,
                                                                           void                    *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_TYPE_COLOR_SENSOR, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_power_state_set(cfn_sal_color_sensor_t *driver,
                                                                         cfn_hal_power_state_t   state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_event_enable(cfn_sal_color_sensor_t *driver,
                                                                      uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_event_disable(cfn_sal_color_sensor_t *driver,
                                                                       uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_event_get(cfn_sal_color_sensor_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_error_enable(cfn_sal_color_sensor_t *driver,
                                                                      uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_error_disable(cfn_sal_color_sensor_t *driver,
                                                                       uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_error_get(cfn_sal_color_sensor_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_COLOR_SENSOR, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_read_xyz(cfn_sal_color_sensor_t *driver,
                                                                  float                  *x,
                                                                  float                  *y,
                                                                  float                  *z)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, read_xyz, driver, error, x, y, z);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_read_cct(cfn_sal_color_sensor_t *driver, float *cct_k)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, read_cct, driver, error, cct_k);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_read_raw_channels(
    cfn_sal_color_sensor_t *driver, uint32_t *ch0, uint32_t *ch1, uint32_t *ch2, uint32_t *ch3)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, read_raw_channels, driver, error, ch0, ch1, ch2, ch3);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_set_integration_time(cfn_sal_color_sensor_t *driver,
                                                                              uint32_t                ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, set_integration_time, driver, error, ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_set_auto_range(cfn_sal_color_sensor_t *driver, bool enable)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, set_auto_range, driver, error, enable);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_set_thresholds(cfn_sal_color_sensor_t *driver,
                                                                        uint32_t                low,
                                                                        uint32_t                high)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, set_thresholds, driver, error, low, high);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_color_sensor_get_status(cfn_sal_color_sensor_t *driver,
                                                                    uint32_t               *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_COLOR_SENSOR, get_status, driver, error, status_flags);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_COLOR_SENSOR_H */
