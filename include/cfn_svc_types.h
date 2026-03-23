/**
 * @file cfn_svc_types.h
 * @brief Common types for Caffeine Services.
 */

#ifndef CAFFEINE_SERVICES_CFN_SVC_TYPES_H
#define CAFFEINE_SERVICES_CFN_SVC_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "cfn_hal.h"

/**
 * @brief Generic physical mapping for services.
 *
 * This structure serves as the foundational physical interface (PHY) for services
 * that interact with HAL peripherals (e.g., I2C, SPI, UART). It is designed to be
 * extensible by embedding it as the first member of a specialized PHY structure.
 */
typedef struct
{
    void    *handle;   /*!< Pointer to the HAL peripheral driver (e.g., cfn_hal_i2c_t *) */
    void    *instance; /*!< Addressing info: I2C device address, SPI CS pin handle, etc. */
    uint32_t type;     /*!< Peripheral type identifier (e.g., CFN_HAL_PERIPHERAL_TYPE_I2C) */
    void    *user_arg; /*!< Optional custom argument for shared contexts or callbacks */
} cfn_svc_phy_t;

#define CFN_SVC_PHY_INITIALIZER(_handle, _instance, _type, _user_arg)                                                  \
    {                                                                                                                  \
        .handle = (_handle), .instance = (_instance), .type = (_type), .user_arg = (_user_arg)                         \
    }

/**
 * @brief Foundational reference counter for shared combination sensor contexts.
 */
typedef struct
{
    uint8_t              ref_count;
    cfn_hal_error_code_t last_init_error;
} cfn_svc_shared_ctx_t;

/**
 * @brief Static initializer for a shared context.
 */
#define CFN_SVC_SHARED_CTX_INITIALIZER()                                                                               \
    {                                                                                                                  \
        .ref_count = 0, .last_init_error = CFN_HAL_ERROR_OK                                                            \
    }

/**
 * @brief Safely increments the reference count.
 * @return true if this is the first initialization (hardware should be initialized).
 */
static inline bool cfn_svc_shared_ctx_should_init(cfn_svc_shared_ctx_t *ctx)
{
    if (!ctx)
    {
        return false;
    }
    if (ctx->ref_count == 0)
    {
        ctx->ref_count++;
        return true;
    }
    ctx->ref_count++;
    return false;
}

/**
 * @brief Safely decrements the reference count.
 * @return true if this is the final deinitialization (hardware should be turned off).
 */
static inline bool cfn_svc_shared_ctx_should_deinit(cfn_svc_shared_ctx_t *ctx)
{
    if (!ctx || ctx->ref_count == 0)
    {
        return false;
    }
    ctx->ref_count--;
    return (ctx->ref_count == 0);
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_CFN_SVC_TYPES_H */
