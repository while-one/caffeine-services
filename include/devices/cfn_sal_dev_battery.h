/**
 * @file cfn_sal_dev_battery.h
 * @brief Abstract Battery Monitor service interface.
 */

#ifndef CAFFEINE_SAL_DEV_BATTERY_H
#define CAFFEINE_SAL_DEV_BATTERY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_DEV_TYPE_BATTERY CFN_SAL_TYPE('B', 'A', 'T')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_DEV_BATTERY_EVENT_NONE        = 0,
    CFN_SAL_DEV_BATTERY_EVENT_CHARGING    = CFN_HAL_BIT(0),
    CFN_SAL_DEV_BATTERY_EVENT_DISCHARGING = CFN_HAL_BIT(1),
    CFN_SAL_DEV_BATTERY_EVENT_FULL        = CFN_HAL_BIT(2),
    CFN_SAL_DEV_BATTERY_EVENT_LOW         = CFN_HAL_BIT(3),
    CFN_SAL_DEV_BATTERY_EVENT_CRITICAL    = CFN_HAL_BIT(4),
} cfn_sal_dev_battery_event_t;

typedef enum
{
    CFN_SAL_DEV_BATTERY_STATUS_UNKNOWN = 0,
    CFN_SAL_DEV_BATTERY_STATUS_CHARGING,
    CFN_SAL_DEV_BATTERY_STATUS_DISCHARGING,
    CFN_SAL_DEV_BATTERY_STATUS_NOT_CHARGING,
    CFN_SAL_DEV_BATTERY_STATUS_FULL
} cfn_sal_dev_battery_status_t;

typedef enum
{
    CFN_SAL_DEV_BATTERY_STATE_DISCHARGING = 0,
    CFN_SAL_DEV_BATTERY_STATE_CHARGING,
    CFN_SAL_DEV_BATTERY_STATE_FULL,
    CFN_SAL_DEV_BATTERY_STATE_FAULT
} cfn_sal_dev_battery_charge_state_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    uint32_t design_capacity_mah;
    float    voltage_min;
    float    voltage_max;
    void    *custom;
} cfn_sal_dev_battery_config_t;

typedef struct cfn_sal_dev_battery_s     cfn_sal_dev_battery_t;
typedef struct cfn_sal_dev_battery_api_s cfn_sal_dev_battery_api_t;

typedef void (*cfn_sal_dev_battery_callback_t)(cfn_sal_dev_battery_t *driver,
                                               uint32_t               event,
                                               uint32_t               error,
                                               void                  *user_arg);

/**
 * @brief Battery Monitor Virtual Method Table (VMT).
 */
struct cfn_sal_dev_battery_api_s
{
    cfn_hal_api_base_t base;

    /* Measurement Operations */
    cfn_hal_error_code_t (*get_voltage)(cfn_sal_dev_battery_t *driver, float *volts_out);
    cfn_hal_error_code_t (*get_percentage)(cfn_sal_dev_battery_t *driver, uint8_t *percent_out);
    cfn_hal_error_code_t (*get_current)(cfn_sal_dev_battery_t *driver, float *amps_out);
    cfn_hal_error_code_t (*get_remaining_capacity)(cfn_sal_dev_battery_t *driver, uint32_t *mah_out);

    cfn_hal_error_code_t (*read_current_ma)(cfn_sal_dev_battery_t *driver, float *current_out);
    cfn_hal_error_code_t (*read_capacity_mah)(cfn_sal_dev_battery_t *driver, float *cap_out);
    cfn_hal_error_code_t (*read_state_of_charge)(cfn_sal_dev_battery_t *driver, float *soc_percent_out);

