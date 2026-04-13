/**
 * @file cfn_sal_dev_gnss.h
 * @brief Abstract GNSS (GPS/Glonass/Galileo) service interface.
 */

#ifndef CAFFEINE_SAL_DEV_GNSS_H
#define CAFFEINE_SAL_DEV_GNSS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <time.h>
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_GNSS CFN_SAL_TYPE('G', 'N', 'S')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_GNSS_EVENT_NONE         = 0,
    CFN_SAL_DEV_GNSS_EVENT_FIX_ACQUIRED = CFN_HAL_BIT(0),
    CFN_SAL_DEV_GNSS_EVENT_FIX_LOST     = CFN_HAL_BIT(1),
    CFN_SAL_DEV_GNSS_EVENT_DATA_READY   = CFN_HAL_BIT(2),
} cfn_sal_dev_gnss_event_t;

typedef enum
{
    CFN_SAL_DEV_GNSS_FIX_NONE = 0,
    CFN_SAL_DEV_GNSS_FIX_2D,
    CFN_SAL_DEV_GNSS_FIX_3D,
    CFN_SAL_DEV_GNSS_FIX_DGPS,
    CFN_SAL_DEV_GNSS_FIX_RTK_FIXED,
    CFN_SAL_DEV_GNSS_FIX_RTK_FLOAT
} cfn_sal_dev_gnss_fix_type_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    double                      latitude;
    double                      longitude;
    float                       altitude_m;
    float                       speed_kmh;
    float                       course_deg;
    float                       hdop;
    cfn_sal_dev_gnss_fix_type_t fix_type;
    uint8_t                     satellites_used;
} cfn_sal_dev_gnss_location_t;

typedef struct
{
    uint32_t fix_rate_ms;
    uint32_t baudrate; /* If UART based */
    void    *custom;
} cfn_sal_dev_gnss_config_t;

typedef struct cfn_sal_dev_gnss_s     cfn_sal_dev_gnss_t;
typedef struct cfn_sal_dev_gnss_api_s cfn_sal_dev_gnss_api_t;

