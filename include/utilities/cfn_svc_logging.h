/**
 * @file cfn_svc_logging.h
 * @brief Abstract Logging service interface.
 */

#ifndef CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_LOGGING_H
#define CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_LOGGING_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <stdarg.h>
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_LOGGING CFN_SVC_TYPE('L', 'O', 'G')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_LOG_LEVEL_DEBUG = 0,
    CFN_SVC_LOG_LEVEL_INFO,
    CFN_SVC_LOG_LEVEL_WARN,
    CFN_SVC_LOG_LEVEL_ERROR,
    CFN_SVC_LOG_LEVEL_FATAL
} cfn_svc_log_level_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    cfn_svc_log_level_t min_level;
    bool                use_timestamp;
    void               *custom;
} cfn_svc_logging_config_t;

typedef struct cfn_svc_logging_s     cfn_svc_logging_t;
typedef struct cfn_svc_logging_api_s cfn_svc_logging_api_t;

typedef void (*cfn_svc_logging_callback_t)(cfn_svc_logging_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Logging Virtual Method Table (VMT).
 */
struct cfn_svc_logging_api_s
{
    cfn_hal_api_base_t base;

    /* Logging Operations */
    cfn_hal_error_code_t (*log)(cfn_svc_logging_t *driver, cfn_svc_log_level_t level, const char *fmt, va_list args);
    cfn_hal_error_code_t (*flush)(cfn_svc_logging_t *driver);

    /* Configuration */
    cfn_hal_error_code_t (*set_level)(cfn_svc_logging_t *driver, cfn_svc_log_level_t level);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_logging_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_logging, cfn_svc_logging_config_t, cfn_svc_logging_api_t, cfn_svc_phy_t, cfn_svc_logging_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_logging_populate(cfn_svc_logging_t              *driver,
                                             uint32_t                        peripheral_id,
                                             const cfn_svc_logging_api_t    *api,
                                             const cfn_svc_phy_t            *phy,
                                             const cfn_svc_logging_config_t *config,
                                             cfn_svc_logging_callback_t      callback,
                                             void                           *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_LOGGING, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_logging_construct(cfn_svc_logging_t              *driver,
                                               const cfn_svc_logging_config_t *config,
                                               const cfn_svc_phy_t            *phy,
                                               cfn_svc_logging_callback_t      callback,
                                               void                           *user_arg);
cfn_hal_error_code_t cfn_svc_logging_destruct(cfn_svc_logging_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_init(cfn_svc_logging_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_LOGGING);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_deinit(cfn_svc_logging_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_LOGGING);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_config_set(cfn_svc_logging_t              *driver,
                                                               const cfn_svc_logging_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_LOGGING, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_config_get(cfn_svc_logging_t        *driver,
                                                               cfn_svc_logging_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_callback_register(cfn_svc_logging_t         *driver,
                                                                      cfn_svc_logging_callback_t callback,
                                                                      void                      *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_LOGGING, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_power_state_set(cfn_svc_logging_t    *driver,
                                                                    cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_LOGGING, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_event_enable(cfn_svc_logging_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_LOGGING, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_event_disable(cfn_svc_logging_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_LOGGING, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_event_get(cfn_svc_logging_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_LOGGING, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_error_enable(cfn_svc_logging_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_LOGGING, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_error_disable(cfn_svc_logging_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_LOGGING, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_logging_error_get(cfn_svc_logging_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_LOGGING, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_log(cfn_svc_logging_t  *driver,
                                                cfn_svc_log_level_t level,
                                                const char         *fmt,
                                                ...)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    va_list              args;
    va_start(args, fmt);
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_LOGGING, log, driver, error, level, fmt, args);
    va_end(args);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_log_flush(cfn_svc_logging_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_LOGGING, flush, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_LOGGING_H */
