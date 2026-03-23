/**
 * @file cfn_svc_fs.h
 * @brief Abstract File System service interface.
 */

#ifndef CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_FS_H
#define CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_FS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_FS CFN_SVC_TYPE('F', 'S', 'S')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_FS_MODE_READ = CFN_HAL_BIT(0),
    CFN_SVC_FS_MODE_WRITE = CFN_HAL_BIT(1),
    CFN_SVC_FS_MODE_APPEND = CFN_HAL_BIT(2),
    CFN_SVC_FS_MODE_CREATE = CFN_HAL_BIT(3),
} cfn_svc_fs_mode_t;

/* Types Structs ----------------------------------------------------*/

typedef void *cfn_svc_fs_file_t;

typedef struct
{
    const char *mount_point;
    void       *custom;
} cfn_svc_fs_config_t;

typedef struct cfn_svc_fs_s     cfn_svc_fs_t;
typedef struct cfn_svc_fs_api_s cfn_svc_fs_api_t;

typedef void (*cfn_svc_fs_callback_t)(cfn_svc_fs_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief File System Virtual Method Table (VMT).
 */
struct cfn_svc_fs_api_s
{
    cfn_hal_api_base_t base;

    /* FS Operations */
    cfn_hal_error_code_t (*mount)(cfn_svc_fs_t *driver);
    cfn_hal_error_code_t (*unmount)(cfn_svc_fs_t *driver);
    cfn_hal_error_code_t (*format)(cfn_svc_fs_t *driver);

    /* File Operations */
    cfn_hal_error_code_t (*open)(cfn_svc_fs_t *driver, cfn_svc_fs_file_t *file, const char *path, uint32_t mode);
    cfn_hal_error_code_t (*close)(cfn_svc_fs_t *driver, cfn_svc_fs_file_t file);
    cfn_hal_error_code_t (*read)(
        cfn_svc_fs_t *driver, cfn_svc_fs_file_t file, uint8_t *buffer, size_t len, size_t *read_len);
    cfn_hal_error_code_t (*write)(
        cfn_svc_fs_t *driver, cfn_svc_fs_file_t file, const uint8_t *data, size_t len, size_t *written_len);
    cfn_hal_error_code_t (*seek)(cfn_svc_fs_t *driver, cfn_svc_fs_file_t file, size_t offset);
    cfn_hal_error_code_t (*remove)(cfn_svc_fs_t *driver, const char *path);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_fs_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_fs, cfn_svc_fs_config_t, cfn_svc_fs_api_t, cfn_svc_phy_t, cfn_svc_fs_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_fs_populate(cfn_svc_fs_t              *driver,
                                        uint32_t                   peripheral_id,
                                        const cfn_svc_fs_api_t    *api,
                                        const cfn_svc_phy_t       *phy,
                                        const cfn_svc_fs_config_t *config,
                                        cfn_svc_fs_callback_t      callback,
                                        void                      *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_FS, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_fs_construct(cfn_svc_fs_t              *driver,
                                          const cfn_svc_fs_config_t *config,
                                          const cfn_svc_phy_t       *phy,
                                          cfn_svc_fs_callback_t      callback,
                                          void                      *user_arg);
cfn_hal_error_code_t cfn_svc_fs_destruct(cfn_svc_fs_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_init(cfn_svc_fs_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_FS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_deinit(cfn_svc_fs_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_FS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_config_set(cfn_svc_fs_t *driver, const cfn_svc_fs_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_FS, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_config_get(cfn_svc_fs_t *driver, cfn_svc_fs_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_callback_register(cfn_svc_fs_t         *driver,
                                                                 cfn_svc_fs_callback_t callback,
                                                                 void                 *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_FS, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_power_state_set(cfn_svc_fs_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_FS, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_event_enable(cfn_svc_fs_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_FS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_event_disable(cfn_svc_fs_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_FS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_event_get(cfn_svc_fs_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_FS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_error_enable(cfn_svc_fs_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_FS, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_error_disable(cfn_svc_fs_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_FS, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_error_get(cfn_svc_fs_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_FS, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_mount(cfn_svc_fs_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_FS, mount, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_unmount(cfn_svc_fs_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SVC_TYPE_FS, unmount, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_open(cfn_svc_fs_t      *driver,
                                                    cfn_svc_fs_file_t *file,
                                                    const char        *path,
                                                    uint32_t           mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_FS, open, driver, error, file, path, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_fs_close(cfn_svc_fs_t *driver, cfn_svc_fs_file_t file)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_FS, close, driver, error, file);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_svc_fs_read(cfn_svc_fs_t *driver, cfn_svc_fs_file_t file, uint8_t *buffer, size_t len, size_t *read_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_FS, read, driver, error, file, buffer, len, read_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_svc_fs_write(cfn_svc_fs_t *driver, cfn_svc_fs_file_t file, const uint8_t *data, size_t len, size_t *written_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_FS, write, driver, error, file, data, len, written_len);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_SOFTWARE_CFN_SVC_FS_H */
