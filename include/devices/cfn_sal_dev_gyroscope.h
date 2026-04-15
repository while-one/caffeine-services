/**
 * @file cfn_sal_dev_gyroscope.h
 * @brief Abstract Gyroscope service interface.
 */

#ifndef CAFFEINE_SAL_DEV_GYROSCOPE_H
#define CAFFEINE_SAL_DEV_GYROSCOPE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_GYROSCOPE CFN_SAL_TYPE('G', 'Y', 'R')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_EVENT_NONE       = 0,
    CFN_SAL_DEV_GYROSCOPE_EVENT_DATA_READY = CFN_HAL_BIT(0),
    CFN_SAL_DEV_GYROSCOPE_EVENT_WAKEUP     = CFN_HAL_BIT(1),
} cfn_sal_dev_gyroscope_event_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_MODE_NORMAL,
    CFN_SAL_DEV_GYROSCOPE_MODE_LOW_POWER,
    CFN_SAL_DEV_GYROSCOPE_MODE_STANDBY,
    CFN_SAL_DEV_GYROSCOPE_MODE_POWER_DOWN,
} cfn_sal_dev_gyroscope_mode_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_RANGE_125DPS,
    CFN_SAL_DEV_GYROSCOPE_RANGE_250DPS,
    CFN_SAL_DEV_GYROSCOPE_RANGE_500DPS,
    CFN_SAL_DEV_GYROSCOPE_RANGE_1000DPS,
    CFN_SAL_DEV_GYROSCOPE_RANGE_2000DPS,
} cfn_sal_dev_gyroscope_range_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_ODR_1_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_10_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_25_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_50_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_100_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_200_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_400_HZ,
    CFN_SAL_DEV_GYROSCOPE_ODR_1000_HZ,
} cfn_sal_dev_gyroscope_odr_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_BW_ODR_DIV_2,
    CFN_SAL_DEV_GYROSCOPE_BW_ODR_DIV_4,
} cfn_sal_dev_gyroscope_bw_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_FIFO_MODE_BYPASS,
    CFN_SAL_DEV_GYROSCOPE_FIFO_MODE_FIFO,
    CFN_SAL_DEV_GYROSCOPE_FIFO_MODE_STREAM,
    CFN_SAL_DEV_GYROSCOPE_FIFO_MODE_TRIGGER,
} cfn_sal_dev_gyroscope_fifo_mode_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_INT_PIN_1,
    CFN_SAL_DEV_GYROSCOPE_INT_PIN_2,
} cfn_sal_dev_gyroscope_int_pin_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_INT_MODE_PUSH_PULL,
    CFN_SAL_DEV_GYROSCOPE_INT_MODE_OPEN_DRAIN,
} cfn_sal_dev_gyroscope_int_mode_t;

typedef enum
{
    CFN_SAL_DEV_GYROSCOPE_INT_LEVEL_ACTIVE_LOW,
    CFN_SAL_DEV_GYROSCOPE_INT_LEVEL_ACTIVE_HIGH,
} cfn_sal_dev_gyroscope_int_level_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    cfn_sal_dev_gyroscope_int_mode_t  mode;
    cfn_sal_dev_gyroscope_int_level_t level;
    uint32_t                          event_mask;
} cfn_sal_dev_gyroscope_int_config_t;

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} cfn_sal_dev_gyroscope_data_t;

typedef struct
{
    cfn_sal_dev_gyroscope_mode_t       mode;
    cfn_sal_dev_gyroscope_odr_t        odr;
    cfn_sal_dev_gyroscope_bw_t         bandwidth;
    cfn_sal_dev_gyroscope_range_t      range;
    cfn_sal_dev_gyroscope_fifo_mode_t  fifo_mode;
    uint8_t                            fifo_watermark;
    cfn_sal_dev_gyroscope_int_config_t int1_config;
    cfn_sal_dev_gyroscope_int_config_t int2_config;
    void                              *custom;
} cfn_sal_dev_gyroscope_config_t;

typedef struct cfn_sal_dev_gyroscope_s     cfn_sal_dev_gyroscope_t;
typedef struct cfn_sal_dev_gyroscope_api_s cfn_sal_dev_gyroscope_api_t;

typedef void (*cfn_sal_dev_gyroscope_callback_t)(cfn_sal_dev_gyroscope_t *driver,
                                                 uint32_t                 event,
                                                 uint32_t                 error,
                                                 void                    *user_arg);

/**
 * @brief Gyroscope Sensor Virtual Method Table (VMT).
 */
