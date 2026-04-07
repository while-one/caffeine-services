cmake_minimum_required(VERSION 3.23)

project(caffeine-sal
        VERSION 0.1.9
        LANGUAGES C ASM
        DESCRIPTION "Header-only polymorphic middleware services layer for the Caffeine Framework"
        HOMEPAGE_URL "https://github.com/while-one/caffeine-sal"
)

# Global standards for the Caffeine Framework
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 3. Code Quality (Formatting & Analysis)
# Define the absolute universe of sources for this repository
file(GLOB_RECURSE ALL_UNIVERSE_SOURCES 
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
    "${PROJECT_SOURCE_DIR}/tests/*.c"
)

include(${PROJECT_SOURCE_DIR}/caffeine-build/cmake/CaffeineMacros.cmake)
cfn_add_universe_targets(INPUTS ${ALL_UNIVERSE_SOURCES})

# Useful for IDEs, clang-tidy, and clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

# --- Dependency: Caffeine-HAL ---
include(FetchContent)
FetchContent_Declare(
    caffeine-hal
    GIT_REPOSITORY https://github.com/while-one/caffeine-hal.git
    GIT_TAG        v0.5.8

)

# Ensure the mock library is available for tests
if(CFN_SAL_BUILD_TESTS)
    set(CFN_HAL_PROVIDE_MOCK ON)
endif()

FetchContent_MakeAvailable(caffeine-hal)

# --- Component Configuration ---
add_library(${PROJECT_NAME} INTERFACE)
add_library(caffeine::sal ALIAS ${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} INTERFACE ${CAFFEINE_COMPILE_OPTIONS})

# Consistent naming for exported target
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME services)

target_link_libraries(${PROJECT_NAME} INTERFACE caffeine::hal)

# Gather all headers for target definition and tooling
file(GLOB_RECURSE PROJECT_ALL_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

target_sources(
        ${PROJECT_NAME}
        INTERFACE
        FILE_SET headers
        TYPE HEADERS
        BASE_DIRS include
        FILES ${PROJECT_ALL_HEADERS}
)

# --- Testing ---
option(CFN_BUILD_TESTS "Build the Caffeine-Services test executable" OFF)

if(PROJECT_IS_TOP_LEVEL AND CFN_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()


# --- Installation ---
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Prepare the version information
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

# Configure the input and output of the config file that will be generated
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Store the ${PROJECT_NAME}_Targets generated previously in the config file
install(
        EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE caffeine::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Copy the generic config file and the version config file to the installation destination
install(
        FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# ==============================================================================
# Tooling: Formatting & Analysis
# ==============================================================================

# Gather sources for formatting and analysis (C headers only for this interface repo)
set(ALL_HEADERS ${PROJECT_ALL_HEADERS})
set(ALL_ANALYSIS_SOURCES ${ALL_HEADERS})

cfn_add_code_quality_targets(
    ${PROJECT_NAME}
    HEADERS_ONLY
    ANALYSIS_SOURCES ${ALL_HEADERS}
    CPPCHECK_ARGS -I ${PROJECT_SOURCE_DIR}/include -I ${caffeine-hal_SOURCE_DIR}/include
    TIDY_ARGS -I${PROJECT_SOURCE_DIR}/include -I${caffeine-hal_SOURCE_DIR}/include
    TIDY_HEADER_FILTER "${PROJECT_SOURCE_DIR}/include/.*"
)

# 4. C11 Compliance Check
if(NOT CMAKE_CROSSCOMPILING)
    add_executable(${PROJECT_NAME}-compliance ${PROJECT_SOURCE_DIR}/tests/compliance.c)
    target_link_libraries(${PROJECT_NAME}-compliance PRIVATE ${PROJECT_NAME})
    target_compile_options(${PROJECT_NAME}-compliance PRIVATE
        -std=c11
        -pedantic-errors
        -Wall
        -Wextra
        -Werror
    )
    set_target_properties(${PROJECT_NAME}-compliance PROPERTIES EXCLUDE_FROM_ALL ON)

    if(TARGET ${PROJECT_NAME}-analyze)
        add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-compliance)
    endif()
endif()

# 5. Doxygen Documentation
set(ALL_UNIVERSE_SOURCES_NO_TESTS ${ALL_UNIVERSE_SOURCES})
list(FILTER ALL_UNIVERSE_SOURCES_NO_TESTS EXCLUDE REGEX ".*/tests/.*")
cfn_add_docs(
    ${PROJECT_NAME}-docs
    INPUTS ${ALL_UNIVERSE_SOURCES_NO_TESTS} ${PROJECT_SOURCE_DIR}/README.md
    COMMENT "Generating API documentation with Doxygen"
)
