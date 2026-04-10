/**
 * @file cfn_sal_serialization.h
 * @brief Generic Schema-Driven Serialization Service.
 * Provides an abstract interface for encoding and decoding data structures
 * using static schema descriptors (JSON, Binary, CBOR, etc.).
 */

#ifndef CAFFEINE_SAL_UTILITIES_CFN_SAL_SERIALIZATION_H
#define CAFFEINE_SAL_UTILITIES_CFN_SAL_SERIALIZATION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <stddef.h>
#include "cfn_sal.h"

/* Defines ----------------------------------------------------------*/

#define CFN_SAL_TYPE_SERIALIZATION CFN_SAL_TYPE('S', 'E', 'R')

/**
 * @brief Helper macros for defining serialization schemas.
 */
#define CFN_SAL_SCHEMA_START(name) static const cfn_sal_schema_field_t name##_fields[] = {
#define CFN_SAL_SCHEMA_FIELD(f_name, f_type, struct_type, member)                                                      \
    {                                                                                                                  \
        .name = (f_name), .type = (f_type), .offset = (uint16_t) offsetof((struct_type), (member)),                    \
        .size = (uint16_t) sizeof(((struct_type *) 0)->(member))                                                       \
    }
#define CFN_SAL_SCHEMA_END(name)                                                                                       \
    }                                                                                                                  \
    ;                                                                                                                  \
    static const cfn_sal_schema_t name = { .fields = name##_fields, .count = CFN_HAL_ARRAY_SIZE(name##_fields) };

/* Types Enums ------------------------------------------------------*/

/**
 * @brief Supported data types for schema fields.
 */
typedef enum
{
    CFN_SAL_SCHEMA_FIELD_TYPE_INT32,
    CFN_SAL_SCHEMA_FIELD_TYPE_UINT32,
    CFN_SAL_SCHEMA_FIELD_TYPE_FLOAT,
    CFN_SAL_SCHEMA_FIELD_TYPE_BOOL,
    CFN_SAL_SCHEMA_FIELD_TYPE_STRING, /*!< Null-terminated char array */
    CFN_SAL_SCHEMA_FIELD_TYPE_BYTES,  /*!< Raw byte array */
    CFN_SAL_SCHEMA_FIELD_TYPE_MAX
} cfn_sal_schema_field_type_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Metadata for a single field in a data structure.
 */
typedef struct
{
    const char                 *name;   /*!< Field name (e.g., key in JSON) */
    cfn_sal_schema_field_type_t type;   /*!< Data type of the field */
    uint16_t                    offset; /*!< Offset from the start of the struct */
    uint16_t                    size;   /*!< Size of the field in bytes */
} cfn_sal_schema_field_t;

/**
 * @brief Complete schema descriptor for a data structure.
 */
typedef struct
{
    const cfn_sal_schema_field_t *fields;
    size_t                        count;
} cfn_sal_schema_t;

typedef struct cfn_sal_serialization_s     cfn_sal_serialization_t;
typedef struct cfn_sal_serialization_api_s cfn_sal_serialization_api_t;

typedef void (*cfn_sal_serialization_callback_t)(cfn_sal_serialization_t *driver,
                                                 uint32_t                 event,
                                                 uint32_t                 error,
                                                 void                    *user_arg);

/**
 * @brief Virtual Method Table for Serialization Services.
 */
struct cfn_sal_serialization_api_s
{
    cfn_hal_api_base_t base;

    /**
     * @brief Encodes a data structure into a buffer based on a schema.
     */
    cfn_hal_error_code_t (*encode)(cfn_sal_serialization_t *driver,
                                   const cfn_sal_schema_t  *schema,
                                   const void              *data_ptr,
                                   uint8_t                 *out_buf,
                                   size_t                   out_size,
                                   size_t                  *bytes_written);

    /**
     * @brief Decodes a buffer into a data structure based on a schema.
     */
    cfn_hal_error_code_t (*decode)(cfn_sal_serialization_t *driver,
                                   const cfn_sal_schema_t  *schema,
                                   const uint8_t           *in_buf,
                                   size_t                   in_size,
                                   void                    *data_ptr,
                                   size_t                  *bytes_read);

    /**
     * @brief Encodes a data structure into a stream based on a schema.
     */
    cfn_hal_error_code_t (*encode_stream)(cfn_sal_serialization_t *driver,
                                          const cfn_sal_schema_t  *schema,
                                          const void              *data_ptr,
                                          void                    *out_stream);

    /**
     * @brief Decodes a stream into a data structure based on a schema.
     */
    cfn_hal_error_code_t (*decode_stream)(cfn_sal_serialization_t *driver,
                                          const cfn_sal_schema_t  *schema,
                                          void                    *in_stream,
                                          void                    *data_ptr);
};

CFN_HAL_VMT_CHECK(struct cfn_sal_serialization_api_s);

typedef struct
{
    void *custom; /*!< Codec-specific configuration */
} cfn_sal_serialization_config_t;

struct cfn_sal_serialization_s
{
    cfn_hal_driver_t                      base;
    const cfn_sal_serialization_api_t    *api;
    const cfn_sal_phy_t                  *phy;
    const cfn_sal_serialization_config_t *config;
    cfn_sal_serialization_callback_t      cb;
    void                                 *cb_user_arg;
};
typedef struct cfn_sal_serialization_s cfn_sal_serialization_t;

/* Functions inline ------------------------------------------------- */

CFN_HAL_INLINE void cfn_sal_serialization_populate(cfn_sal_serialization_t              *driver,
                                                   uint32_t                              peripheral_id,
                                                   void                                 *dependency,
                                                   const cfn_sal_serialization_api_t    *api,
                                                   const cfn_sal_phy_t                  *phy,
                                                   const cfn_sal_serialization_config_t *config,
                                                   cfn_sal_serialization_callback_t      callback,
                                                   void                                 *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_SAL_TYPE_SERIALIZATION, peripheral_id, NULL, dependency, api, phy, config, callback, user_arg);
}

