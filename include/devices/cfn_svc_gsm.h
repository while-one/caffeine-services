/**
 * @file cfn_svc_gsm.h
 * @brief Abstract GSM/Cellular service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_GSM_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_GSM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_GSM CFN_SVC_TYPE('G', 'S', 'M')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_GSM_EVENT_NONE = 0,
    CFN_SVC_GSM_EVENT_POWERED_ON = CFN_HAL_BIT(0),
    CFN_SVC_GSM_EVENT_NETWORK_REGISTERED = CFN_HAL_BIT(1),
    CFN_SVC_GSM_EVENT_SMS_RECEIVED = CFN_HAL_BIT(2),
    CFN_SVC_GSM_EVENT_CALL_INCOMING = CFN_HAL_BIT(3),
    CFN_SVC_GSM_EVENT_MQTT_CONNECTED = CFN_HAL_BIT(4),
} cfn_svc_gsm_event_t;

typedef enum
{
    CFN_SVC_GSM_NET_NOT_REGISTERED = 0,
    CFN_SVC_GSM_NET_REGISTERED_HOME,
    CFN_SVC_GSM_NET_SEARCHING,
    CFN_SVC_GSM_NET_REGISTRATION_DENIED,
    CFN_SVC_GSM_NET_UNKNOWN,
    CFN_SVC_GSM_NET_REGISTERED_ROAMING
} cfn_svc_gsm_net_status_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void *instance; /*!< Mapping to AT Parser or UART handle */
    void *user_arg;
} cfn_svc_gsm_phy_t;

typedef struct
{
    const char *apn;
    const char *sim_pin;
    void       *custom;
} cfn_svc_gsm_config_t;

typedef struct cfn_svc_gsm_s     cfn_svc_gsm_t;
typedef struct cfn_svc_gsm_api_s cfn_svc_gsm_api_t;

typedef void (*cfn_svc_gsm_callback_t)(cfn_svc_gsm_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief GSM Virtual Method Table (VMT).
 */
struct cfn_svc_gsm_api_s
{
    cfn_hal_api_base_t base;

    /* Power & Control */
    cfn_hal_error_code_t (*power_on)(cfn_svc_gsm_t *driver);
    cfn_hal_error_code_t (*power_off)(cfn_svc_gsm_t *driver);
    cfn_hal_error_code_t (*reset)(cfn_svc_gsm_t *driver);

    /* Network Info */
    cfn_hal_error_code_t (*get_signal_quality)(cfn_svc_gsm_t *driver, int32_t *rssi_dbm);
    cfn_hal_error_code_t (*get_network_status)(cfn_svc_gsm_t *driver, cfn_svc_gsm_net_status_t *status);

    /* SMS */
    cfn_hal_error_code_t (*send_sms)(cfn_svc_gsm_t *driver, const char *number, const char *text);

    /* Data / IP (Simplified) */
    cfn_hal_error_code_t (*http_get)(cfn_svc_gsm_t *driver, const char *url, uint8_t *response, size_t *len);
    cfn_hal_error_code_t (*mqtt_publish)(cfn_svc_gsm_t *driver, const char *topic, const uint8_t *payload, size_t len);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_gsm_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_gsm, cfn_svc_gsm_config_t, cfn_svc_gsm_api_t, cfn_svc_gsm_phy_t, cfn_svc_gsm_callback_t);

#define CFN_SVC_GSM_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                        \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_GSM, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_gsm_init(cfn_svc_gsm_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_GSM);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_gsm_send_sms(cfn_svc_gsm_t *driver, const char *number, const char *text)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_GSM, send_sms, driver, error, number, text);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_GSM_H */
