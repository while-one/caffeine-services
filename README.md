<p align="center">
  <a href="https://whileone.me">
    <img src="https://raw.githubusercontent.com/while-one/caffeine-build/main/assets/logo.png" alt="Caffeine Logo" width="50%">
  </a>
<h1 align="center">The Caffeine Framework</h1>
</p>

# Caffeine-SAL

<p align="center">
  <img src="https://img.shields.io/badge/C-11-blue.svg?style=flat-square&logo=c" alt="C11">
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat-square&logo=cmake&logoColor=white" alt="CMake">
  <a href="https://github.com/while-one/caffeine-sal/tags">
    <img src="https://img.shields.io/github/v/tag/while-one/caffeine-sal?style=flat-square&label=Release" alt="Latest Release">
  </a>
  <a href="https://github.com/while-one/caffeine-sal/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/while-one/caffeine-sal/ci.yml?style=flat-square&branch=main" alt="CI Status">
  </a>
  <a href="https://github.com/while-one/caffeine-sal/commits/main">
    <img src="https://img.shields.io/github/last-commit/while-one/caffeine-sal.svg?style=flat-square" alt="Last Commit">
  </a>
  <a href="https://github.com/while-one/caffeine-sal/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/while-one/caffeine-sal?style=flat-square&color=blue" alt="License: MIT">
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

*   `include/cfn_sal.h`: Core macros and FourCC definitions.
*   `include/devices/`: Hardware-agnostic interfaces for physical components.
    *   `led.h`, `button.h`, `accel.h`, `temp_sensor.h`, `hum_sensor.h`, `battery.h`, `light_sensor.h`, `pressure_sensor.h`, `gnss.h`, `display.h`.
*   `include/network/`: Generic connectivity and protocol abstractions.
    *   `connection.h` (Link-layer management), `transport.h` (Streaming/Datagram Data).
*   `include/utilities/`: High-level software services and data structures.
    *   `cli.h`, `at_parser.h`, `collection.h`, `fs.h` (File System), `logging.h`.

---

## Development & Analysis

The project includes built-in targets for maintaining code quality. Use the framework's centralized scripts for orchestration:

```bash
# Full Quality Gate (Format -> Analyze -> Build -> Test)
./caffeine-build/scripts/ci.sh all

# Run only unit tests
./caffeine-build/scripts/ci.sh test unit-tests-gtest
```

---

## Integration

### 1. CMake (FetchContent)

```cmake
include(FetchContent)
FetchContent_Declare(
    caffeine-sal
    GIT_REPOSITORY https://github.com/while-one/caffeine-sal.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(caffeine-sal)

target_link_libraries(your_app PRIVATE caffeine::sal)
```

---

## Development & Contribution

All contributions must adhere to the core framework guidelines:
*   Strict C11 (No C++ or GNU extensions).
*   No dynamic memory allocation.
*   Allman-style braces and 120-column limit.
*   Polymorphic VMT pattern for all complex interfaces.

---

## Support the Gallery

While this library is no Mondrian, it deals with a different form of **abstraction art**. Hardware abstraction is a craft of its own—one that keeps your application code portable and your debugging sessions short.

Whether Caffeine is fueling an elegant embedded project or just helping you wake up your hardware, you can contribute in the following ways:

* Star & Share: If you find this project useful, give it a ⭐ on GitHub and share it with your fellow firmware engineers. It helps others find the library and grows the Caffeine community.
* Show & Tell: If you are using Caffeine in a project (personal or professional), let me know! Hearing how it's being used is a huge motivator.
* Propose Features: If the library is missing a specific "brushstroke," let's design the interface together.
* Port New Targets: Help us expand the collection by porting the HAL to new silicon or peripheral sets.
* Expand the HIL Lab: Contributions go primarily toward acquiring new development boards. These serve as dedicated Hardware-in-the-Loop test targets, ensuring every commit remains rock-solid across our entire fleet of supported hardware.

**If my projects helped you, feel free to buy me a brew. Or if it caused you an extra debugging session, open a PR!**

<a href="https://www.buymeacoffee.com/whileone" target="_blank">
  <img src="https://img.shields.io/badge/Caffeine%20me--0077ff?style=for-the-badge&logo=buy-me-a-coffee&logoColor=white" 
       height="40" 
       style="border-radius: 5px;">
</a>&nbsp;&nbsp;&nbsp;&nbsp;
<a href="https://github.com/sponsors/while-one" target="_blank">
<img src="https://img.shields.io/badge/Sponsor--ea4aaa?style=for-the-badge&logo=github-sponsors" height="40" style="border-radius: 5px;"> </a>&nbsp;&nbsp;&nbsp;
<a href="https://github.com/while-one/caffeine-sal/compare" target="_blank">
<img src="https://img.shields.io/badge/Open%20a%20PR--orange?style=for-the-badge&logo=github&logoColor=white" height="40" style="border-radius: 5px;">
</a>

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
