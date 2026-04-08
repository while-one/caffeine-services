/**
 * @file cfn_sal_hum_sensor.h
 * @brief Abstract Humidity Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_HUM_SENSOR_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_HUM_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_HUM_SENSOR CFN_SAL_TYPE('H', 'U', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_HUM_MODE_CONTINUOUS,
    CFN_SAL_HUM_MODE_ONE_SHOT,
} cfn_sal_hum_mode_t;

typedef enum
{
    CFN_SAL_HUM_EVENT_NONE       = 0,
    CFN_SAL_HUM_EVENT_DATA_READY = CFN_HAL_BIT(0),
} cfn_sal_hum_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_sal_hum_config_t;

typedef struct cfn_sal_hum_sensor_s     cfn_sal_hum_sensor_t;
typedef struct cfn_sal_hum_sensor_api_s cfn_sal_hum_sensor_api_t;

typedef void (*cfn_sal_hum_callback_t)(cfn_sal_hum_sensor_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Humidity Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_hum_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_relative_humidity)(cfn_sal_hum_sensor_t *driver, float *hum_out);
    cfn_hal_error_code_t (*read_raw)(cfn_sal_hum_sensor_t *driver, int32_t *raw_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_resolution)(cfn_sal_hum_sensor_t *driver, uint8_t bits);
    cfn_hal_error_code_t (*get_resolution)(cfn_sal_hum_sensor_t *driver, uint8_t *bits_out);
    cfn_hal_error_code_t (*set_mode)(cfn_sal_hum_sensor_t *driver, cfn_sal_hum_mode_t mode);
    cfn_hal_error_code_t (*start_conversion)(cfn_sal_hum_sensor_t *driver);
    cfn_hal_error_code_t (*set_oversampling)(cfn_sal_hum_sensor_t *driver, uint8_t factor);
    cfn_hal_error_code_t (*get_status)(cfn_sal_hum_sensor_t *driver, uint32_t *status_flags);

    /* Environmental & Device Features */
    cfn_hal_error_code_t (*enable_heater)(cfn_sal_hum_sensor_t *driver, uint32_t power_mw, uint32_t duration_ms);
    cfn_hal_error_code_t (*soft_reset)(cfn_sal_hum_sensor_t *driver);
    cfn_hal_error_code_t (*get_serial_number)(cfn_sal_hum_sensor_t *driver, uint32_t *serial_out);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_hum_sensor_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_hum_sensor, cfn_sal_hum_config_t, cfn_sal_hum_sensor_api_t, cfn_sal_phy_t, cfn_sal_hum_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_hum_sensor_populate(cfn_sal_hum_sensor_t           *driver,
                                                uint32_t                        peripheral_id,
                                                const cfn_sal_hum_sensor_api_t *api,
                                                const cfn_sal_phy_t            *phy,
                                                const cfn_sal_hum_config_t     *config,
                                                cfn_sal_hum_callback_t          callback,
                                                void                           *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api         = api;
    driver->phy         = phy;
    driver->config      = config;
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_sal_hum_sensor_construct(cfn_sal_hum_sensor_t       *driver,
                                                  const cfn_sal_hum_config_t *config,
                                                  const cfn_sal_phy_t        *phy,
                                                  cfn_sal_hum_callback_t      callback,
                                                  void                       *user_arg);
cfn_hal_error_code_t cfn_sal_hum_sensor_destruct(cfn_sal_hum_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_init(cfn_sal_hum_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_HUM_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_deinit(cfn_sal_hum_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_HUM_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_config_set(cfn_sal_hum_sensor_t       *driver,
                                                                  const cfn_sal_hum_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_config_get(cfn_sal_hum_sensor_t *driver,
                                                                  cfn_sal_hum_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_callback_register(cfn_sal_hum_sensor_t  *driver,
                                                                         cfn_sal_hum_callback_t callback,
                                                                         void                  *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_TYPE_HUM_SENSOR, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_power_state_set(cfn_sal_hum_sensor_t *driver,
                                                                       cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_event_enable(cfn_sal_hum_sensor_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_event_disable(cfn_sal_hum_sensor_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_event_get(cfn_sal_hum_sensor_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_error_enable(cfn_sal_hum_sensor_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_error_disable(cfn_sal_hum_sensor_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_error_get(cfn_sal_hum_sensor_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_HUM_SENSOR, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_read_rh(cfn_sal_hum_sensor_t *driver, float *hum_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, read_relative_humidity, driver, error, hum_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_read_raw(cfn_sal_hum_sensor_t *driver, int32_t *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, read_raw, driver, error, raw_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_set_resolution(cfn_sal_hum_sensor_t *driver, uint8_t bits)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, set_resolution, driver, error, bits);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_get_resolution(cfn_sal_hum_sensor_t *driver, uint8_t *bits_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, get_resolution, driver, error, bits_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_set_mode(cfn_sal_hum_sensor_t *driver, cfn_sal_hum_mode_t mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, set_mode, driver, error, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_start_conversion(cfn_sal_hum_sensor_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_HUM_SENSOR, start_conversion, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_set_oversampling(cfn_sal_hum_sensor_t *driver, uint8_t factor)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, set_oversampling, driver, error, factor);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_get_status(cfn_sal_hum_sensor_t *driver, uint32_t *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, get_status, driver, error, status_flags);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_enable_heater(cfn_sal_hum_sensor_t *driver,
                                                                     uint32_t              power_mw,
                                                                     uint32_t              duration_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, enable_heater, driver, error, power_mw, duration_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_soft_reset(cfn_sal_hum_sensor_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_HUM_SENSOR, soft_reset, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_hum_sensor_get_serial_number(cfn_sal_hum_sensor_t *driver,
                                                                         uint32_t             *serial_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_HUM_SENSOR, get_serial_number, driver, error, serial_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_HUM_SENSOR_H */
