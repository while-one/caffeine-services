/**
 * @file cfn_sal_utl_kv.h
 * @brief Abstract Key-Value (KV) storage service interface.
 */

#ifndef CAFFEINE_SAL_UTL_KV_H
#define CAFFEINE_SAL_UTL_KV_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_UTL_TYPE_KEY_VALUE CFN_SAL_TYPE('K', 'V', 'S')

/* Types Enums ------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *partition_name;
    void       *custom;
} cfn_sal_utl_key_value_config_t;

typedef struct cfn_sal_utl_key_value_s     cfn_sal_utl_key_value_t;
typedef struct cfn_sal_utl_key_value_api_s cfn_sal_utl_key_value_api_t;

typedef void (*cfn_sal_utl_key_value_callback_t)(cfn_sal_utl_key_value_t *driver,
                                                 uint32_t                 event,
                                                 uint32_t                 error,
                                                 void                    *user_arg);

/**
 * @brief Key-Value Store Virtual Method Table (VMT).
 */
struct cfn_sal_utl_key_value_api_s
{
    cfn_hal_api_base_t base;

    /* KV Operations */
    cfn_hal_error_code_t (*set)(cfn_sal_utl_key_value_t *driver, const char *key, const void *val, size_t len);
    cfn_hal_error_code_t (*get)(
        cfn_sal_utl_key_value_t *driver, const char *key, void *val_buf, size_t max_len, size_t *actual_len);
    cfn_hal_error_code_t (*del)(cfn_sal_utl_key_value_t *driver, const char *key);
    cfn_hal_error_code_t (*clear)(cfn_sal_utl_key_value_t *driver);
    cfn_hal_error_code_t (*get_info)(cfn_sal_utl_key_value_t *driver, size_t *used_bytes, size_t *total_bytes);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_utl_key_value_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_utl_key_value,
                           cfn_sal_utl_key_value_config_t,
                           cfn_sal_utl_key_value_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_utl_key_value_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_utl_key_value_populate(cfn_sal_utl_key_value_t              *driver,
                                                   uint32_t                              peripheral_id,
                                                   void                                 *dependency,
                                                   const cfn_sal_utl_key_value_api_t    *api,
                                                   const cfn_sal_phy_t                  *phy,
                                                   const cfn_sal_utl_key_value_config_t *config,
                                                   cfn_sal_utl_key_value_callback_t      callback,
                                                   void                                 *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_UTL_TYPE_KEY_VALUE, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_utl_key_value_construct(cfn_sal_utl_key_value_t              *driver,
                                                     const cfn_sal_utl_key_value_config_t *config,
                                                     const cfn_sal_phy_t                  *phy,
                                                     void                                 *dependency,
                                                     cfn_sal_utl_key_value_callback_t      callback,
                                                     void                                 *user_arg);
cfn_hal_error_code_t cfn_sal_utl_key_value_destruct(cfn_sal_utl_key_value_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_init(cfn_sal_utl_key_value_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_deinit(cfn_sal_utl_key_value_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_config_set(cfn_sal_utl_key_value_t              *driver,
                                                                     const cfn_sal_utl_key_value_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_config_get(cfn_sal_utl_key_value_t        *driver,
                                                                     cfn_sal_utl_key_value_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_callback_register(cfn_sal_utl_key_value_t         *driver,
                                                                            cfn_sal_utl_key_value_callback_t callback,
                                                                            void                            *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_power_state_set(cfn_sal_utl_key_value_t *driver,
                                                                          cfn_hal_power_state_t    state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_event_enable(cfn_sal_utl_key_value_t *driver,
                                                                       uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_event_disable(cfn_sal_utl_key_value_t *driver,
                                                                        uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_event_get(cfn_sal_utl_key_value_t *driver,
                                                                    uint32_t                *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_error_enable(cfn_sal_utl_key_value_t *driver,
                                                                       uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_error_disable(cfn_sal_utl_key_value_t *driver,
                                                                        uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_error_get(cfn_sal_utl_key_value_t *driver,
                                                                    uint32_t                *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_UTL_TYPE_KEY_VALUE, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_set(cfn_sal_utl_key_value_t *driver,
                                                              const char              *key,
                                                              const void              *val,
                                                              size_t                   len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_KEY_VALUE, set, driver, error, key, val, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_get(
    cfn_sal_utl_key_value_t *driver, const char *key, void *val_buf, size_t max_len, size_t *actual_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_KEY_VALUE, get, driver, error, key, val_buf, max_len, actual_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_del(cfn_sal_utl_key_value_t *driver, const char *key)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_KEY_VALUE, del, driver, error, key);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_clear(cfn_sal_utl_key_value_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_UTL_TYPE_KEY_VALUE, clear, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_utl_key_value_get_info(cfn_sal_utl_key_value_t *driver,
                                                                   size_t                  *used_bytes,
                                                                   size_t                  *total_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_UTL_TYPE_KEY_VALUE, get_info, driver, error, used_bytes, total_bytes);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_UTL_KV_H */
