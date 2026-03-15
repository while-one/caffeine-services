/**
 * @file cfn_svc_temp_sensor.h
 * @brief Abstract Temperature Sensor service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_TEMP_SENSOR_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_TEMP_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_TEMP_SENSOR CFN_SVC_TYPE('T', 'M', 'P')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_TEMP_EVENT_NONE = 0,
    CFN_SVC_TEMP_EVENT_DATA_READY = CFN_HAL_BIT(0),
    CFN_SVC_TEMP_EVENT_THRESHOLD_HIGH = CFN_HAL_BIT(1),
    CFN_SVC_TEMP_EVENT_THRESHOLD_LOW = CFN_HAL_BIT(2),
} cfn_svc_temp_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *instance; /*!< Mapping to I2C/SPI driver or ADC handle */
    void *user_arg;
} cfn_svc_temp_phy_t;

typedef struct
{
    uint32_t sampling_rate_hz;
    uint8_t  resolution_bits;
    void    *custom;
} cfn_svc_temp_config_t;

typedef struct cfn_svc_temp_sensor_s     cfn_svc_temp_sensor_t;
typedef struct cfn_svc_temp_sensor_api_s cfn_svc_temp_sensor_api_t;

typedef void (*cfn_svc_temp_callback_t)(cfn_svc_temp_sensor_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Temperature Sensor Virtual Method Table (VMT).
 */
struct cfn_svc_temp_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_celsius)(cfn_svc_temp_sensor_t *driver, float *temp_out);
    cfn_hal_error_code_t (*read_fahrenheit)(cfn_svc_temp_sensor_t *driver, float *temp_out);
    cfn_hal_error_code_t (*read_raw)(cfn_svc_temp_sensor_t *driver, int32_t *raw_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_resolution)(cfn_svc_temp_sensor_t *driver, uint8_t bits);
    cfn_hal_error_code_t (*get_resolution)(cfn_svc_temp_sensor_t *driver, uint8_t *bits_out);
    cfn_hal_error_code_t (*set_high_threshold)(cfn_svc_temp_sensor_t *driver, float celsius);
    cfn_hal_error_code_t (*set_low_threshold)(cfn_svc_temp_sensor_t *driver, float celsius);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_temp_sensor_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_temp_sensor, cfn_svc_temp_config_t, cfn_svc_temp_sensor_api_t, cfn_svc_temp_phy_t, cfn_svc_temp_callback_t);

#define CFN_SVC_TEMP_SENSOR_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_TEMP_SENSOR, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_temp_sensor_init(cfn_svc_temp_sensor_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_TEMP_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_temp_sensor_read_celsius(cfn_svc_temp_sensor_t *driver, float *temp_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_TEMP_SENSOR, read_celsius, driver, error, temp_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_TEMP_SENSOR_H */
