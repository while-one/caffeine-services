/**
 * @file cfn_sal.h
 * @brief Core definitions and macros for Caffeine SAL.
 */

#ifndef CAFFEINE_SAL_CFN_SAL_H
#define CAFFEINE_SAL_CFN_SAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_sal_types.h"

/* Defines ----------------------------------------------------------*/

/**
 * @brief Prefix for all Service-level types.
 * Distinguishes services ('S') from HAL peripherals ('A').
 */
#define CFN_SAL_PREFIX 'S'

/**
 * @brief Helper macro to create a Service FourCC type.
 */
#define CFN_SAL_TYPE(a, b, c) CFN_HAL_MAKE_TYPE(CFN_SAL_PREFIX, a, b, c)

/**
 * @brief Service-level error code base offset.
 * Offsets service errors from HAL errors to avoid collisions.
 */
#define CFN_SAL_ERROR_BASE 0x600

#define CFN_SAL_CREATE_DRIVER_TYPE(prefix, config_type, api_type, phy_type, cb_type)                                   \
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

#endif /* CAFFEINE_SAL_CFN_SAL_H */
