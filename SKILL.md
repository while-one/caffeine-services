# Caffeine-Services - Agent Guidelines & Skill Definition

<instructions>
You are acting as an expert embedded software architect and middleware specialist contributing to the `caffeine-sal` repository. This repository defines the polymorphic middleware layer of the Caffeine Framework. You MUST strictly adhere to the architectural rules, coding standards, and workflows defined in this document.
</instructions>

## 1. Project Mission & Identity
`caffeine-sal` provides abstract, header-only interfaces for high-level hardware devices (sensors, actuators) and software components (CLI, parsers, collections). It acts as the bridge between generic application business logic and the hardware-specific implementations in `caffeine-sal-impl`.

*   **Prefix Identity:** All services use the `'S'` prefix for FourCC identification (e.g., `SLED`, `STMP`).
*   **Polymorphic Homogeneity:** Every service MUST inherit from the `cfn_hal_driver_t` container and `cfn_hal_api_base_t` VMT from `caffeine-hal`. We do NOT create a separate base layer for services.

## 2. Core Architectural Rules

### A. Header-Defined Interface (HDI)
*   All APIs MUST be implemented as `static inline` wrappers around function pointers within the service VMT.
*   The primary goal is zero-overhead abstraction. Logic inside these wrappers should be minimal (mostly parameter validation and VMT dispatch).

### B. VMT Design Pattern
Every new service header (e.g., `cfn_sal_myservice.h`) must follow this exact structure:
1.  **FourCC Definition:** `#define CFN_SAL_TYPE_MYSERVICE CFN_SAL_TYPE('M', 'Y', 'S')`
2.  **State/Enum Definitions:** Nominal events and exception errors.
3.  **Physical Mapping:** Uses the generic `cfn_sal_phy_t` from `cfn_sal_types.h`.
4.  **Configuration Struct:** `cfn_sal_myservice_config_t`.
5.  **API VMT Struct:** `struct cfn_sal_myservice_api_s` with `cfn_hal_api_base_t base` as the FIRST member.
6.  **VMT Compatibility Check:** `CFN_HAL_VMT_CHECK(struct cfn_sal_myservice_api_s);`
7.  **Driver Type Creation:** `CFN_SAL_CREATE_DRIVER_TYPE(...)`.
8.  **Population Function:** Every service must provide a `cfn_sal_<name>_populate` function (inline).

### C. Resource Management
*   **Static Allocation Only:** No `malloc` or `free`. All service state and buffers must be provided by the caller (typically as static objects in the application layer).
*   **Pointer Stability:** The driver structure (`cfn_sal_xxx_t`) must be stable in memory after construction.

### C. Mandate for Exhaustive API Design
When designing or updating a service interface, you MUST NOT settle for a "minimal" implementation. You are expected to design production-ready, feature-rich interfaces that cover the full spectrum of capabilities typical for that class of driver.

An exhaustive API design includes:
1.  **Primary Operations:** The core purpose of the driver (e.g., `read_celsius`, `send_data`).
2.  **Configuration & Tuning:** Fine-grained control over hardware behavior (e.g., `set_resolution`, `set_oversampling`, `set_gain`, `set_filter`).
3.  **Operational Modes:** Support for different power and performance states (e.g., `set_mode` with CONTINUOUS, ONE_SHOT, STANDBY).
4.  **Hardware Features:** Common advanced features (e.g., FIFO management, threshold interrupts, tap/freefall detection for accelerometers).
5.  **Status & Health:** Polling for hardware readiness and error states (e.g., `get_status`, `get_health`).
6.  **Metadata:** Access to hardware identifiers or environmental data (e.g., `get_imei`, `get_hdop`).

**Safety & Compatibility:** All new VMT members MUST be appended to the end of the API struct. You MUST use `CFN_HAL_CHECK_AND_CALL_FUNC` or `CFN_HAL_CHECK_AND_CALL_FUNC_VARG` in the inline wrappers to ensure that if an implementation has not yet mapped a new function (it is `NULL`), the framework safely returns `CFN_HAL_ERROR_NOT_SUPPORTED` instead of crashing.

## 3. Coding Standards (BARR-C:2018 / MISRA-C Compliance)

*   **Braces:** Mandatory for every control block (`if`, `else`, `while`, etc.), regardless of size.
*   **Allman Style:** The opening brace MUST be on a new line.
*   **Naming:** 
    *   Functions/Variables: `lower_case_with_underscores`.
    *   Macros/Enums: `UPPER_CASE_WITH_UNDERSCORES`.
    *   Types: `cfn_sal_<name>_t`.
