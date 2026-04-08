/**
 * @file cfn_sal_analog_sensor.h
 * @brief Generic Analog/ADC Sensor service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_ANALOG_SENSOR_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_ANALOG_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_ANALOG_SENSOR CFN_SAL_TYPE('A', 'N', 'A')

/* Types Enums ------------------------------------------------------*/

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    float scale;  /*!< Multiplication factor (e.g., for voltage dividers) */
    float offset; /*!< Addition factor */
    void *custom;
} cfn_sal_analog_config_t;

typedef struct cfn_sal_analog_sensor_s     cfn_sal_analog_sensor_t;
typedef struct cfn_sal_analog_sensor_api_s cfn_sal_analog_sensor_api_t;

typedef void (*cfn_sal_analog_callback_t)(cfn_sal_analog_sensor_t *driver,
                                          uint32_t                 event,
                                          uint32_t                 error,
                                          void                    *user_arg);

/**
 * @brief Analog Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_analog_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_value)(cfn_sal_analog_sensor_t *driver, float *val_out);
    cfn_hal_error_code_t (*read_raw)(cfn_sal_analog_sensor_t *driver, uint32_t *raw_out);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_analog_sensor_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(
    sal_analog_sensor, cfn_sal_analog_config_t, cfn_sal_analog_sensor_api_t, cfn_sal_phy_t, cfn_sal_analog_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_analog_sensor_populate(cfn_sal_analog_sensor_t           *driver,
                                                   uint32_t                           peripheral_id,
                                                   const cfn_sal_analog_sensor_api_t *api,
                                                   const cfn_sal_phy_t               *phy,
                                                   const cfn_sal_analog_config_t     *config,
                                                   cfn_sal_analog_callback_t          callback,
                                                   void                              *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SAL_TYPE_ANALOG_SENSOR, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api         = api;
    driver->phy         = phy;
    driver->config      = config;
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_sal_analog_sensor_construct(cfn_sal_analog_sensor_t       *driver,
                                                     const cfn_sal_analog_config_t *config,
                                                     const cfn_sal_phy_t           *phy,
                                                     cfn_sal_analog_callback_t      callback,
                                                     void                          *user_arg);
cfn_hal_error_code_t cfn_sal_analog_sensor_destruct(cfn_sal_analog_sensor_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_analog_sensor_init(cfn_sal_analog_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_ANALOG_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_analog_sensor_deinit(cfn_sal_analog_sensor_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_ANALOG_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_analog_sensor_read_value(cfn_sal_analog_sensor_t *driver, float *val_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_ANALOG_SENSOR, read_value, driver, error, val_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_analog_sensor_read_raw(cfn_sal_analog_sensor_t *driver, uint32_t *raw_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_ANALOG_SENSOR, read_raw, driver, error, raw_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_ANALOG_SENSOR_H */
