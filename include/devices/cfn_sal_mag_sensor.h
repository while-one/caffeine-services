/**
 * @file cfn_sal_mag_sensor.h
 * @brief Abstract Magnetometer Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_MAG_SENSOR_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_MAG_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_MAG_SENSOR CFN_SAL_TYPE('M', 'A', 'G')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_MAG_RANGE_4G,  /*!< ±4 Gauss */
    CFN_SAL_MAG_RANGE_8G,  /*!< ±8 Gauss */
    CFN_SAL_MAG_RANGE_12G, /*!< ±12 Gauss */
    CFN_SAL_MAG_RANGE_16G, /*!< ±16 Gauss */

    CFN_SAL_MAG_RANGE_MAX
} cfn_sal_mag_range_t;

typedef enum
{
    CFN_SAL_MAG_MODE_POWER_DOWN,
    CFN_SAL_MAG_MODE_CONTINUOUS,
    CFN_SAL_MAG_MODE_ONE_SHOT,
} cfn_sal_mag_mode_t;

typedef enum
{
    CFN_SAL_MAG_EVENT_NONE       = 0,
    CFN_SAL_MAG_EVENT_DATA_READY = CFN_HAL_BIT(0),
} cfn_sal_mag_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    int32_t x_ugauss; /*!< Micro-gauss */
    int32_t y_ugauss;
    int32_t z_ugauss;
} cfn_sal_mag_data_t;

typedef struct
{
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_sal_mag_config_t;

typedef struct cfn_sal_mag_sensor_s     cfn_sal_mag_sensor_t;
typedef struct cfn_sal_mag_sensor_api_s cfn_sal_mag_sensor_api_t;

typedef void (*cfn_sal_mag_callback_t)(cfn_sal_mag_sensor_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Magnetometer Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_mag_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_xyz_ugauss)(cfn_sal_mag_sensor_t *driver, cfn_sal_mag_data_t *data_out);
    cfn_hal_error_code_t (*read_xyz_raw)(cfn_sal_mag_sensor_t *driver, int16_t *x, int16_t *y, int16_t *z);

    /* Configuration */
    cfn_hal_error_code_t (*set_range)(cfn_sal_mag_sensor_t *driver, cfn_sal_mag_range_t range);
    cfn_hal_error_code_t (*get_range)(cfn_sal_mag_sensor_t *driver, cfn_sal_mag_range_t *range_out);
    cfn_hal_error_code_t (*set_mode)(cfn_sal_mag_sensor_t *driver, cfn_sal_mag_mode_t mode);
    cfn_hal_error_code_t (*set_datarate)(cfn_sal_mag_sensor_t *driver, uint32_t hz);
    cfn_hal_error_code_t (*soft_reset)(cfn_sal_mag_sensor_t *driver);
    cfn_hal_error_code_t (*get_status)(cfn_sal_mag_sensor_t *driver, uint32_t *status_flags);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_mag_sensor_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_mag_sensor, cfn_sal_mag_config_t, cfn_sal_mag_sensor_api_t, cfn_sal_phy_t, cfn_sal_mag_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_mag_sensor_populate(cfn_sal_mag_sensor_t           *driver,
                                                uint32_t                        peripheral_id,
                                                void                           *dependency,
                                                const cfn_sal_mag_sensor_api_t *api,
                                                const cfn_sal_phy_t            *phy,
                                                const cfn_sal_mag_config_t     *config,
                                                cfn_sal_mag_callback_t          callback,
                                                void                           *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_MAG_SENSOR, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_mag_sensor_construct(cfn_sal_mag_sensor_t       *driver,
                                                  const cfn_sal_mag_config_t *config,
                                                  const cfn_sal_phy_t        *phy,
                                                  void                       *dependency,
                                                  cfn_sal_mag_callback_t      callback,
                                                  void                       *user_arg);
cfn_hal_error_code_t cfn_sal_mag_sensor_destruct(cfn_sal_mag_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_mag_sensor_init(cfn_sal_mag_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_MAG_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_mag_sensor_deinit(cfn_sal_mag_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_MAG_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_mag_sensor_read_xyz_ugauss(cfn_sal_mag_sensor_t *driver,
                                                                       cfn_sal_mag_data_t   *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_MAG_SENSOR, read_xyz_ugauss, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_mag_sensor_set_range(cfn_sal_mag_sensor_t *driver,
                                                                 cfn_sal_mag_range_t   range)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_MAG_SENSOR, set_range, driver, error, range);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_mag_sensor_set_mode(cfn_sal_mag_sensor_t *driver, cfn_sal_mag_mode_t mode)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_MAG_SENSOR, set_mode, driver, error, mode);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_MAG_SENSOR_H */
