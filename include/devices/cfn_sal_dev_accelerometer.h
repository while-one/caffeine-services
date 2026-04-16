/**
 * @file cfn_sal_dev_accelerometer.h
 * @brief Abstract Accelerometer service interface.
 */

#ifndef CAFFEINE_SAL_DEV_ACCELEROMETER_H
#define CAFFEINE_SAL_DEV_ACCELEROMETER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_ACCELEROMETER CFN_SAL_TYPE('A', 'C', 'L')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_EVENT_NONE       = 0,
    CFN_SAL_DEV_ACCELEROMETER_EVENT_DATA_READY = CFN_HAL_BIT(0),
    CFN_SAL_DEV_ACCELEROMETER_EVENT_FREEFALL   = CFN_HAL_BIT(1),
    CFN_SAL_DEV_ACCELEROMETER_EVENT_WAKEUP     = CFN_HAL_BIT(2),
    CFN_SAL_DEV_ACCELEROMETER_EVENT_SINGLE_TAP = CFN_HAL_BIT(3),
    CFN_SAL_DEV_ACCELEROMETER_EVENT_DOUBLE_TAP = CFN_HAL_BIT(4),
} cfn_sal_dev_accelerometer_event_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_MODE_NORMAL,
    CFN_SAL_DEV_ACCELEROMETER_MODE_LOW_POWER,
    CFN_SAL_DEV_ACCELEROMETER_MODE_STANDBY,

    CFN_SAL_DEV_ACCELEROMETER_MODE_MAX
} cfn_sal_dev_accelerometer_mode_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_8_BIT,
    CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_10_BIT,
    CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_12_BIT,
    CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_14_BIT,
    CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_16_BIT,

    CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_MAX
} cfn_sal_dev_accelerometer_resolution_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_FILTER_NONE,
    CFN_SAL_DEV_ACCELEROMETER_FILTER_HIGH_PASS,
    CFN_SAL_DEV_ACCELEROMETER_FILTER_LOW_PASS,
    CFN_SAL_DEV_ACCELEROMETER_FILTER_BAND_PASS,

    CFN_SAL_DEV_ACCELEROMETER_FILTER_MAX
} cfn_sal_dev_accelerometer_filter_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_RANGE_2G,
    CFN_SAL_DEV_ACCELEROMETER_RANGE_4G,
    CFN_SAL_DEV_ACCELEROMETER_RANGE_8G,
    CFN_SAL_DEV_ACCELEROMETER_RANGE_16G,

    CFN_SAL_DEV_ACCELEROMETER_RANGE_MAX
} cfn_sal_dev_accelerometer_range_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_ODR_1_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_10_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_25_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_50_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_100_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_200_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_400_HZ,
    CFN_SAL_DEV_ACCELEROMETER_ODR_1000_HZ,

    CFN_SAL_DEV_ACCELEROMETER_ODR_MAX
} cfn_sal_dev_accelerometer_odr_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_BW_ODR_DIV_2,
    CFN_SAL_DEV_ACCELEROMETER_BW_ODR_DIV_4,

    CFN_SAL_DEV_ACCELEROMETER_BW_MAX
} cfn_sal_dev_accelerometer_bw_t;

typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_FIFO_MODE_BYPASS,
    CFN_SAL_DEV_ACCELEROMETER_FIFO_MODE_FIFO,
    CFN_SAL_DEV_ACCELEROMETER_FIFO_MODE_STREAM,
    CFN_SAL_DEV_ACCELEROMETER_FIFO_MODE_TRIGGER,

    CFN_SAL_DEV_ACCELEROMETER_FIFO_MODE_MAX
} cfn_sal_dev_accelerometer_fifo_mode_t;

// Which physical interrupt pin on the sensor to use
typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_INT_PIN_1,
    CFN_SAL_DEV_ACCELEROMETER_INT_PIN_2,

    CFN_SAL_DEV_ACCELEROMETER_INT_PIN_MAX
} cfn_sal_dev_accelerometer_int_pin_t;

// Electrical mode of the interrupt pin
typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_INT_MODE_PUSH_PULL,
    CFN_SAL_DEV_ACCELEROMETER_INT_MODE_OPEN_DRAIN,

    CFN_SAL_DEV_ACCELEROMETER_INT_MODE_MAX
} cfn_sal_dev_accelerometer_int_mode_t;

