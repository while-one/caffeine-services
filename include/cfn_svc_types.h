/**
 * @file cfn_svc_types.h
 * @brief Common type definitions and error codes for Caffeine Services.
 */

#ifndef CAFFEINE_SERVICES_CFN_SVC_TYPES_H
#define CAFFEINE_SERVICES_CFN_SVC_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief Service-level error codes.
 * Extends the cfn_hal_error_code_t space starting from CFN_SVC_ERROR_BASE (0x600).
 */
typedef enum
{
    CFN_SVC_ERROR_OK = CFN_HAL_ERROR_OK,

    /* Collection Specific Errors */
    CFN_SVC_ERROR_COLLECTION_FULL = CFN_SVC_ERROR_BASE,
    CFN_SVC_ERROR_COLLECTION_EMPTY,
    CFN_SVC_ERROR_COLLECTION_NOT_FOUND,

    /* Sensor Specific Errors */
    CFN_SVC_ERROR_SENSOR_COMM_FAIL = 0x700,
    CFN_SVC_ERROR_SENSOR_DATA_INVALID,

    /* Communication Specific Errors */
    CFN_SVC_ERROR_AT_RESPONSE_TIMEOUT = 0x800,
    CFN_SVC_ERROR_AT_RESPONSE_ERROR,
    CFN_SVC_ERROR_GSM_NOT_REGISTERED,

} cfn_svc_error_code_t;

/* Types Structs ----------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_CFN_SVC_TYPES_H */
