/**
 * @file compliance.c
 * @brief Rigorous C file to verify C11 compliance and macro safety for Services.
 */

#include "cfn_svc.h"
#include "devices/cfn_svc_accel.h"
#include "devices/cfn_svc_battery.h"
#include "devices/cfn_svc_button.h"
#include "devices/cfn_svc_display.h"
#include "devices/cfn_svc_gnss.h"
#include "devices/cfn_svc_gsm.h"
#include "devices/cfn_svc_hum_sensor.h"
#include "devices/cfn_svc_led.h"
#include "devices/cfn_svc_light_sensor.h"
#include "devices/cfn_svc_pressure_sensor.h"
#include "devices/cfn_svc_temp_sensor.h"
#include "network/cfn_svc_connection.h"
#include "network/cfn_svc_transport.h"
#include "utilities/cfn_svc_at_parser.h"
#include "utilities/cfn_svc_cli.h"
#include "utilities/cfn_svc_collection.h"
#include "utilities/cfn_svc_fs.h"
#include "utilities/cfn_svc_logging.h"

/* Dummy Implementation for Compliance Testing */

static cfn_hal_error_code_t dummy_init(cfn_hal_driver_t *base)
{
    (void) base;
    return CFN_HAL_ERROR_OK;
}

static const cfn_svc_led_api_t DUMMY_LED_API = {
    .base = {
        .init = dummy_init,
        .deinit = NULL,
        .power_state_set = NULL,
        .config_set = NULL,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = NULL,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = NULL,
    },
    .set_state = NULL,
    .get_state = NULL,
    .toggle = NULL,
    .set_brightness = NULL,
    .get_brightness = NULL,
    .set_color = NULL,
    .blink_start = NULL,
    .blink_stop = NULL
};

static const cfn_svc_led_config_t LED_CONFIG =
{
    .active_low = true,
    .custom =  NULL
};

static const cfn_svc_phy_t LED_PHY = CFN_SVC_PHY_INITIALIZER(NULL, (void *) 0x40000000, CFN_HAL_PERIPHERAL_TYPE_GPIO, NULL);


int main(void)
{
    /* 1. Test Static Initializer Macros */
    cfn_svc_led_t     led = {0};


    cfn_svc_led_populate(&led,
                                         0xDEAD,
                                         &DUMMY_LED_API,
                                         &LED_PHY,
                                         &LED_CONFIG,
                                         NULL,
                                         NULL);
    /* 2. Test Base Initialization (Macro Expansion) */
    cfn_hal_error_code_t err = cfn_svc_led_init(&led);
    if (err != CFN_HAL_ERROR_OK)
    {
        return (int) err;
    }

    /* 3. Test Service Extension Macro Expansion */
    err = cfn_svc_led_toggle(&led);

    /* 4. Test Lock Macro Expansion (CFN_HAL_WITH_LOCK) */
    cfn_hal_error_code_t lock_result;
    CFN_HAL_WITH_LOCK(&led, 100, lock_result, cfn_svc_led_set_state, CFN_SVC_LED_STATE_ON);

    return (int) (lock_result + err);
}
