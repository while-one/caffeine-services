/**
 * @file cfn_sal_dev_pressure.h
 * @brief Abstract Barometric Pressure Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEV_PRESSURE_H
#define CAFFEINE_SAL_DEV_PRESSURE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_PRESSURE CFN_SAL_TYPE('B', 'A', 'R')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_PRESSURE_MODE_CONTINUOUS,
    CFN_SAL_DEV_PRESSURE_MODE_ONE_SHOT,
} cfn_sal_dev_pressure_mode_t;

typedef enum
{
    CFN_SAL_DEV_PRESSURE_EVENT_NONE       = 0,
    CFN_SAL_DEV_PRESSURE_EVENT_DATA_READY = CFN_HAL_BIT(0),
} cfn_sal_dev_pressure_event_t;

typedef enum
{
    CFN_SAL_DEV_PRESSURE_OVERSAMPLING_NONE,
    CFN_SAL_DEV_PRESSURE_OVERSAMPLING_1X,
    CFN_SAL_DEV_PRESSURE_OVERSAMPLING_2X,
    CFN_SAL_DEV_PRESSURE_OVERSAMPLING_4X,
    CFN_SAL_DEV_PRESSURE_OVERSAMPLING_8X,
    CFN_SAL_DEV_PRESSURE_OVERSAMPLING_16X,
} cfn_sal_dev_pressure_oversampling_t;

typedef enum
{
    CFN_SAL_DEV_PRESSURE_FILTER_NONE,
    CFN_SAL_DEV_PRESSURE_FILTER_COEFF_2,
    CFN_SAL_DEV_PRESSURE_FILTER_COEFF_4,
    CFN_SAL_DEV_PRESSURE_FILTER_COEFF_8,
    CFN_SAL_DEV_PRESSURE_FILTER_COEFF_16,
} cfn_sal_dev_pressure_filter_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    cfn_sal_dev_pressure_mode_t         mode;
    cfn_sal_dev_pressure_oversampling_t oversampling;
    cfn_sal_dev_pressure_filter_t       filter;
    uint32_t                            sampling_rate_hz;
    void                               *custom;
} cfn_sal_dev_pressure_config_t;

typedef struct cfn_sal_dev_pressure_s     cfn_sal_dev_pressure_t;
typedef struct cfn_sal_dev_pressure_api_s cfn_sal_dev_pressure_api_t;

typedef void (*cfn_sal_dev_pressure_callback_t)(cfn_sal_dev_pressure_t *driver,
                                                uint32_t                event,
                                                uint32_t                error,
                                                void                   *user_arg);

/**
 * @brief Pressure Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_dev_pressure_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_hpa)(cfn_sal_dev_pressure_t *driver, float *hpa_out);
    cfn_hal_error_code_t (*read_raw)(cfn_sal_dev_pressure_t *driver, int32_t *raw_out);

    /* Info */
    cfn_hal_error_code_t (*get_altitude_m)(cfn_sal_dev_pressure_t *driver, float sea_level_hpa, float *alt_out);

    /* Configuration */
    cfn_hal_error_code_t (*start_conversion)(cfn_sal_dev_pressure_t *driver);
    cfn_hal_error_code_t (*get_status)(cfn_sal_dev_pressure_t *driver, uint32_t *status_flags);
};

CFN_SAL_DEV_VMT_CHECK(struct cfn_sal_dev_pressure_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_pressure,
                           cfn_sal_dev_pressure_config_t,
                           cfn_sal_dev_pressure_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_pressure_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_pressure_populate(cfn_sal_dev_pressure_t              *driver,
                                                  uint32_t                             peripheral_id,
                                                  void                                *dependency,
                                                  const cfn_sal_dev_pressure_api_t    *api,
                                                  const cfn_sal_phy_t                 *phy,
                                                  const cfn_sal_dev_pressure_config_t *config,
                                                  cfn_sal_dev_pressure_callback_t      callback,
                                                  void                                *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_PRESSURE, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_pressure_construct(cfn_sal_dev_pressure_t              *driver,
                                                    const cfn_sal_dev_pressure_config_t *config,
                                                    const cfn_sal_phy_t                 *phy,
                                                    void                                *dependency,
                                                    cfn_sal_dev_pressure_callback_t      callback,
                                                    void                                *user_arg);
cfn_hal_error_code_t cfn_sal_dev_pressure_destruct(cfn_sal_dev_pressure_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_init(cfn_sal_dev_pressure_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_deinit(cfn_sal_dev_pressure_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_config_set(cfn_sal_dev_pressure_t              *driver,
                                                                    const cfn_sal_dev_pressure_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_config_get(cfn_sal_dev_pressure_t        *driver,
                                                                    cfn_sal_dev_pressure_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_callback_register(cfn_sal_dev_pressure_t         *driver,
                                                                           cfn_sal_dev_pressure_callback_t callback,
                                                                           void                           *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_PRESSURE, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_power_state_set(cfn_sal_dev_pressure_t *driver,
                                                                         cfn_hal_power_state_t   state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_event_enable(cfn_sal_dev_pressure_t *driver,
                                                                      uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_event_disable(cfn_sal_dev_pressure_t *driver,
                                                                       uint32_t                event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_event_get(cfn_sal_dev_pressure_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_error_enable(cfn_sal_dev_pressure_t *driver,
                                                                      uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_error_disable(cfn_sal_dev_pressure_t *driver,
                                                                       uint32_t                error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_error_get(cfn_sal_dev_pressure_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_PRESSURE, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_read_hpa(cfn_sal_dev_pressure_t *driver, float *hpa_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_PRESSURE, read_hpa, driver, error, hpa_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_read_raw(cfn_sal_dev_pressure_t *driver, int32_t *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_PRESSURE, read_raw, driver, error, raw_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_get_altitude(cfn_sal_dev_pressure_t *driver,
                                                                      float                   sea_level_hpa,
                                                                      float                  *alt_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_PRESSURE, get_altitude_m, driver, error, sea_level_hpa, alt_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_start_conversion(cfn_sal_dev_pressure_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_PRESSURE, start_conversion, driver, error);
    return error;
}
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_get_status(cfn_sal_dev_pressure_t *driver,
                                                                    uint32_t               *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_PRESSURE, get_status, driver, error, status_flags);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_get_id(cfn_sal_dev_pressure_t *driver, uint32_t *id_out)
{
    return cfn_sal_dev_get_id((void *) driver, id_out);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_pressure_handle_interrupt(cfn_sal_dev_pressure_t *driver)
{
    return cfn_sal_dev_handle_interrupt((void *) driver);
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_PRESSURE_H */