typedef void (*cfn_sal_dev_gnss_callback_t)(cfn_sal_dev_gnss_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief GNSS Virtual Method Table (VMT).
 */
struct cfn_sal_dev_gnss_api_s
{
    cfn_hal_api_base_t base;

    /* Power & Control */
    cfn_hal_error_code_t (*power_on)(cfn_sal_dev_gnss_t *driver);
    cfn_hal_error_code_t (*power_off)(cfn_sal_dev_gnss_t *driver);

    /* Data Retrieval */
    cfn_hal_error_code_t (*get_location)(cfn_sal_dev_gnss_t *driver, cfn_sal_dev_gnss_location_t *loc_out);
    cfn_hal_error_code_t (*get_time)(cfn_sal_dev_gnss_t *driver, struct tm *time_out);
    cfn_hal_error_code_t (*get_time_utc)(cfn_sal_dev_gnss_t *driver, uint32_t *unix_timestamp);
    cfn_hal_error_code_t (*get_satellites_in_view)(cfn_sal_dev_gnss_t *driver, uint8_t *count_out);
    cfn_hal_error_code_t (*get_hdop)(cfn_sal_dev_gnss_t *driver, float *hdop_out);
    cfn_hal_error_code_t (*get_speed_knots)(cfn_sal_dev_gnss_t *driver, float *speed_out);
    cfn_hal_error_code_t (*get_heading_degrees)(cfn_sal_dev_gnss_t *driver, float *heading_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_fix_rate)(cfn_sal_dev_gnss_t *driver, uint32_t rate_ms);
    cfn_hal_error_code_t (*set_power_mode)(cfn_sal_dev_gnss_t *driver, uint8_t mode);

    /* Maintenance */
    cfn_hal_error_code_t (*cold_start)(cfn_sal_dev_gnss_t *driver);
    cfn_hal_error_code_t (*warm_start)(cfn_sal_dev_gnss_t *driver);
    cfn_hal_error_code_t (*hot_start)(cfn_sal_dev_gnss_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_dev_gnss_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_dev_gnss, cfn_sal_dev_gnss_config_t, cfn_sal_dev_gnss_api_t, cfn_sal_phy_t, cfn_sal_dev_gnss_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_gnss_populate(cfn_sal_dev_gnss_t              *driver,
                                              uint32_t                         peripheral_id,
                                              void                            *dependency,
                                              const cfn_sal_dev_gnss_api_t    *api,
                                              const cfn_sal_phy_t             *phy,
                                              const cfn_sal_dev_gnss_config_t *config,
                                              cfn_sal_dev_gnss_callback_t      callback,
                                              void                            *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_GNSS, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_gnss_construct(cfn_sal_dev_gnss_t              *driver,
                                                const cfn_sal_dev_gnss_config_t *config,
                                                const cfn_sal_phy_t             *phy,
                                                void                            *dependency,
                                                cfn_sal_dev_gnss_callback_t      callback,
                                                void                            *user_arg);
cfn_hal_error_code_t cfn_sal_dev_gnss_destruct(cfn_sal_dev_gnss_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_init(cfn_sal_dev_gnss_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_GNSS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_deinit(cfn_sal_dev_gnss_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_GNSS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_config_set(cfn_sal_dev_gnss_t              *driver,
                                                                const cfn_sal_dev_gnss_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_GNSS, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_config_get(cfn_sal_dev_gnss_t        *driver,
                                                                cfn_sal_dev_gnss_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_callback_register(cfn_sal_dev_gnss_t         *driver,
                                                                       cfn_sal_dev_gnss_callback_t callback,
                                                                       void                       *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_GNSS, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_power_state_set(cfn_sal_dev_gnss_t   *driver,
                                                                     cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_GNSS, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_event_enable(cfn_sal_dev_gnss_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_GNSS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_event_disable(cfn_sal_dev_gnss_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_GNSS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_event_get(cfn_sal_dev_gnss_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_GNSS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_error_enable(cfn_sal_dev_gnss_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_GNSS, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_error_disable(cfn_sal_dev_gnss_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_GNSS, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_error_get(cfn_sal_dev_gnss_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_GNSS, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_power_on(cfn_sal_dev_gnss_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_GNSS, power_on, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_power_off(cfn_sal_dev_gnss_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_GNSS, power_off, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_location(cfn_sal_dev_gnss_t          *driver,
                                                                  cfn_sal_dev_gnss_location_t *loc_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_location, driver, error, loc_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_time(cfn_sal_dev_gnss_t *driver, struct tm *time_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_time, driver, error, time_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_satellites_in_view(cfn_sal_dev_gnss_t *driver,
                                                                            uint8_t            *count_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_satellites_in_view, driver, error, count_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_set_fix_rate(cfn_sal_dev_gnss_t *driver, uint32_t rate_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, set_fix_rate, driver, error, rate_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_cold_start(cfn_sal_dev_gnss_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_GNSS, cold_start, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_warm_start(cfn_sal_dev_gnss_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_GNSS, warm_start, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_hot_start(cfn_sal_dev_gnss_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_DEV_TYPE_GNSS, hot_start, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_hdop(cfn_sal_dev_gnss_t *driver, float *hdop_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_hdop, driver, error, hdop_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_time_utc(cfn_sal_dev_gnss_t *driver, uint32_t *unix_timestamp)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_time_utc, driver, error, unix_timestamp);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_speed_knots(cfn_sal_dev_gnss_t *driver, float *speed_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_speed_knots, driver, error, speed_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_get_heading_degrees(cfn_sal_dev_gnss_t *driver, float *heading_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, get_heading_degrees, driver, error, heading_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gnss_set_power_mode(cfn_sal_dev_gnss_t *driver, uint8_t mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GNSS, set_power_mode, driver, error, mode);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_GNSS_H */
