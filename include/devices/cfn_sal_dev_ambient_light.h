/**
 * @file cfn_sal_dev_ambient_light.h
 * @brief Abstract Ambient Light Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEV_AMBIENT_LIGHT_H
#define CAFFEINE_SAL_DEV_AMBIENT_LIGHT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_AMBIENT_LIGHT CFN_SAL_TYPE('L', 'U', 'X')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_AMBIENT_LIGHT_EVENT_NONE           = 0,
    CFN_SAL_DEV_AMBIENT_LIGHT_EVENT_DATA_READY     = CFN_HAL_BIT(0),
    CFN_SAL_DEV_AMBIENT_LIGHT_EVENT_THRESHOLD_HIGH = CFN_HAL_BIT(1),
    CFN_SAL_DEV_AMBIENT_LIGHT_EVENT_THRESHOLD_LOW  = CFN_HAL_BIT(2),
} cfn_sal_dev_ambient_light_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t gain;
    uint32_t integration_time_ms;
    uint32_t high_threshold;
    uint32_t low_threshold;
    uint8_t  persistence_hits;
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_sal_dev_ambient_light_config_t;

typedef struct cfn_sal_dev_ambient_light_s     cfn_sal_dev_ambient_light_t;
typedef struct cfn_sal_dev_ambient_light_api_s cfn_sal_dev_ambient_light_api_t;

typedef void (*cfn_sal_dev_ambient_light_callback_t)(cfn_sal_dev_ambient_light_t *driver,
                                                     uint32_t                     event,
                                                     uint32_t                     error,
                                                     void                        *user_arg);

/**
 * @brief Light Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_dev_ambient_light_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_lux)(cfn_sal_dev_ambient_light_t *driver, float *lux_out);
    cfn_hal_error_code_t (*read_raw)(cfn_sal_dev_ambient_light_t *driver, uint32_t *raw_out);
    cfn_hal_error_code_t (*read_channels)(cfn_sal_dev_ambient_light_t *driver, uint16_t *ch1, uint16_t *ch2);

    /* Feature Data Retrieval */
    cfn_hal_error_code_t (*get_status)(cfn_sal_dev_ambient_light_t *driver, uint32_t *status_flags);
    cfn_hal_error_code_t (*get_id)(cfn_sal_dev_ambient_light_t *driver, uint32_t *id_out);

    void (*handle_interrupt)(cfn_sal_dev_ambient_light_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_dev_ambient_light_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_ambient_light,
                           cfn_sal_dev_ambient_light_config_t,
                           cfn_sal_dev_ambient_light_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_ambient_light_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_ambient_light_populate(cfn_sal_dev_ambient_light_t              *driver,
                                                       uint32_t                                  peripheral_id,
                                                       void                                     *dependency,
                                                       const cfn_sal_dev_ambient_light_api_t    *api,
                                                       const cfn_sal_phy_t                      *phy,
                                                       const cfn_sal_dev_ambient_light_config_t *config,
                                                       cfn_sal_dev_ambient_light_callback_t      callback,
                                                       void                                     *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_ambient_light_construct(cfn_sal_dev_ambient_light_t              *driver,
                                                         const cfn_sal_dev_ambient_light_config_t *config,
                                                         const cfn_sal_phy_t                      *phy,
                                                         void                                     *dependency,
                                                         cfn_sal_dev_ambient_light_callback_t      callback,
                                                         void                                     *user_arg);
cfn_hal_error_code_t cfn_sal_dev_ambient_light_destruct(cfn_sal_dev_ambient_light_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_init(cfn_sal_dev_ambient_light_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_deinit(cfn_sal_dev_ambient_light_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_config_set(
    cfn_sal_dev_ambient_light_t *driver, const cfn_sal_dev_ambient_light_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_config_get(cfn_sal_dev_ambient_light_t        *driver,
                                                                         cfn_sal_dev_ambient_light_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_callback_register(
    cfn_sal_dev_ambient_light_t *driver, cfn_sal_dev_ambient_light_callback_t callback, void *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_power_state_set(cfn_sal_dev_ambient_light_t *driver,
                                                                              cfn_hal_power_state_t        state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_event_enable(cfn_sal_dev_ambient_light_t *driver,
                                                                           uint32_t                     event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_event_disable(cfn_sal_dev_ambient_light_t *driver,
                                                                            uint32_t                     event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_event_get(cfn_sal_dev_ambient_light_t *driver,
                                                                        uint32_t                    *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_error_enable(cfn_sal_dev_ambient_light_t *driver,
                                                                           uint32_t                     error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_error_disable(cfn_sal_dev_ambient_light_t *driver,
                                                                            uint32_t                     error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_error_get(cfn_sal_dev_ambient_light_t *driver,
                                                                        uint32_t                    *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_read_lux(cfn_sal_dev_ambient_light_t *driver,
                                                                       float                       *lux_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, read_lux, driver, error, lux_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_read_raw(cfn_sal_dev_ambient_light_t *driver,
                                                                       uint32_t                    *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, read_raw, driver, error, raw_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_read_channels(cfn_sal_dev_ambient_light_t *driver,
                                                                            uint16_t                    *ch1,
                                                                            uint16_t                    *ch2)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, read_channels, driver, error, ch1, ch2);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_get_status(cfn_sal_dev_ambient_light_t *driver,
                                                                         uint32_t                    *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, get_status, driver, error, status_flags);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_ambient_light_get_id(cfn_sal_dev_ambient_light_t *driver,
                                                                     uint32_t                    *id_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_AMBIENT_LIGHT, get_id, driver, error, id_out);
    return error;
}

CFN_HAL_INLINE void cfn_sal_dev_ambient_light_handle_interrupt(cfn_sal_dev_ambient_light_t *driver)
{
    if (driver && driver->base.type == CFN_SAL_DEV_TYPE_AMBIENT_LIGHT && driver->api)
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

#endif /* CAFFEINE_SAL_DEV_AMBIENT_LIGHT_H */