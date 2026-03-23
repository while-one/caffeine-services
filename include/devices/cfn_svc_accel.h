/**
 * @file cfn_svc_accel.h
 * @brief Abstract Accelerometer service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_ACCEL_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_ACCEL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_ACCEL CFN_SVC_TYPE('A', 'C', 'L')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_ACCEL_EVENT_NONE = 0,
    CFN_SVC_ACCEL_EVENT_DATA_READY = CFN_HAL_BIT(0),
    CFN_SVC_ACCEL_EVENT_FREEFALL = CFN_HAL_BIT(1),
    CFN_SVC_ACCEL_EVENT_WAKEUP = CFN_HAL_BIT(2),
    CFN_SVC_ACCEL_EVENT_SINGLE_TAP = CFN_HAL_BIT(3),
    CFN_SVC_ACCEL_EVENT_DOUBLE_TAP = CFN_HAL_BIT(4),
} cfn_svc_accel_event_t;

typedef enum
{
    CFN_SVC_ACCEL_MODE_NORMAL,
    CFN_SVC_ACCEL_MODE_LOW_POWER,
    CFN_SVC_ACCEL_MODE_STANDBY,
} cfn_svc_accel_mode_t;

typedef enum
{
    CFN_SVC_ACCEL_RANGE_2G,
    CFN_SVC_ACCEL_RANGE_4G,
    CFN_SVC_ACCEL_RANGE_8G,
    CFN_SVC_ACCEL_RANGE_16G,
} cfn_svc_accel_range_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} cfn_svc_accel_data_t;

typedef struct
{
    uint32_t              sampling_rate_hz;
    cfn_svc_accel_range_t range;
    void                 *custom;
} cfn_svc_accel_config_t;

typedef struct cfn_svc_accel_s     cfn_svc_accel_t;
typedef struct cfn_svc_accel_api_s cfn_svc_accel_api_t;

typedef void (*cfn_svc_accel_callback_t)(cfn_svc_accel_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Accelerometer Virtual Method Table (VMT).
 */
struct cfn_svc_accel_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_xyz_mg)(cfn_svc_accel_t *driver, cfn_svc_accel_data_t *data_out);
    cfn_hal_error_code_t (*read_xyz_raw)(cfn_svc_accel_t *driver, cfn_svc_accel_data_t *data_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_range)(cfn_svc_accel_t *driver, cfn_svc_accel_range_t range);
    cfn_hal_error_code_t (*get_range)(cfn_svc_accel_t *driver, cfn_svc_accel_range_t *range_out);
    cfn_hal_error_code_t (*set_datarate)(cfn_svc_accel_t *driver, uint32_t hz);
    cfn_hal_error_code_t (*get_datarate)(cfn_svc_accel_t *driver, uint32_t *hz_out);

    /* Feature Control */
    cfn_hal_error_code_t (*enable_interrupts)(cfn_svc_accel_t *driver, uint32_t event_mask);
    cfn_hal_error_code_t (*set_mode)(cfn_svc_accel_t *driver, cfn_svc_accel_mode_t mode);
    cfn_hal_error_code_t (*config_fifo)(cfn_svc_accel_t *driver, uint8_t watermark, uint8_t mode);
    cfn_hal_error_code_t (*read_fifo)(cfn_svc_accel_t *driver, cfn_svc_accel_data_t *data, size_t *count);
    cfn_hal_error_code_t (*config_tap_detection)(cfn_svc_accel_t *driver, bool double_tap, uint8_t threshold);
    cfn_hal_error_code_t (*config_freefall)(cfn_svc_accel_t *driver, uint16_t threshold_mg, uint16_t time_ms);
    cfn_hal_error_code_t (*get_status)(cfn_svc_accel_t *driver, uint32_t *status_flags);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_accel_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(
    svc_accel, cfn_svc_accel_config_t, cfn_svc_accel_api_t, cfn_svc_phy_t, cfn_svc_accel_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_accel_populate(cfn_svc_accel_t              *driver,
                                           uint32_t                      peripheral_id,
                                           const cfn_svc_accel_api_t    *api,
                                           const cfn_svc_phy_t          *phy,
                                           const cfn_svc_accel_config_t *config,
                                           cfn_svc_accel_callback_t      callback,
                                           void                         *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_ACCEL, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_accel_construct(cfn_svc_accel_t              *driver,
                                             const cfn_svc_accel_config_t *config,
                                             const cfn_svc_phy_t          *phy,
                                             cfn_svc_accel_callback_t      callback,
                                             void                         *user_arg);
cfn_hal_error_code_t cfn_svc_accel_destruct(cfn_svc_accel_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_init(cfn_svc_accel_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_ACCEL);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_deinit(cfn_svc_accel_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SVC_TYPE_ACCEL);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_config_set(cfn_svc_accel_t              *driver,
                                                             const cfn_svc_accel_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SVC_TYPE_ACCEL, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_config_get(cfn_svc_accel_t *driver, cfn_svc_accel_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_callback_register(cfn_svc_accel_t         *driver,
                                                                    cfn_svc_accel_callback_t callback,
                                                                    void                    *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SVC_TYPE_ACCEL, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_power_state_set(cfn_svc_accel_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SVC_TYPE_ACCEL, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_event_enable(cfn_svc_accel_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SVC_TYPE_ACCEL, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_event_disable(cfn_svc_accel_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SVC_TYPE_ACCEL, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_event_get(cfn_svc_accel_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SVC_TYPE_ACCEL, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_error_enable(cfn_svc_accel_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SVC_TYPE_ACCEL, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_error_disable(cfn_svc_accel_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SVC_TYPE_ACCEL, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_error_get(cfn_svc_accel_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SVC_TYPE_ACCEL, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_read_mg(cfn_svc_accel_t *driver, cfn_svc_accel_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, read_xyz_mg, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_read_raw(cfn_svc_accel_t *driver, cfn_svc_accel_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, read_xyz_raw, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_set_range(cfn_svc_accel_t *driver, cfn_svc_accel_range_t range)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, set_range, driver, error, range);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_get_range(cfn_svc_accel_t *driver, cfn_svc_accel_range_t *range_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, get_range, driver, error, range_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_set_datarate(cfn_svc_accel_t *driver, uint32_t hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, set_datarate, driver, error, hz);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_get_datarate(cfn_svc_accel_t *driver, uint32_t *hz_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, get_datarate, driver, error, hz_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_enable_interrupts(cfn_svc_accel_t *driver, uint32_t event_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, enable_interrupts, driver, error, event_mask);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_set_mode(cfn_svc_accel_t *driver, cfn_svc_accel_mode_t mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, set_mode, driver, error, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_config_fifo(cfn_svc_accel_t *driver, uint8_t watermark, uint8_t mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, config_fifo, driver, error, watermark, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_read_fifo(cfn_svc_accel_t      *driver,
                                                           cfn_svc_accel_data_t *data,
                                                           size_t               *count)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, read_fifo, driver, error, data, count);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_config_tap_detection(cfn_svc_accel_t *driver,
                                                                      bool              double_tap,
                                                                      uint8_t           threshold)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, config_tap_detection, driver, error, double_tap, threshold);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_config_freefall(cfn_svc_accel_t *driver,
                                                                 uint16_t          threshold_mg,
                                                                 uint16_t          time_ms)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, config_freefall, driver, error, threshold_mg, time_ms);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_get_status(cfn_svc_accel_t *driver, uint32_t *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, get_status, driver, error, status_flags);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_ACCEL_H */