struct cfn_sal_dev_gyroscope_api_s
{
    cfn_hal_api_base_t base;
    cfn_sal_dev_api_t  dev;

    /* Measurement Operations */
    cfn_hal_error_code_t (*read_xyz_mdps)(cfn_sal_dev_gyroscope_t *driver, cfn_sal_dev_gyroscope_data_t *data_out);
    cfn_hal_error_code_t (*read_xyz_raw)(cfn_sal_dev_gyroscope_t *driver, cfn_sal_dev_gyroscope_data_t *data_out);

    /* Feature Data Retrieval */
    cfn_hal_error_code_t (*read_fifo)(cfn_sal_dev_gyroscope_t      *driver,
                                      cfn_sal_dev_gyroscope_data_t *data,
                                      size_t                       *count);
    cfn_hal_error_code_t (*get_status)(cfn_sal_dev_gyroscope_t *driver, uint32_t *status_flags);
};

CFN_SAL_DEV_VMT_CHECK(struct cfn_sal_dev_gyroscope_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_gyroscope,
                           cfn_sal_dev_gyroscope_config_t,
                           cfn_sal_dev_gyroscope_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_gyroscope_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_gyroscope_populate(cfn_sal_dev_gyroscope_t              *driver,
                                                   uint32_t                              peripheral_id,
                                                   void                                 *dependency,
                                                   const cfn_sal_dev_gyroscope_api_t    *api,
                                                   const cfn_sal_phy_t                  *phy,
                                                   const cfn_sal_dev_gyroscope_config_t *config,
                                                   cfn_sal_dev_gyroscope_callback_t      callback,
                                                   void                                 *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_GYROSCOPE, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_gyroscope_construct(cfn_sal_dev_gyroscope_t              *driver,
                                                     const cfn_sal_dev_gyroscope_config_t *config,
                                                     const cfn_sal_phy_t                  *phy,
                                                     void                                 *dependency,
                                                     cfn_sal_dev_gyroscope_callback_t      callback,
                                                     void                                 *user_arg);
cfn_hal_error_code_t cfn_sal_dev_gyroscope_destruct(cfn_sal_dev_gyroscope_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_init(cfn_sal_dev_gyroscope_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_deinit(cfn_sal_dev_gyroscope_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_config_set(cfn_sal_dev_gyroscope_t              *driver,
                                                                     const cfn_sal_dev_gyroscope_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_config_get(cfn_sal_dev_gyroscope_t        *driver,
                                                                     cfn_sal_dev_gyroscope_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_callback_register(cfn_sal_dev_gyroscope_t         *driver,
                                                                            cfn_sal_dev_gyroscope_callback_t callback,
                                                                            void                            *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_power_state_set(cfn_sal_dev_gyroscope_t *driver,
                                                                          cfn_hal_power_state_t    state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_event_enable(cfn_sal_dev_gyroscope_t *driver,
                                                                       uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_event_disable(cfn_sal_dev_gyroscope_t *driver,
                                                                        uint32_t                 event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_event_get(cfn_sal_dev_gyroscope_t *driver,
                                                                    uint32_t                *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_error_enable(cfn_sal_dev_gyroscope_t *driver,
                                                                       uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_error_disable(cfn_sal_dev_gyroscope_t *driver,
                                                                        uint32_t                 error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_error_get(cfn_sal_dev_gyroscope_t *driver,
                                                                    uint32_t                *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_GYROSCOPE, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_read_xyz_mdps(cfn_sal_dev_gyroscope_t      *driver,
                                                                        cfn_sal_dev_gyroscope_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GYROSCOPE, read_xyz_mdps, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_read_xyz_raw(cfn_sal_dev_gyroscope_t      *driver,
                                                                       cfn_sal_dev_gyroscope_data_t *data_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GYROSCOPE, read_xyz_raw, driver, error, data_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_read_fifo(cfn_sal_dev_gyroscope_t      *driver,
                                                                    cfn_sal_dev_gyroscope_data_t *data,
                                                                    size_t                       *count)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GYROSCOPE, read_fifo, driver, error, data, count);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_get_status(cfn_sal_dev_gyroscope_t *driver,
                                                                     uint32_t                *status_flags)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_GYROSCOPE, get_status, driver, error, status_flags);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_get_id(cfn_sal_dev_gyroscope_t *driver, uint32_t *id_out)
{
    return cfn_sal_dev_get_id((void *) driver, id_out);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_gyroscope_handle_interrupt(cfn_sal_dev_gyroscope_t *driver)
{
    return cfn_sal_dev_handle_interrupt((void *) driver);
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_GYROSCOPE_H */