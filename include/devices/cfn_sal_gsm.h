/**
 * @file cfn_sal_gsm.h
 * @brief Abstract GSM/Cellular service interface.
 */

#ifndef CAFFEINE_SAL_DEVICES_CFN_SAL_GSM_H
#define CAFFEINE_SAL_DEVICES_CFN_SAL_GSM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_GSM CFN_SAL_TYPE('G', 'S', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SAL_GSM_EVENT_NONE               = 0,
    CFN_SAL_GSM_EVENT_POWERED_ON         = CFN_HAL_BIT(0),
    CFN_SAL_GSM_EVENT_NETWORK_REGISTERED = CFN_HAL_BIT(1),
    CFN_SAL_GSM_EVENT_SMS_RECEIVED       = CFN_HAL_BIT(2),
    CFN_SAL_GSM_EVENT_CALL_INCOMING      = CFN_HAL_BIT(3),
    CFN_SAL_GSM_EVENT_MQTT_CONNECTED     = CFN_HAL_BIT(4),
} cfn_sal_gsm_event_t;

typedef enum
{
    CFN_SAL_GSM_NET_NOT_REGISTERED = 0,
    CFN_SAL_GSM_NET_REGISTERED_HOME,
    CFN_SAL_GSM_NET_SEARCHING,
    CFN_SAL_GSM_NET_REGISTRATION_DENIED,
    CFN_SAL_GSM_NET_UNKNOWN,
    CFN_SAL_GSM_NET_REGISTERED_ROAMING
} cfn_sal_gsm_net_status_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    const char *apn;
    const char *sim_pin;
    void       *custom;
} cfn_sal_gsm_config_t;

typedef struct cfn_sal_gsm_s     cfn_sal_gsm_t;
typedef struct cfn_sal_gsm_api_s cfn_sal_gsm_api_t;

