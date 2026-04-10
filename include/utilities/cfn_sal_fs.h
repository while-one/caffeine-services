/**
 * @file cfn_sal_fs.h
 * @brief Abstract File System service interface.
 */

#ifndef CAFFEINE_SAL_SOFTWARE_CFN_SAL_FS_H
#define CAFFEINE_SAL_SOFTWARE_CFN_SAL_FS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_FS CFN_SAL_TYPE('F', 'S', 'S')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_FS_MODE_READ   = CFN_HAL_BIT(0),
    CFN_SAL_FS_MODE_WRITE  = CFN_HAL_BIT(1),
    CFN_SAL_FS_MODE_APPEND = CFN_HAL_BIT(2),
    CFN_SAL_FS_MODE_CREATE = CFN_HAL_BIT(3),
} cfn_sal_fs_mode_t;

typedef enum
{
    CFN_SAL_FS_SEEK_SET, /*!< Seek from the start of the file */
    CFN_SAL_FS_SEEK_CUR, /*!< Seek from the current position */
    CFN_SAL_FS_SEEK_END, /*!< Seek from the end of the file */
} cfn_sal_fs_seek_origin_t;

/* Types Structs ----------------------------------------------------*/

typedef void *cfn_sal_fs_file_t;

typedef struct
{
    const char *mount_point;
    void       *custom;
} cfn_sal_fs_config_t;

typedef struct cfn_sal_fs_s     cfn_sal_fs_t;
typedef struct cfn_sal_fs_api_s cfn_sal_fs_api_t;

typedef void (*cfn_sal_fs_callback_t)(cfn_sal_fs_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief File System Virtual Method Table (VMT).
 */
struct cfn_sal_fs_api_s
{
    cfn_hal_api_base_t base;

    /* FS Operations */
    cfn_hal_error_code_t (*mount)(cfn_sal_fs_t *driver);
    cfn_hal_error_code_t (*unmount)(cfn_sal_fs_t *driver);
    cfn_hal_error_code_t (*format)(cfn_sal_fs_t *driver);

    /* File Operations */
    cfn_hal_error_code_t (*open)(cfn_sal_fs_t *driver, cfn_sal_fs_file_t *file, const char *path, uint32_t mode);
    cfn_hal_error_code_t (*close)(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file);
    cfn_hal_error_code_t (*read)(
        cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, uint8_t *buffer, size_t len, size_t *read_len);
    cfn_hal_error_code_t (*write)(
        cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, const uint8_t *data, size_t len, size_t *written_len);
    cfn_hal_error_code_t (*seek)(cfn_sal_fs_t            *driver,
                                 cfn_sal_fs_file_t        file,
                                 uint32_t                 offset,
                                 cfn_sal_fs_seek_origin_t origin);
    cfn_hal_error_code_t (*tell)(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, uint32_t *offset_out);
    cfn_hal_error_code_t (*eof)(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, bool *is_eof);
    cfn_hal_error_code_t (*remove)(cfn_sal_fs_t *driver, const char *path);
    cfn_hal_error_code_t (*rename)(cfn_sal_fs_t *driver, const char *old_path, const char *new_path);
    cfn_hal_error_code_t (*mkdir)(cfn_sal_fs_t *driver, const char *path);
    cfn_hal_error_code_t (*rmdir)(cfn_sal_fs_t *driver, const char *path);
    cfn_hal_error_code_t (*stat)(cfn_sal_fs_t *driver, const char *path, void *stat_out);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_fs_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_fs, cfn_sal_fs_config_t, cfn_sal_fs_api_t, cfn_sal_phy_t, cfn_sal_fs_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_fs_populate(cfn_sal_fs_t              *driver,
                                        uint32_t                   peripheral_id,
                                        void                      *dependency,
                                        const cfn_sal_fs_api_t    *api,
                                        const cfn_sal_phy_t       *phy,
                                        const cfn_sal_fs_config_t *config,
                                        cfn_sal_fs_callback_t      callback,
                                        void                      *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_FS, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_fs_construct(cfn_sal_fs_t              *driver,
                                          const cfn_sal_fs_config_t *config,
                                          const cfn_sal_phy_t       *phy,
                                          void                      *dependency,
                                          cfn_sal_fs_callback_t      callback,
                                          void                      *user_arg);
cfn_hal_error_code_t cfn_sal_fs_destruct(cfn_sal_fs_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_init(cfn_sal_fs_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_FS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_deinit(cfn_sal_fs_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_FS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_config_set(cfn_sal_fs_t *driver, const cfn_sal_fs_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_FS, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_config_get(cfn_sal_fs_t *driver, cfn_sal_fs_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_callback_register(cfn_sal_fs_t         *driver,
                                                                 cfn_sal_fs_callback_t callback,
                                                                 void                 *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SAL_TYPE_FS, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_power_state_set(cfn_sal_fs_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_FS, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_event_enable(cfn_sal_fs_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_FS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_event_disable(cfn_sal_fs_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_FS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_event_get(cfn_sal_fs_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_FS, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_error_enable(cfn_sal_fs_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_FS, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_error_disable(cfn_sal_fs_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_FS, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_error_get(cfn_sal_fs_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_FS, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_mount(cfn_sal_fs_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_FS, mount, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_unmount(cfn_sal_fs_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_FS, unmount, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_open(cfn_sal_fs_t      *driver,
                                                    cfn_sal_fs_file_t *file,
                                                    const char        *path,
                                                    uint32_t           mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, open, driver, error, file, path, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_close(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, close, driver, error, file);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_sal_fs_read(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, uint8_t *buffer, size_t len, size_t *read_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, read, driver, error, file, buffer, len, read_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_sal_fs_write(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, const uint8_t *data, size_t len, size_t *written_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, write, driver, error, file, data, len, written_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_format(cfn_sal_fs_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_FS, format, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_seek(cfn_sal_fs_t            *driver,
                                                    cfn_sal_fs_file_t        file,
                                                    uint32_t                 offset,
                                                    cfn_sal_fs_seek_origin_t origin)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, seek, driver, error, file, offset, origin);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_tell(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, uint32_t *offset_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, tell, driver, error, file, offset_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_eof(cfn_sal_fs_t *driver, cfn_sal_fs_file_t file, bool *is_eof)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, eof, driver, error, file, is_eof);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_remove(cfn_sal_fs_t *driver, const char *path)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, remove, driver, error, path);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_rename(cfn_sal_fs_t *driver, const char *old_path, const char *new_path)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, rename, driver, error, old_path, new_path);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_mkdir(cfn_sal_fs_t *driver, const char *path)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, mkdir, driver, error, path);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_rmdir(cfn_sal_fs_t *driver, const char *path)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, rmdir, driver, error, path);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_fs_stat(cfn_sal_fs_t *driver, const char *path, void *stat_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_FS, stat, driver, error, path, stat_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_SOFTWARE_CFN_SAL_FS_H */
