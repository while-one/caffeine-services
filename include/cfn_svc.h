/**
 * @file cfn_svc.h
 * @brief Core definitions and macros for Caffeine Services.
 */

#ifndef CAFFEINE_SERVICES_CFN_SVC_H
#define CAFFEINE_SERVICES_CFN_SVC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_base.h"

/* Defines ----------------------------------------------------------*/

/**
 * @brief Prefix for all Service-level types.
 * Distinguishes services ('S') from HAL peripherals ('A').
 */
#define CFN_SVC_PREFIX 'S'

/**
 * @brief Helper macro to create a Service FourCC type.
 */
#define CFN_SVC_TYPE(a, b, c) CFN_HAL_MAKE_TYPE(CFN_SVC_PREFIX, a, b, c)

/**
 * @brief Service-level error code base offset.
 * Offsets service errors from HAL errors to avoid collisions.
 */
#define CFN_SVC_ERROR_BASE 0x600

/**
 * @brief Generic macro for Service static initializers.
 * Reuses the HAL initialization logic since Services use the same base driver container.
 */
#define CFN_SVC_DRIVER_INITIALIZER(type_code, api_ptr, phy_ptr, config_ptr) \
    CFN_HAL_DRIVER_INITIALIZER(type_code, api_ptr, phy_ptr, config_ptr)

#define CFN_SVC_CREATE_DRIVER_TYPE(prefix, config_type, api_type, phy_type, cb_type)                                   \
    struct cfn_##prefix##_s                                                                                            \
    {                                                                                                                  \
        cfn_hal_driver_t   base;                                                                                       \
        const config_type *config;                                                                                     \
        const api_type    *api;                                                                                        \
        const phy_type    *phy;                                                                                        \
        cb_type            cb;                                                                                         \
        void              *cb_user_arg;                                                                                \
    };                                                                                                                 \
    typedef struct cfn_##prefix##_s cfn_##prefix##_t

/* Types Enums ------------------------------------------------------*/

/* Extend cfn_hal_error_code_t with service specific errors if needed */

/* Types Structs ----------------------------------------------------*/

/* Functions prototypes ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_CFN_SVC_H */
