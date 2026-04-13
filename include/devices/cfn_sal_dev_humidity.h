/**
 * @file cfn_sal_dev_humidity.h
 * @brief Abstract Humidity Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEV_HUMIDITY_H
#define CAFFEINE_SAL_DEV_HUMIDITY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_HUMIDITY CFN_SAL_TYPE('H', 'U', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_HUMIDITY_MODE_CONTINUOUS,
    CFN_SAL_DEV_HUMIDITY_MODE_ONE_SHOT,
} cfn_sal_dev_humidity_mode_t;

typedef enum
{
    CFN_SAL_DEV_HUMIDITY_EVENT_NONE       = 0,
    CFN_SAL_DEV_HUMIDITY_EVENT_DATA_READY = CFN_HAL_BIT(0),
} cfn_sal_dev_humidity_event_t;

typedef enum
{
    CFN_SAL_DEV_HUMIDITY_OVERSAMPLING_NONE,
    CFN_SAL_DEV_HUMIDITY_OVERSAMPLING_1X,
    CFN_SAL_DEV_HUMIDITY_OVERSAMPLING_2X,
    CFN_SAL_DEV_HUMIDITY_OVERSAMPLING_4X,
    CFN_SAL_DEV_HUMIDITY_OVERSAMPLING_8X,
    CFN_SAL_DEV_HUMIDITY_OVERSAMPLING_16X,
} cfn_sal_dev_humidity_oversampling_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    cfn_sal_dev_humidity_mode_t         mode;
    cfn_sal_dev_humidity_oversampling_t oversampling;
    uint32_t                            sampling_rate_hz;
    int32_t                             high_threshold;
    int32_t                             low_threshold;
    void                               *custom;
} cfn_sal_dev_humidity_config_t;

typedef struct cfn_sal_dev_humidity_s     cfn_sal_dev_humidity_t;
typedef struct cfn_sal_dev_humidity_api_s cfn_sal_dev_humidity_api_t;

typedef void (*cfn_sal_dev_humidity_callback_t)(cfn_sal_dev_humidity_t *driver,
                                                uint32_t                event,
                                                uint32_t                error,
                                                void                   *user_arg);

/**
 * @brief Humidity Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_dev_humidity_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_relative_humidity)(cfn_sal_dev_humidity_t *driver, float *hum_out);
    cfn_hal_error_code_t (*read_raw)(cfn_sal_dev_humidity_t *driver, int32_t *raw_out);

    /* Configuration */
    cfn_hal_error_code_t (*start_conversion)(cfn_sal_dev_humidity_t *driver);
    cfn_hal_error_code_t (*get_status)(cfn_sal_dev_humidity_t *driver, uint32_t *status_flags);

    /* Environmental & Device Features */
    cfn_hal_error_code_t (*enable_heater)(cfn_sal_dev_humidity_t *driver, uint32_t power_mw, uint32_t duration_ms);
    cfn_hal_error_code_t (*soft_reset)(cfn_sal_dev_humidity_t *driver);
    cfn_hal_error_code_t (*get_id)(cfn_sal_dev_humidity_t *driver, uint32_t *id_out);

    void (*handle_interrupt)(cfn_sal_dev_humidity_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_dev_humidity_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_humidity,
                           cfn_sal_dev_humidity_config_t,
                           cfn_sal_dev_humidity_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_humidity_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_humidity_populate(cfn_sal_dev_humidity_t              *driver,
                                                  uint32_t                             peripheral_id,
                                                  void                                *dependency,
                                                  const cfn_sal_dev_humidity_api_t    *api,
                                                  const cfn_sal_phy_t                 *phy,
                                                  const cfn_sal_dev_humidity_config_t *config,
                                                  cfn_sal_dev_humidity_callback_t      callback,
                                                  void                                *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_HUMIDITY, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_humidity_construct(cfn_sal_dev_humidity_t              *driver,
                                                    const cfn_sal_dev_humidity_config_t *config,
                                                    const cfn_sal_phy_t                 *phy,
                                                    void                                *dependency,
                                                    cfn_sal_dev_humidity_callback_t      callback,
                                                    void                                *user_arg);
cfn_hal_error_code_t cfn_sal_dev_humidity_destruct(cfn_sal_dev_humidity_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_init(cfn_sal_dev_humidity_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_deinit(cfn_sal_dev_humidity_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_config_set(cfn_sal_dev_humidity_t              *driver,
                                                                    const cfn_sal_dev_humidity_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_config_get(cfn_sal_dev_humidity_t        *driver,
                                                                    cfn_sal_dev_humidity_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_callback_register(cfn_sal_dev_humidity_t         *driver,
                                                                           cfn_sal_dev_humidity_callback_t callback,
                                                                           void                           *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_power_state_set(cfn_sal_dev_humidity_t *driver,
                                                                         cfn_hal_power_state_t   state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_event_enable(cfn_sal_dev_humidity_t *driver,
                                                                      uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_event_disable(cfn_sal_dev_humidity_t *driver,
                                                                       uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_event_get(cfn_sal_dev_humidity_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_error_enable(cfn_sal_dev_humidity_t *driver,
                                                                      uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_error_disable(cfn_sal_dev_humidity_t *driver,
                                                                       uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_error_get(cfn_sal_dev_humidity_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_HUMIDITY, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_read_rh(cfn_sal_dev_humidity_t *driver, float *hum_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_HUMIDITY, read_relative_humidity, driver, error, hum_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_read_raw(cfn_sal_dev_humidity_t *driver, int32_t *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_HUMIDITY, read_raw, driver, error, raw_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_start_conversion(cfn_sal_dev_humidity_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_HUMIDITY, start_conversion, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_get_status(cfn_sal_dev_humidity_t *driver,
                                                                    uint32_t               *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_HUMIDITY, get_status, driver, error, status_flags);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_enable_heater(cfn_sal_dev_humidity_t *driver,
                                                                       uint32_t                power_mw,
                                                                       uint32_t                duration_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_HUMIDITY, enable_heater, driver, error, power_mw, duration_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_soft_reset(cfn_sal_dev_humidity_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_HUMIDITY, soft_reset, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_humidity_get_id(cfn_sal_dev_humidity_t *driver, uint32_t *id_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_HUMIDITY, get_id, driver, error, id_out);
    return error;
}

CFN_HAL_INLINE void cfn_sal_dev_humidity_handle_interrupt(cfn_sal_dev_humidity_t *driver)
{
    if (driver && driver->base.type == CFN_SAL_DEV_TYPE_HUMIDITY && driver->api)
    {
        if (driver->api->handle_interrupt)
        {
            driver->api->handle_interrupt(driver);
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_HUMIDITY_H */