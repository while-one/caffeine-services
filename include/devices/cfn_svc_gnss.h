/**
 * @file cfn_svc_gnss.h
 * @brief Abstract GNSS (GPS/Glonass/Galileo) service interface.
 */

#ifndef CAFFEINE_SERVICES_DEVICES_CFN_SVC_GNSS_H
#define CAFFEINE_SERVICES_DEVICES_CFN_SVC_GNSS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <time.h>
#include "cfn_svc.h"
#include "cfn_svc_types.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_GNSS CFN_SVC_TYPE('G', 'N', 'S')

/* Types Enums ------------------------------------------------------*/

typedef enum
{
    CFN_SVC_GNSS_EVENT_NONE = 0,
    CFN_SVC_GNSS_EVENT_FIX_ACQUIRED = CFN_HAL_BIT(0),
    CFN_SVC_GNSS_EVENT_FIX_LOST = CFN_HAL_BIT(1),
    CFN_SVC_GNSS_EVENT_DATA_READY = CFN_HAL_BIT(2),
} cfn_svc_gnss_event_t;

typedef enum
{
    CFN_SVC_GNSS_FIX_NONE = 0,
    CFN_SVC_GNSS_FIX_2D,
    CFN_SVC_GNSS_FIX_3D,
    CFN_SVC_GNSS_FIX_DGPS,
    CFN_SVC_GNSS_FIX_RTK_FIXED,
    CFN_SVC_GNSS_FIX_RTK_FLOAT
} cfn_svc_gnss_fix_type_t;

/* Types Structs ----------------------------------------------------*/

typedef struct
{
    double                  latitude;
    double                  longitude;
    float                   altitude_m;
    float                   speed_kmh;
    float                   course_deg;
    float                   hdop;
    cfn_svc_gnss_fix_type_t fix_type;
    uint8_t                 satellites_used;
} cfn_svc_gnss_location_t;

typedef struct
{
    void *instance; /*!< Mapping to UART handle or I2C/SPI driver */
    void *user_arg;
} cfn_svc_gnss_phy_t;

typedef struct
{
    uint32_t fix_rate_ms;
    uint32_t baudrate; /* If UART based */
    void    *custom;
} cfn_svc_gnss_config_t;

typedef struct cfn_svc_gnss_s     cfn_svc_gnss_t;
typedef struct cfn_svc_gnss_api_s cfn_svc_gnss_api_t;

typedef void (*cfn_svc_gnss_callback_t)(cfn_svc_gnss_t *driver, uint32_t event, uint32_t error, void *user_arg);

/**
 * @brief GNSS Virtual Method Table (VMT).
 */
struct cfn_svc_gnss_api_s
{
    cfn_hal_api_base_t base;

    /* Power & Control */
    cfn_hal_error_code_t (*power_on)(cfn_svc_gnss_t *driver);
    cfn_hal_error_code_t (*power_off)(cfn_svc_gnss_t *driver);

    /* Data Retrieval */
    cfn_hal_error_code_t (*get_location)(cfn_svc_gnss_t *driver, cfn_svc_gnss_location_t *loc_out);
    cfn_hal_error_code_t (*get_time)(cfn_svc_gnss_t *driver, struct tm *time_out);
    cfn_hal_error_code_t (*get_satellites_in_view)(cfn_svc_gnss_t *driver, uint8_t *count_out);

    /* Configuration */
    cfn_hal_error_code_t (*set_fix_rate)(cfn_svc_gnss_t *driver, uint32_t rate_ms);

    /* Maintenance */
    cfn_hal_error_code_t (*cold_start)(cfn_svc_gnss_t *driver);
    cfn_hal_error_code_t (*warm_start)(cfn_svc_gnss_t *driver);
    cfn_hal_error_code_t (*hot_start)(cfn_svc_gnss_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_gnss_api_s);

CFN_SVC_CREATE_DRIVER_TYPE(svc_gnss, cfn_svc_gnss_config_t, cfn_svc_gnss_api_t, cfn_svc_gnss_phy_t, cfn_svc_gnss_callback_t);

#define CFN_SVC_GNSS_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                       \
    CFN_SVC_DRIVER_INITIALIZER(CFN_SVC_TYPE_GNSS, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_gnss_init(cfn_svc_gnss_t *driver)
{
    if (!driver) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_GNSS);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_gnss_get_location(cfn_svc_gnss_t *driver, cfn_svc_gnss_location_t *loc_out)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_SVC_TYPE_GNSS, get_location, driver, error, loc_out);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_DEVICES_CFN_SVC_GNSS_H */
