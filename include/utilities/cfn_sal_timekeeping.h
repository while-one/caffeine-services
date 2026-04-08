/**
 * @file cfn_sal_timekeeping.h
 * @brief High-level timekeeping and epoch management service.
 */

#ifndef CAFFEINE_SAL_UTILITIES_CFN_SAL_TIMEKEEPING_H
#define CAFFEINE_SAL_UTILITIES_CFN_SAL_TIMEKEEPING_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"
#include <time.h>

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_TIMEKEEPING CFN_SAL_TYPE('T', 'I', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_TIMEKEEPING_EVENT_NONE        = 0,
    CFN_SAL_TIMEKEEPING_EVENT_SYNC_OK     = CFN_HAL_BIT(0),
    CFN_SAL_TIMEKEEPING_EVENT_SYNC_FAILED = CFN_HAL_BIT(1),
} cfn_sal_timekeeping_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t sync_interval_s; /*!< Automatic synchronization interval */
    bool     use_rtc_fallback; /*!< Fallback to hardware RTC if network sync fails */
    void    *custom;
} cfn_sal_timekeeping_config_t;

typedef struct cfn_sal_timekeeping_s     cfn_sal_timekeeping_t;
typedef struct cfn_sal_timekeeping_api_s cfn_sal_timekeeping_api_t;

typedef void (*cfn_sal_timekeeping_callback_t)(
    cfn_sal_timekeeping_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Timekeeping Virtual Method Table (VMT).
 */
struct cfn_sal_timekeeping_api_s
{
    cfn_hal_api_base_t base;

    /* Time Operations */
    cfn_hal_error_code_t (*set_time)(cfn_sal_timekeeping_t *driver, time_t timestamp);
    cfn_hal_error_code_t (*get_time)(cfn_sal_timekeeping_t *driver, time_t *timestamp_out);
    cfn_hal_error_code_t (*get_ms)(cfn_sal_timekeeping_t *driver, uint64_t *ms_out);

    /* Synchronization */
    cfn_hal_error_code_t (*sync_now)(cfn_sal_timekeeping_t *driver);
    cfn_hal_error_code_t (*is_synchronized)(cfn_sal_timekeeping_t *driver, bool *is_sync_out);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_timekeeping_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_timekeeping,
                           cfn_sal_timekeeping_config_t,
                           cfn_sal_timekeeping_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_timekeeping_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_timekeeping_populate(cfn_sal_timekeeping_t              *driver,
                                                 uint32_t                           peripheral_id,
                                                 const cfn_sal_timekeeping_api_t    *api,
                                                 const cfn_sal_phy_t               *phy,
                                                 const cfn_sal_timekeeping_config_t *config,
                                                 cfn_sal_timekeeping_callback_t      callback,
                                                 void                              *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SAL_TYPE_TIMEKEEPING, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api         = api;
    driver->phy         = phy;
    driver->config      = config;
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_sal_timekeeping_construct(cfn_sal_timekeeping_t              *driver,
                                                   const cfn_sal_timekeeping_config_t *config,
                                                   const cfn_sal_phy_t               *phy,
                                                   cfn_sal_timekeeping_callback_t      callback,
                                                   void                              *user_arg);
cfn_hal_error_code_t cfn_sal_timekeeping_destruct(cfn_sal_timekeeping_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_init(cfn_sal_timekeeping_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_TIMEKEEPING);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_deinit(cfn_sal_timekeeping_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_TIMEKEEPING);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_set_time(cfn_sal_timekeeping_t *driver, time_t timestamp)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_TIMEKEEPING, set_time, driver, error, timestamp);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_get_time(cfn_sal_timekeeping_t *driver, time_t *timestamp_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_TIMEKEEPING, get_time, driver, error, timestamp_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_get_ms(cfn_sal_timekeeping_t *driver, uint64_t *ms_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_TIMEKEEPING, get_ms, driver, error, ms_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_sync_now(cfn_sal_timekeeping_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_TIMEKEEPING, sync_now, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_timekeeping_is_synchronized(cfn_sal_timekeeping_t *driver, bool *is_sync_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_TIMEKEEPING, is_synchronized, driver, error, is_sync_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_UTILITIES_CFN_SAL_TIMEKEEPING_H */
