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
#include "cfn_svc_types.h"

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
    void *instance; /*!< Mapping to I2C/SPI driver */
    void *user_arg;
} cfn_svc_accel_phy_t;

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
};

CFN_HAL_VMT_CHECK(struct cfn_svc_accel_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_accel, cfn_svc_accel_config_t, cfn_svc_accel_api_t, cfn_svc_accel_phy_t, cfn_svc_accel_callback_t);

#define CFN_SVC_ACCEL_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                      \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_ACCEL, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_init(cfn_svc_accel_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_ACCEL);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_accel_read_mg(cfn_svc_accel_t *driver, cfn_svc_accel_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_ACCEL, read_xyz_mg, driver, error, data_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_ACCEL_H */
