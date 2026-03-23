/**
 * @file cfn_svc_serialization.h
 * @brief Generic Schema-Driven Serialization Service.
 * Provides an abstract interface for encoding and decoding data structures
 * using static schema descriptors (JSON, Binary, CBOR, etc.).
 */

#ifndef CAFFEINE_SERVICES_UTILITIES_CFN_SVC_SERIALIZATION_H
#define CAFFEINE_SERVICES_UTILITIES_CFN_SVC_SERIALIZATION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <stddef.h>
#include "cfn_svc.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SVC_TYPE_SERIALIZATION CFN_SVC_TYPE('S', 'E', 'R')

/**
 * @brief Helper macros for defining serialization schemas.
 */
#define CFN_SVC_SCHEMA_START(name) static const cfn_svc_schema_field_t name##_fields[] = {
#define CFN_SVC_SCHEMA_FIELD(f_name, f_type, struct_type, member)                                                      \
    {                                                                                                                  \
        .name = (f_name), .type = (f_type), .offset = (uint16_t) offsetof((struct_type), (member)),                    \
        .size = (uint16_t) sizeof(((struct_type *) 0)->(member))                                                       \
    }
#define CFN_SVC_SCHEMA_END(name)                                                                                       \
    }                                                                                                                  \
    ;                                                                                                                  \
    static const cfn_svc_schema_t name = { .fields = name##_fields, .count = CFN_HAL_ARRAY_SIZE(name##_fields) };

/* Types Enums ------------------------------------------------------*/

/**
 * @brief Supported data types for schema fields.
 */
typedef enum
{
    CFN_SVC_SCHEMA_FIELD_TYPE_INT32,
    CFN_SVC_SCHEMA_FIELD_TYPE_UINT32,
    CFN_SVC_SCHEMA_FIELD_TYPE_FLOAT,
    CFN_SVC_SCHEMA_FIELD_TYPE_BOOL,
    CFN_SVC_SCHEMA_FIELD_TYPE_STRING, /*!< Null-terminated char array */
    CFN_SVC_SCHEMA_FIELD_TYPE_BYTES,  /*!< Raw byte array */
    CFN_SVC_SCHEMA_FIELD_TYPE_MAX
} cfn_svc_schema_field_type_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Metadata for a single field in a data structure.
 */
typedef struct
{
    const char                 *name;   /*!< Field name (e.g., key in JSON) */
    cfn_svc_schema_field_type_t type;   /*!< Data type of the field */
    uint16_t                    offset; /*!< Offset from the start of the struct */
    uint16_t                    size;   /*!< Size of the field in bytes */
} cfn_svc_schema_field_t;

/**
 * @brief Complete schema descriptor for a data structure.
 */
typedef struct
{
    const cfn_svc_schema_field_t *fields;
    size_t                        count;
} cfn_svc_schema_t;

typedef struct cfn_svc_serialization_s     cfn_svc_serialization_t;
typedef struct cfn_svc_serialization_api_s cfn_svc_serialization_api_t;

typedef void (*cfn_svc_serialization_callback_t)(cfn_svc_serialization_t *driver,
                                                 uint32_t                 event,
                                                 uint32_t                 error,
                                                 void                    *user_arg);

/**
 * @brief Virtual Method Table for Serialization Services.
 */
struct cfn_svc_serialization_api_s
{
    cfn_hal_api_base_t base;

    /**
     * @brief Encodes a data structure into a buffer based on a schema.
     */
    cfn_hal_error_code_t (*encode)(cfn_svc_serialization_t *driver,
                                   const cfn_svc_schema_t  *schema,
                                   const void              *data_ptr,
                                   uint8_t                 *out_buf,
                                   size_t                   out_size,
                                   size_t                  *bytes_written);

    /**
     * @brief Decodes a buffer into a data structure based on a schema.
     */
    cfn_hal_error_code_t (*decode)(cfn_svc_serialization_t *driver,
                                   const cfn_svc_schema_t  *schema,
                                   const uint8_t           *in_buf,
                                   size_t                   in_size,
                                   void                    *data_ptr,
                                   size_t                  *bytes_read);
};

CFN_HAL_VMT_CHECK(struct cfn_svc_serialization_api_s);

typedef struct
{
    void *custom; /*!< Codec-specific configuration */
} cfn_svc_serialization_config_t;

struct cfn_svc_serialization_s
{
    cfn_hal_driver_t                      base;
    const cfn_svc_serialization_api_t    *api;
    const cfn_svc_phy_t                  *phy;
    const cfn_svc_serialization_config_t *config;
    cfn_svc_serialization_callback_t      cb;
    void                                 *cb_user_arg;
};
typedef struct cfn_svc_serialization_s cfn_svc_serialization_t;

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_svc_serialization_populate(cfn_svc_serialization_t              *driver,
                                                   uint32_t                              peripheral_id,
                                                   const cfn_svc_serialization_api_t    *api,
                                                   const cfn_svc_phy_t                  *phy,
                                                   const cfn_svc_serialization_config_t *config,
                                                   cfn_svc_serialization_callback_t      callback,
                                                   void                                 *user_arg)
{
    if (!driver)
    {
        return;
    }
    cfn_hal_base_populate(&driver->base, CFN_SVC_TYPE_SERIALIZATION, peripheral_id, api ? &api->base : NULL, NULL);
    driver->api = api;
    driver->phy = phy;
    driver->config = config;
    driver->cb = callback;
    driver->cb_user_arg = user_arg;
}

cfn_hal_error_code_t cfn_svc_serialization_construct(cfn_svc_serialization_t              *driver,
                                                     const cfn_svc_serialization_config_t *config,
                                                     const cfn_svc_phy_t                  *phy,
                                                     cfn_svc_serialization_callback_t      callback,
                                                     void                                 *user_arg);

cfn_hal_error_code_t cfn_svc_serialization_destruct(cfn_svc_serialization_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_serialization_init(cfn_svc_serialization_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SVC_TYPE_SERIALIZATION);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_serialization_encode(cfn_svc_serialization_t *driver,
                                                                 const cfn_svc_schema_t  *schema,
                                                                 const void              *data_ptr,
                                                                 uint8_t                 *out_buf,
                                                                 size_t                   out_size,
                                                                 size_t                  *bytes_written)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SVC_TYPE_SERIALIZATION, encode, driver, error, schema, data_ptr, out_buf, out_size, bytes_written);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_svc_serialization_decode(cfn_svc_serialization_t *driver,
                                                                 const cfn_svc_schema_t  *schema,
                                                                 const uint8_t           *in_buf,
                                                                 size_t                   in_size,
                                                                 void                    *data_ptr,
                                                                 size_t                  *bytes_read)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SVC_TYPE_SERIALIZATION, decode, driver, error, schema, in_buf, in_size, data_ptr, bytes_read);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SERVICES_UTILITIES_CFN_SVC_SERIALIZATION_H */
