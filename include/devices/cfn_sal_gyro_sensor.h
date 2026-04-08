/**
 * @file cfn_sal_gyro_sensor.h
 * @brief Abstract Gyroscope service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_GYRO_SENSOR_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_GYRO_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_GYRO_SENSOR CFN_SAL_TYPE('G', 'Y', 'R')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_GYRO_EVENT_NONE       = 0,
    CFN_SAL_GYRO_EVENT_DATA_READY = CFN_HAL_BIT(0),
    CFN_SAL_GYRO_EVENT_WAKEUP     = CFN_HAL_BIT(1),
} cfn_sal_gyro_event_t;

typedef enum
{
    CFN_SAL_GYRO_MODE_NORMAL,
    CFN_SAL_GYRO_MODE_LOW_POWER,
    CFN_SAL_GYRO_MODE_STANDBY,
    CFN_SAL_GYRO_MODE_POWER_DOWN,
} cfn_sal_gyro_mode_t;

typedef enum
{
    CFN_SAL_GYRO_RANGE_125DPS,
    CFN_SAL_GYRO_RANGE_250DPS,
    CFN_SAL_GYRO_RANGE_500DPS,
    CFN_SAL_GYRO_RANGE_1000DPS,
    CFN_SAL_GYRO_RANGE_2000DPS,
} cfn_sal_gyro_range_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} cfn_sal_gyro_data_t;

typedef struct
{
    uint32_t             sampling_rate_hz;
    cfn_sal_gyro_range_t range;
    void                *custom;
} cfn_sal_gyro_config_t;

typedef struct cfn_sal_gyro_sensor_s     cfn_sal_gyro_sensor_t;
typedef struct cfn_sal_gyro_sensor_api_s cfn_sal_gyro_sensor_api_t;

typedef void (*cfn_sal_gyro_callback_t)(cfn_sal_gyro_sensor_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Gyroscope Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_gyro_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_xyz_mdps)(cfn_sal_gyro_sensor_t *driver, cfn_sal_gyro_data_t *data_out);
    cfn_hal_error_code_t (*read_xyz_raw)(cfn_sal_gyro_sensor_t *driver, cfn_sal_gyro_data_t *data_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_range)(cfn_sal_gyro_sensor_t *driver, cfn_sal_gyro_range_t range);
    cfn_hal_error_code_t (*get_range)(cfn_sal_gyro_sensor_t *driver, cfn_sal_gyro_range_t *range_out);
    cfn_hal_error_code_t (*set_datarate)(cfn_sal_gyro_sensor_t *driver, uint32_t hz);
    cfn_hal_error_code_t (*get_datarate)(cfn_sal_gyro_sensor_t *driver, uint32_t *hz_out);

    /* Feature Control */
    cfn_hal_error_code_t (*enable_interrupts)(cfn_sal_gyro_sensor_t *driver, uint32_t event_mask);
    cfn_hal_error_code_t (*set_mode)(cfn_sal_gyro_sensor_t *driver, cfn_sal_gyro_mode_t mode);
    cfn_hal_error_code_t (*config_fifo)(cfn_sal_gyro_sensor_t *driver, uint8_t watermark, uint8_t mode);
    cfn_hal_error_code_t (*read_fifo)(cfn_sal_gyro_sensor_t *driver, cfn_sal_gyro_data_t *data, size_t *count);
    cfn_hal_error_code_t (*get_status)(cfn_sal_gyro_sensor_t *driver, uint32_t *status_flags);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_gyro_sensor_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_gyro_sensor, cfn_sal_gyro_config_t, cfn_sal_gyro_sensor_api_t, cfn_sal_phy_t, cfn_sal_gyro_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_gyro_sensor_populate(cfn_sal_gyro_sensor_t           *driver,
                                                 uint32_t                         peripheral_id,
                                                 const cfn_sal_gyro_sensor_api_t *api,
                                                 const cfn_sal_phy_t             *phy,
                                                 const cfn_sal_gyro_config_t     *config,
                                                 cfn_sal_gyro_callback_t          callback,
                                                 void                            *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api         = api;
    driver->phy         = phy;
    driver->config      = config;
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_sal_gyro_sensor_construct(cfn_sal_gyro_sensor_t       *driver,
                                                   const cfn_sal_gyro_config_t *config,
                                                   const cfn_sal_phy_t         *phy,
                                                   cfn_sal_gyro_callback_t      callback,
                                                   void                        *user_arg);
cfn_hal_error_code_t cfn_sal_gyro_sensor_destruct(cfn_sal_gyro_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_init(cfn_sal_gyro_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_deinit(cfn_sal_gyro_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_config_set(cfn_sal_gyro_sensor_t       *driver,
                                                                   const cfn_sal_gyro_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_config_get(cfn_sal_gyro_sensor_t *driver,
                                                                   cfn_sal_gyro_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_callback_register(cfn_sal_gyro_sensor_t  *driver,
                                                                          cfn_sal_gyro_callback_t callback,
                                                                          void                   *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_TYPE_GYRO_SENSOR, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_power_state_set(cfn_sal_gyro_sensor_t *driver,
                                                                        cfn_hal_power_state_t  state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_event_enable(cfn_sal_gyro_sensor_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_event_disable(cfn_sal_gyro_sensor_t *driver,
                                                                      uint32_t               event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_event_get(cfn_sal_gyro_sensor_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_error_enable(cfn_sal_gyro_sensor_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_error_disable(cfn_sal_gyro_sensor_t *driver,
                                                                      uint32_t               error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_error_get(cfn_sal_gyro_sensor_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_GYRO_SENSOR, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_read_xyz_mdps(cfn_sal_gyro_sensor_t *driver,
                                                                      cfn_sal_gyro_data_t   *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, read_xyz_mdps, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_read_xyz_raw(cfn_sal_gyro_sensor_t *driver,
                                                                     cfn_sal_gyro_data_t   *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, read_xyz_raw, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_set_range(cfn_sal_gyro_sensor_t *driver,
                                                                  cfn_sal_gyro_range_t   range)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, set_range, driver, error, range);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_get_range(cfn_sal_gyro_sensor_t *driver,
                                                                  cfn_sal_gyro_range_t  *range_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, get_range, driver, error, range_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_set_datarate(cfn_sal_gyro_sensor_t *driver, uint32_t hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, set_datarate, driver, error, hz);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_get_datarate(cfn_sal_gyro_sensor_t *driver, uint32_t *hz_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, get_datarate, driver, error, hz_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_enable_interrupts(cfn_sal_gyro_sensor_t *driver,
                                                                          uint32_t               event_mask)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, enable_interrupts, driver, error, event_mask);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_set_mode(cfn_sal_gyro_sensor_t *driver,
                                                                 cfn_sal_gyro_mode_t    mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, set_mode, driver, error, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_config_fifo(cfn_sal_gyro_sensor_t *driver,
                                                                    uint8_t                watermark,
                                                                    uint8_t                mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, config_fifo, driver, error, watermark, mode);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_read_fifo(cfn_sal_gyro_sensor_t *driver,
                                                                  cfn_sal_gyro_data_t   *data,
                                                                  size_t                *count)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, read_fifo, driver, error, data, count);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gyro_sensor_get_status(cfn_sal_gyro_sensor_t *driver,
                                                                   uint32_t              *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GYRO_SENSOR, get_status, driver, error, status_flags);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_GYRO_SENSOR_H */