cfn_hal_error_code_t cfn_sal_serialization_construct(cfn_sal_serialization_t              *driver,
                                                     const cfn_sal_serialization_config_t *config,
                                                     const cfn_sal_phy_t                  *phy,
                                                     void                                 *dependency,
                                                     cfn_sal_serialization_callback_t      callback,
                                                     void                                 *user_arg);

cfn_hal_error_code_t cfn_sal_serialization_destruct(cfn_sal_serialization_t *driver);

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_serialization_init(cfn_sal_serialization_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_SAL_TYPE_SERIALIZATION);
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_serialization_encode(cfn_sal_serialization_t *driver,
                                                                 const cfn_sal_schema_t  *schema,
                                                                 const void              *data_ptr,
                                                                 uint8_t                 *out_buf,
                                                                 size_t                   out_size,
                                                                 size_t                  *bytes_written)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_TYPE_SERIALIZATION, encode, driver, error, schema, data_ptr, out_buf, out_size, bytes_written);
    return error;
}

CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_serialization_decode(cfn_sal_serialization_t *driver,
                                                                 const cfn_sal_schema_t  *schema,
                                                                 const uint8_t           *in_buf,
                                                                 size_t                   in_size,
                                                                 void                    *data_ptr,
                                                                 size_t                  *bytes_read)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_TYPE_SERIALIZATION, decode, driver, error, schema, in_buf, in_size, data_ptr, bytes_read);
    return error;
}

/**
 * @brief Encodes a data structure into a stream based on a schema.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_serialization_encode_stream(cfn_sal_serialization_t *driver,
                                                                        const cfn_sal_schema_t  *schema,
                                                                        const void              *data_ptr,
                                                                        void                    *out_stream)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_TYPE_SERIALIZATION, encode_stream, driver, error, schema, data_ptr, out_stream);
    return error;
}

/**
 * @brief Decodes a stream into a data structure based on a schema.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_sal_serialization_decode_stream(cfn_sal_serialization_t *driver,
                                                                        const cfn_sal_schema_t  *schema,
                                                                        void                    *in_stream,
                                                                        void                    *data_ptr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_SAL_TYPE_SERIALIZATION, decode_stream, driver, error, schema, in_stream, data_ptr);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_SAL_UTILITIES_CFN_SAL_SERIALIZATION_H */
