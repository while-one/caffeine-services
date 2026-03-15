/**
 * @file cfn_svc_hum_sensor.h
 * @brief Abstract Humidity Sensor service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_HUM_SENSOR_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_HUM_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_HUM_SENSOR CFN_SVC_TYPE('H', 'U', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_HUM_EVENT_NONE = 0,
    CFN_SVC_HUM_EVENT_DATA_READY = CFN_HAL_BIT(0),
} cfn_svc_hum_event_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *instance; /*!< Mapping to I2C/SPI driver */
    void *user_arg;
} cfn_svc_hum_phy_t;

typedef struct
{
    uint32_t sampling_rate_hz;
    void    *custom;
} cfn_svc_hum_config_t;

typedef struct cfn_svc_hum_sensor_s     cfn_svc_hum_sensor_t;
typedef struct cfn_svc_hum_sensor_api_s cfn_svc_hum_sensor_api_t;

typedef void (*cfn_svc_hum_callback_t)(cfn_svc_hum_sensor_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief Humidity Sensor Virtual Method Table (VMT).
 */
struct cfn_svc_hum_sensor_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_relative_humidity)(cfn_svc_hum_sensor_t *driver, float *hum_out);
    cfn_hal_error_code_t (*read_raw)(cfn_svc_hum_sensor_t *driver, int32_t *raw_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_resolution)(cfn_svc_hum_sensor_t *driver, uint8_t bits);
    cfn_hal_error_code_t (*get_resolution)(cfn_svc_hum_sensor_t *driver, uint8_t *bits_out);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_hum_sensor_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_hum_sensor, cfn_svc_hum_config_t, cfn_svc_hum_sensor_api_t, cfn_svc_hum_phy_t, cfn_svc_hum_callback_t);

#define CFN_SVC_HUM_SENSOR_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_HUM_SENSOR, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_init(cfn_svc_hum_sensor_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_HUM_SENSOR);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_hum_sensor_read_rh(cfn_svc_hum_sensor_t *driver, float *hum_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_HUM_SENSOR, read_relative_humidity, driver, error, hum_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_HUM_SENSOR_H */
