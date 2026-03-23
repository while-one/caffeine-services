<p align="center">
  <a href="https://whileone.me">
    <img src="https://whileone.me/images/caffeine-small.png" alt="Caffeine Logo" width="384" height="384">
  </a>
</p>

# Caffeine-Services

<p align="center">
  <img src="https://img.shields.io/badge/C-11-blue.svg?style=flat-square&logo=c" alt="C11">
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat-square&logo=cmake&logoColor=white" alt="CMake">
  <a href="https://github.com/while-one/caffeine-services/tags">
    <img src="https://img.shields.io/github/v/tag/while-one/caffeine-services?style=flat-square&label=Release" alt="Latest Release">
  </a>
  <a href="https://github.com/while-one/caffeine-services/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/while-one/caffeine-services/ci.yml?style=flat-square&branch=main" alt="CI Status">
  </a>
  <a href="https://github.com/while-one/caffeine-services/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/while-one/caffeine-services?style=flat-square&color=blue" alt="License: MIT">
  </a>
</p>

**Caffeine-Services** is the polymorphic middleware layer of the **Caffeine Framework**. It provides a strictly C11-compliant, header-only, Virtual Method Table (VMT) architecture to define abstract interfaces for high-level hardware devices and software services.

Built directly on top of [**Caffeine-HAL**](https://github.com/while-one/caffeine-hal), this library ensures that business logic remains completely portable across different hardware implementations.

---

## Overview

The library decouples high-level application logic from concrete implementations. By providing generic interfaces like `Transport` or `Connection`, application code can interact with any communication link (UART, BLE, LoRaWAN, TCP) through a unified API.

### Key Features
*   **Header-Only Interface:** Zero-overhead abstractions using `static inline` wrappers.
*   **Homogeneous Architecture:** Inherits from the standard `cfn_hal_driver_t` container, ensuring a consistent lifecycle (`init`, `deinit`, `config`, `callback`) across the entire framework.
*   **Generic Networking:** Unified `Connection` and `Transport` (Sync/Async) abstractions for link-layer and data-stream management.
*   **Thread-Safe:** Designed for seamless integration with the `CFN_HAL_WITH_LOCK` mechanism.
*   **Safety First:** Exhaustive parameter validation and FourCC type checking at runtime.

---

## Directory Structure

*   `include/cfn_svc.h`: Core macros and FourCC definitions.
*   `include/devices/`: Hardware-agnostic interfaces for physical components.
    *   `led.h`, `button.h`, `accel.h`, `temp_sensor.h`, `hum_sensor.h`, `battery.h`, `light_sensor.h`, `pressure_sensor.h`, `gnss.h`, `display.h`.
*   `include/network/`: Generic connectivity and protocol abstractions.
    *   `connection.h` (Link-layer management), `transport.h` (Streaming/Datagram Data).
*   `include/utilities/`: High-level software services and data structures.
    *   `cli.h`, `at_parser.h`, `collection.h`, `fs.h` (File System), `logging.h`.

---

## Integration

### 1. CMake (FetchContent)

```cmake
include(FetchContent)
FetchContent_Declare(
    caffeine-services
    GIT_REPOSITORY https://github.com/while-one/caffeine-services.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(caffeine-services)

target_link_libraries(your_app PRIVATE caffeine::services)
```

---

## Development & Contribution

All contributions must adhere to the [**SKILL.md**](SKILL.md) guidelines:
*   Strict C11 (No C++ or GNU extensions).
*   No dynamic memory allocation.
*   Allman-style braces and 120-column limit.
*   Polymorphic VMT pattern for all complex interfaces.

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