    /* Status */
    cfn_hal_error_code_t (*get_status)(cfn_sal_dev_battery_t *driver, cfn_sal_dev_battery_status_t *status_out);
    cfn_hal_error_code_t (*get_charge_state)(cfn_sal_dev_battery_t              *driver,
                                             cfn_sal_dev_battery_charge_state_t *state_out);
    cfn_hal_error_code_t (*get_temperature)(cfn_sal_dev_battery_t *driver, float *temp_out);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_dev_battery_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_dev_battery,
                           cfn_sal_dev_battery_config_t,
                           cfn_sal_dev_battery_api_t,
                           cfn_sal_phy_t,
                           cfn_sal_dev_battery_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_dev_battery_populate(cfn_sal_dev_battery_t              *driver,
                                                 uint32_t                            peripheral_id,
                                                 void                               *dependency,
                                                 const cfn_sal_dev_battery_api_t    *api,
                                                 const cfn_sal_phy_t                *phy,
                                                 const cfn_sal_dev_battery_config_t *config,
                                                 cfn_sal_dev_battery_callback_t      callback,
                                                 void                               *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_DEV_TYPE_BATTERY, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_dev_battery_construct(cfn_sal_dev_battery_t              *driver,
                                                   const cfn_sal_dev_battery_config_t *config,
                                                   const cfn_sal_phy_t                *phy,
                                                   void                               *dependency,
                                                   cfn_sal_dev_battery_callback_t      callback,
                                                   void                               *user_arg);
cfn_hal_error_code_t cfn_sal_dev_battery_destruct(cfn_sal_dev_battery_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_init(cfn_sal_dev_battery_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_DEV_TYPE_BATTERY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_deinit(cfn_sal_dev_battery_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_DEV_TYPE_BATTERY);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_config_set(cfn_sal_dev_battery_t              *driver,
                                                                   const cfn_sal_dev_battery_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_config_get(cfn_sal_dev_battery_t        *driver,
                                                                   cfn_sal_dev_battery_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_callback_register(cfn_sal_dev_battery_t         *driver,
                                                                          cfn_sal_dev_battery_callback_t callback,
                                                                          void                          *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(
        &driver->base, CFN_SAL_DEV_TYPE_BATTERY, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_power_state_set(cfn_sal_dev_battery_t *driver,
                                                                        cfn_hal_power_state_t  state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_event_enable(cfn_sal_dev_battery_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_event_disable(cfn_sal_dev_battery_t *driver,
                                                                      uint32_t               event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_event_get(cfn_sal_dev_battery_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_error_enable(cfn_sal_dev_battery_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_error_disable(cfn_sal_dev_battery_t *driver,
                                                                      uint32_t               error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_error_get(cfn_sal_dev_battery_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_DEV_TYPE_BATTERY, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_voltage(cfn_sal_dev_battery_t *driver, float *volts_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_voltage, driver, error, volts_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_percentage(cfn_sal_dev_battery_t *driver,
                                                                       uint8_t               *percent_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_percentage, driver, error, percent_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_current(cfn_sal_dev_battery_t *driver, float *amps_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_current, driver, error, amps_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_remaining_capacity(cfn_sal_dev_battery_t *driver,
                                                                               uint32_t              *mah_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_remaining_capacity, driver, error, mah_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_status(cfn_sal_dev_battery_t        *driver,
                                                                   cfn_sal_dev_battery_status_t *status_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_status, driver, error, status_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_temperature(cfn_sal_dev_battery_t *driver, float *temp_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_temperature, driver, error, temp_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_read_current_ma(cfn_sal_dev_battery_t *driver,
                                                                        float                 *current_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, read_current_ma, driver, error, current_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_read_capacity_mah(cfn_sal_dev_battery_t *driver, float *cap_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, read_capacity_mah, driver, error, cap_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_read_state_of_charge(cfn_sal_dev_battery_t *driver,
                                                                             float                 *soc_percent_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, read_state_of_charge, driver, error, soc_percent_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_dev_battery_get_charge_state(cfn_sal_dev_battery_t              *driver,
                                                                         cfn_sal_dev_battery_charge_state_t *state_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_DEV_TYPE_BATTERY, get_charge_state, driver, error, state_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEV_BATTERY_H */