typedef void (*cfn_sal_gsm_callback_t)(cfn_sal_gsm_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief GSM Virtual Method Table (VMT).
 */
struct cfn_sal_gsm_api_s
{
    cfn_hal_api_base_t base;

    /* Power & Control */
    cfn_hal_error_code_t (*power_on)(cfn_sal_gsm_t *driver);
    cfn_hal_error_code_t (*power_off)(cfn_sal_gsm_t *driver);
    cfn_hal_error_code_t (*reset)(cfn_sal_gsm_t *driver);

    /* Network Info */
    cfn_hal_error_code_t (*get_signal_quality)(cfn_sal_gsm_t *driver, int32_t *rssi_dbm);
    cfn_hal_error_code_t (*get_signal_quality_csq)(cfn_sal_gsm_t *driver, uint8_t *csq_out);
    cfn_hal_error_code_t (*get_network_status)(cfn_sal_gsm_t *driver, cfn_sal_gsm_net_status_t *status);
    cfn_hal_error_code_t (*get_operator)(cfn_sal_gsm_t *driver, char *operator_out, size_t max_len);

    /* Identification */
    cfn_hal_error_code_t (*get_imei)(cfn_sal_gsm_t *driver, char *imei_out, size_t max_len);
    cfn_hal_error_code_t (*get_imsi)(cfn_sal_gsm_t *driver, char *imsi_out, size_t max_len);
    cfn_hal_error_code_t (*get_iccid)(cfn_sal_gsm_t *driver, char *iccid_out, size_t max_len);

    /* SMS */
    cfn_hal_error_code_t (*send_sms)(cfn_sal_gsm_t *driver, const char *number, const char *text);
    cfn_hal_error_code_t (*read_sms)(
        cfn_sal_gsm_t *driver, uint16_t index, char *number_out, char *message_out, size_t max_len);

    /* Voice Calls */
    cfn_hal_error_code_t (*dial)(cfn_sal_gsm_t *driver, const char *number);
    cfn_hal_error_code_t (*answer)(cfn_sal_gsm_t *driver);
    cfn_hal_error_code_t (*hangup)(cfn_sal_gsm_t *driver);

    /* Data / IP (Simplified) */
    cfn_hal_error_code_t (*http_get)(cfn_sal_gsm_t *driver, const char *url, uint8_t *response, size_t *len);
    cfn_hal_error_code_t (*mqtt_publish)(cfn_sal_gsm_t *driver, const char *topic, const uint8_t *payload, size_t len);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_gsm_api_s);

CFN_SAL_CREATE_DRIVER_TYPE(sal_gsm, cfn_sal_gsm_config_t, cfn_sal_gsm_api_t, cfn_sal_phy_t, cfn_sal_gsm_callback_t);

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_gsm_populate(cfn_sal_gsm_t              *driver,
                                         uint32_t                    peripheral_id,
                                         void                       *dependency,
                                         const cfn_sal_gsm_api_t    *api,
                                         const cfn_sal_phy_t        *phy,
                                         const cfn_sal_gsm_config_t *config,
                                         cfn_sal_gsm_callback_t      callback,
                                         void                       *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_GSM, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_gsm_construct(cfn_sal_gsm_t              *driver,
                                           const cfn_sal_gsm_config_t *config,
                                           const cfn_sal_phy_t        *phy,
                                           void                       *dependency,
                                           cfn_sal_gsm_callback_t      callback,
                                           void                       *user_arg);
cfn_hal_error_code_t cfn_sal_gsm_destruct(cfn_sal_gsm_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_init(cfn_sal_gsm_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_GSM);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_deinit(cfn_sal_gsm_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_SAL_TYPE_GSM);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_config_set(cfn_sal_gsm_t *driver, const cfn_sal_gsm_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = config;
    return cfn_hal_base_config_set(&driver->base, CFN_SAL_TYPE_GSM, (const void *) config);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_config_get(cfn_sal_gsm_t *driver, cfn_sal_gsm_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_callback_register(cfn_sal_gsm_t         *driver,
                                                                  cfn_sal_gsm_callback_t callback,
                                                                  void                  *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->cb          = callback;
    driver->cb_user_arg = user_arg;
    return cfn_hal_base_callback_register(&driver->base, CFN_SAL_TYPE_GSM, (cfn_hal_callback_t) callback, user_arg);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_power_state_set(cfn_sal_gsm_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_SAL_TYPE_GSM, state);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_event_enable(cfn_sal_gsm_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_SAL_TYPE_GSM, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_event_disable(cfn_sal_gsm_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_SAL_TYPE_GSM, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_event_get(cfn_sal_gsm_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_SAL_TYPE_GSM, event_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_error_enable(cfn_sal_gsm_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_SAL_TYPE_GSM, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_error_disable(cfn_sal_gsm_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_SAL_TYPE_GSM, error_mask);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_error_get(cfn_sal_gsm_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_SAL_TYPE_GSM, error_mask);
}

/* Service Specific Functions --------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_power_on(cfn_sal_gsm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_GSM, power_on, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_power_off(cfn_sal_gsm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_GSM, power_off, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_reset(cfn_sal_gsm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_GSM, reset, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_signal_quality(cfn_sal_gsm_t *driver, int32_t *rssi_dbm)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_signal_quality, driver, error, rssi_dbm);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_network_status(cfn_sal_gsm_t            *driver,
                                                                   cfn_sal_gsm_net_status_t *status)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_network_status, driver, error, status);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_send_sms(cfn_sal_gsm_t *driver, const char *number, const char *text)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, send_sms, driver, error, number, text);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_http_get(cfn_sal_gsm_t *driver,
                                                         const char    *url,
                                                         uint8_t       *response,
                                                         size_t        *len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, http_get, driver, error, url, response, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_mqtt_publish(cfn_sal_gsm_t *driver,
                                                             const char    *topic,
                                                             const uint8_t *payload,
                                                             size_t         len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, mqtt_publish, driver, error, topic, payload, len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_imei(cfn_sal_gsm_t *driver, char *imei_out, size_t max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_imei, driver, error, imei_out, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_imsi(cfn_sal_gsm_t *driver, char *imsi_out, size_t max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_imsi, driver, error, imsi_out, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_iccid(cfn_sal_gsm_t *driver, char *iccid_out, size_t max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_iccid, driver, error, iccid_out, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_operator(cfn_sal_gsm_t *driver, char *operator_out, size_t max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_operator, driver, error, operator_out, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_get_signal_quality_csq(cfn_sal_gsm_t *driver, uint8_t *csq_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, get_signal_quality_csq, driver, error, csq_out);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t
cfn_sal_gsm_read_sms(cfn_sal_gsm_t *driver, uint16_t index, char *number_out, char *message_out, size_t max_len)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_TYPE_GSM, read_sms, driver, error, index, number_out, message_out, max_len);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_dial(cfn_sal_gsm_t *driver, const char *number)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SAL_TYPE_GSM, dial, driver, error, number);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_answer(cfn_sal_gsm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_GSM, answer, driver, error);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_gsm_hangup(cfn_sal_gsm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_SAL_TYPE_GSM, hangup, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_DEVICES_CFN_SAL_GSM_H */