// Active level of the interrupt pin
typedef enum
{
    CFN_SAL_DEV_ACCELEROMETER_INT_LEVEL_ACTIVE_LOW,
    CFN_SAL_DEV_ACCELEROMETER_INT_LEVEL_ACTIVE_HIGH,

    CFN_SAL_DEV_ACCELEROMETER_INT_LEVEL_MAX
} cfn_sal_dev_accelerometer_int_level_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    cfn_sal_dev_accelerometer_int_mode_t  mode;
    cfn_sal_dev_accelerometer_int_level_t level;
    // Mask of cfn_sal_dev_accelerometer_event_t to route to this pin
    uint32_t event_mask;
} cfn_sal_dev_accelerometer_int_config_t;

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} cfn_sal_dev_accelerometer_data_t;

typedef struct
{
    bool    enable;
    bool    double_tap;
    uint8_t threshold;
} cfn_sal_dev_accelerometer_tap_config_t;

typedef struct
{
    bool     enable;
    uint16_t threshold_mg;
    uint16_t time_ms;
} cfn_sal_dev_accelerometer_freefall_config_t;

typedef struct
{
    cfn_sal_dev_accelerometer_mode_t            mode;
    cfn_sal_dev_accelerometer_resolution_t      resolution;
    cfn_sal_dev_accelerometer_odr_t             odr;
    cfn_sal_dev_accelerometer_bw_t              bandwidth;
    cfn_sal_dev_accelerometer_range_t           range;
    cfn_sal_dev_accelerometer_filter_t          filter;
    cfn_sal_dev_accelerometer_fifo_mode_t       fifo_mode;
    uint8_t                                     fifo_watermark;
    cfn_sal_dev_accelerometer_int_config_t      int1_config;
    cfn_sal_dev_accelerometer_int_config_t      int2_config;
    cfn_sal_dev_accelerometer_tap_config_t      tap_config;
    cfn_sal_dev_accelerometer_freefall_config_t freefall_config;
    bool                                        pedometer_enable;
    bool                                        significant_motion_enable;
    bool                                        orientation_6d_enable;
    void                                       *custom;
} cfn_sal_dev_accelerometer_config_t;

typedef struct cfn_sal_dev_accelerometer_s     cfn_sal_dev_accelerometer_t;
typedef struct cfn_sal_dev_accelerometer_api_s cfn_sal_dev_accelerometer_api_t;

typedef void (*cfn_sal_dev_accelerometer_callback_t)(cfn_sal_dev_accelerometer_t *driver,
                                                     uint32_t                     event,
                                                     uint32_t                     error,
                                                     void                        *user_arg);

/**
 * @brief Accelerometer Virtual Method Table (VMT).
 */
struct cfn_sal_dev_accelerometer_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_xyz_mg)(cfn_sal_dev_accelerometer_t      *driver,
                                        cfn_sal_dev_accelerometer_data_t *data_out);
    cfn_hal_error_code_t (*read_xyz_raw)(cfn_sal_dev_accelerometer_t      *driver,
                                         cfn_sal_dev_accelerometer_data_t *data_out);

    /* Feature Data Retrieval */
    cfn_hal_error_code_t (*read_fifo)(cfn_sal_dev_accelerometer_t      *driver,
                                      cfn_sal_dev_accelerometer_data_t *data,
                                      size_t                           *count);
    cfn_hal_error_code_t (*read_step_counter)(cfn_sal_dev_accelerometer_t *driver, uint32_t *steps);
    cfn_hal_error_code_t (*read_6d_orientation)(cfn_sal_dev_accelerometer_t *driver, uint8_t *orientation_code);
    cfn_hal_error_code_t (*get_status)(cfn_sal_dev_accelerometer_t *driver, uint32_t *status_flags);
};

