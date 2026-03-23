/**
 * @file cfn_svc_hum_sensor.h
 * @brief Abstract Humidity Sensor service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_HUM_SENSOR_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_HUM_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_HUM_SENSOR CFN_SVC_TYPE('H', 'U', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_HUM_EVENT_NONE = 0,
    CFN_SVC_HUM_EVENT_DATA_READY = CFN_HAL_BIT(0),
} cfn_svc_hum_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_svc_hum_config_t;

typedef struct cfn_svc_hum_sensor_s     cfn_svc_hum_sensor_t;
typedef struct cfn_svc_hum_sensor_api_s cfn_svc_hum_sensor_api_t;

typedef void (*cfn_svc_hum_callback_t)(cfn_svc_hum_sensor_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Humidity Sensor Virtual Method Table (VMT).
 */
struct cfn_svc_hum_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_relative_humidity)(cfn_svc_hum_sensor_t *driver, float *hum_out);
    cfn_hal_error_code_t (*read_raw)(cfn_svc_hum_sensor_t *driver, int32_t *raw_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_resolution)(cfn_svc_hum_sensor_t *driver, uint8_t bits);
    cfn_hal_error_code_t (*get_resolution)(cfn_svc_hum_sensor_t *driver, uint8_t *bits_out);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_hum_sensor_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_hum_sensor, cfn_svc_hum_config_t, cfn_svc_hum_sensor_api_t, cfn_svc_phy_t, cfn_svc_hum_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_hum_sensor_populate(cfn_svc_hum_sensor_t           *driver,
                                                uint32_t                        peripheral_id,
                                                const cfn_svc_hum_sensor_api_t *api,
                                                const cfn_svc_phy_t            *phy,
                                                const cfn_svc_hum_config_t     *config,
                                                cfn_svc_hum_callback_t          callback,
                                                void                           *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_hum_sensor_construct(cfn_svc_hum_sensor_t       *driver,
                                                  const cfn_svc_hum_config_t *config,
                                                  const cfn_svc_phy_t        *phy,
                                                  cfn_svc_hum_callback_t      callback,
                                                  void                       *user_arg);
cfn_hal_error_code_t cfn_svc_hum_sensor_destruct(cfn_svc_hum_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_init(cfn_svc_hum_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_HUM_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_deinit(cfn_svc_hum_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_HUM_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_config_set(cfn_svc_hum_sensor_t       *driver,
                                                                  const cfn_svc_hum_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_config_get(cfn_svc_hum_sensor_t *driver,
                                                                  cfn_svc_hum_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_callback_register(cfn_svc_hum_sensor_t  *driver,
                                                                         cfn_svc_hum_callback_t callback,
                                                                         void                  *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SVC_TYPE_HUM_SENSOR, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_power_state_set(cfn_svc_hum_sensor_t *driver,
                                                                       cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_event_enable(cfn_svc_hum_sensor_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_event_disable(cfn_svc_hum_sensor_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_event_get(cfn_svc_hum_sensor_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_error_enable(cfn_svc_hum_sensor_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_error_disable(cfn_svc_hum_sensor_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_error_get(cfn_svc_hum_sensor_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_HUM_SENSOR, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_read_rh(cfn_svc_hum_sensor_t *driver, float *hum_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_HUM_SENSOR, read_relative_humidity, driver, error, hum_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_read_raw(cfn_svc_hum_sensor_t *driver, int32_t *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_HUM_SENSOR, read_raw, driver, error, raw_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_set_resolution(cfn_svc_hum_sensor_t *driver, uint8_t bits)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_HUM_SENSOR, set_resolution, driver, error, bits);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_get_resolution(cfn_svc_hum_sensor_t *driver, uint8_t *bits_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_HUM_SENSOR, get_resolution, driver, error, bits_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_HUM_SENSOR_H */