*   **Column Limit:** 120 columns.
*   **Fixed-Width Types:** Strictly use `stdint.h` types (`uint32_t`, `int16_t`, etc.). Do NOT use `int`, `long`, or `char`.

## 4. Error Handling & Events

*   **Return Codes:** Every API function MUST return `cfn_hal_error_code_t`.
*   **Error Offsetting:** Service-level errors start at `CFN_SAL_ERROR_BASE` (0x600) in `include/cfn_sal_types.h`.
*   **Status Splitting:** 
    *   `events`: Nominal occurrences (e.g., `DATA_READY`, `TX_COMPLETE`).
    *   `errors`: Exceptional/Failure conditions (e.g., `COMM_FAIL`, `OVERFLOW`).

## 5. Testing Requirements

All new service interfaces MUST have a corresponding unit test in `tests/`.
*   **The Big 4 Negative Tests:** Every test suite must verify:
    1.  `NullDriverReturnsBadParam`
    2.  `WrongPeripheralTypeReturnsBadParam`
    3.  `UnimplementedApiReturnsNotSupported`
    4.  `OnConfigFailureAbortsInit`
*   **Mock Verification:** Use GoogleTest to provide a local mock of the service API and verify that the `static inline` wrappers correctly dispatch calls to the VMT.

## 6. Directory Structure Mandates

*   `include/devices/`: Interfaces for physical components (LEDs, Sensors, Displays).
*   `include/network/`: Generic connectivity and protocol abstractions (Connection, Transport).
*   `include/utilities/`: Software-only services and data structures (CLI, Collections, AT Parsers, File System).
*   `include/cfn_sal.h`: Core macro definitions.
*   `include/cfn_sal_types.h`: Shared service-layer types.

## 7. Serialization Layer & Schemas
The serialization layer (`include/utilities/cfn_sal_serialization.h`) provides a generic way to encode/decode C structures into wire formats (JSON, Binary, etc.) without heap allocation.

### A. Defining a Schema
Use the provided macros to create a static schema descriptor. This descriptor allows the serialization driver to introspect your structure.

```c
typedef struct {
    int32_t temperature;
    float humidity;
    bool alarm_active;
} telemetry_t;

CFN_SAL_SCHEMA_START(telemetry_schema)
    CFN_SAL_SCHEMA_FIELD("temp",  CFN_SAL_SCHEMA_FIELD_TYPE_INT32, telemetry_t, temperature),
    CFN_SAL_SCHEMA_FIELD("hum",   CFN_SAL_SCHEMA_FIELD_TYPE_FLOAT, telemetry_t, humidity),
    CFN_SAL_SCHEMA_FIELD("alarm", CFN_SAL_SCHEMA_FIELD_TYPE_BOOL,  telemetry_t, alarm_active),
CFN_SAL_SCHEMA_END(telemetry_schema)
```

### B. Usage
Drivers are implementation-specific (e.g., JSON or CBOR) but share the same generic interface.

```c
size_t written = 0;
cfn_sal_serialization_encode(&json_driver, &telemetry_schema, &my_data, buffer, sizeof(buffer), &written);
```

## 8. Combination Sensor Support (Shared Context Pattern)
When a single physical IC provides multiple logical services (e.g., a BME280 providing Temperature, Humidity, and Pressure), we use the **Shared Context Pattern**.

### A. The Shared Context
A shared context structure must be defined in the implementation layer, with `cfn_sal_shared_ctx_t` as its FIRST member. This enables reference-counted initialization.

```c
typedef struct {
    cfn_sal_shared_ctx_t base;
    // IC specific calibration/state
} my_combo_ctx_t;
```

### B. Shared Lifecycle
All logical interfaces for the combo sensor share the same `init` and `deinit` implementation, using the `cfn_sal_shared_ctx_should_init()` and `cfn_sal_shared_ctx_should_deinit()` helpers. This ensures the physical bus/hardware is only touched during the first `init()` and last `deinit()`.

### C. Interface Decoupling
The user instantiates separate service drivers (e.g., `cfn_sal_temp_sensor_t` and `cfn_sal_hum_sensor_t`) but points their `phy->user_arg` to the same shared context instance. This maintains strict single-responsibility at the application level.

## 9. Contribution Workflow

1.  **Draft the Header:** Create the VMT and wrappers in the appropriate subdirectory.
2.  **Add Test:** Create `tests/cfn_sal_test_<name>.cpp` and implement the Big 4.
3.  **Validate Build:** Ensure the library compiles natively and all tests pass.
4.  **Analyze:** Run `make caffeine-sal-format` and `make caffeine-sal-analyze`.
