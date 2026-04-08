/**
 * @file cfn_sal_kv.h
 * @brief Abstract Key-Value (KV) storage service interface.
 */

#ifndef CAFFEINE_SAL_UTILITIES_CFN_SAL_KV_H
#define CAFFEINE_SAL_UTILITIES_CFN_SAL_KV_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_KV CFN_SAL_TYPE('K', 'V', 'S')

/* Types Enums ------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *partition_name;
    void       *custom;
} cfn_sal_kv_config_t;

typedef struct cfn_sal_kv_s     cfn_sal_kv_t;
typedef struct cfn_sal_kv_api_s cfn_sal_kv_api_t;

typedef void (*cfn_sal_kv_callback_t)(cfn_sal_kv_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Key-Value Store Virtual Method Table (VMT).
 */
struct cfn_sal_kv_api_s
{
    cfn_hal_api_base_t base;

    /* KV Operations */
    cfn_hal_error_code_t (*set)(cfn_sal_kv_t *driver, const char *key, const void *val, size_t len);
    cfn_hal_error_code_t (*get)(
        cfn_sal_kv_t *driver, const char *key, void *val_buf, size_t max_len, size_t *actual_len);
    cfn_hal_error_code_t (*del)(cfn_sal_kv_t *driver, const char *key);
    cfn_hal_error_code_t (*clear)(cfn_sal_kv_t *driver);
    cfn_hal_error_code_t (*get_info)(cfn_sal_kv_t *driver, size_t *used_bytes, size_t *total_bytes);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_kv_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_kv, cfn_sal_kv_config_t, cfn_sal_kv_api_t, cfn_sal_phy_t, cfn_sal_kv_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_kv_populate(cfn_sal_kv_t              *driver,
                                        uint32_t                   peripheral_id,
                                        const cfn_sal_kv_api_t    *api,
                                        const cfn_sal_phy_t       *phy,
                                        const cfn_sal_kv_config_t *config,
                                        cfn_sal_kv_callback_t      callback,
                                        void                      *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SAL_TYPE_KV, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api         = api;
    driver->phy         = phy;
    driver->config      = config;
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_sal_kv_construct(cfn_sal_kv_t              *driver,
                                          const cfn_sal_kv_config_t *config,
                                          const cfn_sal_phy_t       *phy,
                                          cfn_sal_kv_callback_t      callback,
                                          void                      *user_arg);
cfn_hal_error_code_t cfn_sal_kv_destruct(cfn_sal_kv_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_kv_init(cfn_sal_kv_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_KV);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_kv_deinit(cfn_sal_kv_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_KV);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_kv_set(cfn_sal_kv_t *driver, const char *key, const void *val, size_t len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_KV, set, driver, error, key, val, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_sal_kv_get(cfn_sal_kv_t *driver, const char *key, void *val_buf, size_t max_len, size_t *actual_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_KV, get, driver, error, key, val_buf, max_len, actual_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_kv_del(cfn_sal_kv_t *driver, const char *key)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_KV, del, driver, error, key);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_UTILITIES_CFN_SAL_KV_H */