CFN_SAL_DEV_VMT_CHECK(struct cfn_sal_dev_accelerometer_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_accelerometer,
                           cfn_sal_dev_accelerometer_config_t,
                           cfn_sal_dev_accelerometer_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_accelerometer_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_accelerometer_populate(cfn_sal_dev_accelerometer_t              *driver,
                                                       uint32_t                                  peripheral_id,
                                                       void                                     *dependency,
                                                       const cfn_sal_dev_accelerometer_api_t    *api,
                                                       const cfn_sal_phy_t                      *phy,
                                                       const cfn_sal_dev_accelerometer_config_t *config,
                                                       cfn_sal_dev_accelerometer_callback_t      callback,
                                                       void                                     *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_ACCELEROMETER, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_accelerometer_construct(cfn_sal_dev_accelerometer_t              *driver,
                                                         const cfn_sal_dev_accelerometer_config_t *config,
                                                         const cfn_sal_phy_t                      *phy,
                                                         void                                     *dependency,
                                                         cfn_sal_dev_accelerometer_callback_t      callback,
                                                         void                                     *user_arg);
cfn_hal_error_code_t cfn_sal_dev_accelerometer_destruct(cfn_sal_dev_accelerometer_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_config_validate(
    const cfn_sal_dev_accelerometer_t *driver, const cfn_sal_dev_accelerometer_config_t *config)
{
    if (!driver || !config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (config->mode >= CFN_SAL_DEV_ACCELEROMETER_MODE_MAX ||
        config->resolution >= CFN_SAL_DEV_ACCELEROMETER_RESOLUTION_MAX ||
        config->odr >= CFN_SAL_DEV_ACCELEROMETER_ODR_MAX ||
        config->bandwidth >= CFN_SAL_DEV_ACCELEROMETER_BW_MAX ||
        config->range >= CFN_SAL_DEV_ACCELEROMETER_RANGE_MAX ||
        config->fifo_mode >= CFN_SAL_DEV_ACCELEROMETER_FIFO_MODE_MAX ||
        config->int1_config.mode >= CFN_SAL_DEV_ACCELEROMETER_INT_MODE_MAX ||
        config->int1_config.level >= CFN_SAL_DEV_ACCELEROMETER_INT_LEVEL_MAX ||
        config->int2_config.mode >= CFN_SAL_DEV_ACCELEROMETER_INT_MODE_MAX ||
        config->int2_config.level >= CFN_SAL_DEV_ACCELEROMETER_INT_LEVEL_MAX)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_init(cfn_sal_dev_accelerometer_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_sal_dev_accelerometer_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_deinit(cfn_sal_dev_accelerometer_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_config_set(
    cfn_sal_dev_accelerometer_t *driver, const cfn_sal_dev_accelerometer_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_sal_dev_accelerometer_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_config_get(cfn_sal_dev_accelerometer_t        *driver,
                                                                         cfn_sal_dev_accelerometer_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_callback_register(
    cfn_sal_dev_accelerometer_t *driver, cfn_sal_dev_accelerometer_callback_t callback, void *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_power_state_set(cfn_sal_dev_accelerometer_t *driver,
                                                                              cfn_hal_power_state_t        state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_event_enable(cfn_sal_dev_accelerometer_t *driver,
                                                                           uint32_t                     event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_event_disable(cfn_sal_dev_accelerometer_t *driver,
                                                                            uint32_t                     event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_event_get(cfn_sal_dev_accelerometer_t *driver,
                                                                        uint32_t                    *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_error_enable(cfn_sal_dev_accelerometer_t *driver,
                                                                           uint32_t                     error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_error_disable(cfn_sal_dev_accelerometer_t *driver,
                                                                            uint32_t                     error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_error_get(cfn_sal_dev_accelerometer_t *driver,
                                                                        uint32_t                    *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_ACCELEROMETER, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_read_mg(cfn_sal_dev_accelerometer_t      *driver,
                                                                      cfn_sal_dev_accelerometer_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_ACCELEROMETER, read_xyz_mg, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_read_raw(cfn_sal_dev_accelerometer_t      *driver,
                                                                       cfn_sal_dev_accelerometer_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_ACCELEROMETER, read_xyz_raw, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_read_fifo(cfn_sal_dev_accelerometer_t      *driver,
                                                                        cfn_sal_dev_accelerometer_data_t *data,
                                                                        size_t                           *count)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_ACCELEROMETER, read_fifo, driver, error, data, count);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_get_status(cfn_sal_dev_accelerometer_t *driver,
                                                                         uint32_t                    *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_ACCELEROMETER, get_status, driver, error, status_flags);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_read_step_counter(cfn_sal_dev_accelerometer_t *driver,
                                                                                uint32_t                    *steps)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_ACCELEROMETER, read_step_counter, driver, error, steps);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_accelerometer_read_6d_orientation(cfn_sal_dev_accelerometer_t *driver,
                                                                                  uint8_t *orientation_code)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_DEV_TYPE_ACCELEROMETER, read_6d_orientation, driver, error, orientation_code);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_ACCELEROMETER_H */