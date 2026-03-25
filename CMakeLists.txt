cmake_minimum_required(VERSION 3.23)

project(caffeine-sal
        VERSION 0.1.1
        LANGUAGES C ASM
        DESCRIPTION "Header-only polymorphic middleware services layer for the Caffeine Framework"
        HOMEPAGE_URL "https://github.com/while-one/caffeine-sal"
)

# Global standards for the Caffeine Framework
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(${PROJECT_SOURCE_DIR}/caffeine-build/cmake/CaffeineMacros.cmake OPTIONAL)

# Useful for IDEs, clang-tidy, and clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

# --- Dependency: Caffeine-HAL ---
include(FetchContent)
FetchContent_Declare(
    caffeine-hal
    GIT_REPOSITORY https://github.com/while-one/caffeine-hal.git
    GIT_TAG        v0.4.12
)
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
option(CFN_SAL_BUILD_TESTS "Build the Caffeine-Services test executable" OFF)

if(PROJECT_IS_TOP_LEVEL AND CFN_SAL_BUILD_TESTS)
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

# Gather sources for formatting and analysis
set(ALL_HEADERS ${PROJECT_ALL_HEADERS})
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
set(ALL_ANALYSIS_SOURCES ${ALL_HEADERS} ${TEST_SOURCES})

# 1. Code Formatting (clang-format)
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    option(FORMAT_DRY_RUN "Run clang-format in dry-run mode" OFF)
    
    if(FORMAT_DRY_RUN)
        set(FORMAT_ARGS --dry-run --Werror)
        set(FORMAT_COMMENT "Checking header formatting with clang-format (BARR-C style)...")
    else()
        set(FORMAT_ARGS -i)
        set(FORMAT_COMMENT "Formatting header files with clang-format (BARR-C style)...")
    endif()

    add_custom_target(
        ${PROJECT_NAME}-format
        COMMAND ${CLANG_FORMAT}
        ${FORMAT_ARGS}
        -style=file:${PROJECT_SOURCE_DIR}/caffeine-build/config/coding/.clang-format
        ${ALL_ANALYSIS_SOURCES}
        COMMENT "${FORMAT_COMMENT}"
    )
else()
    message(WARNING "clang-format not found.")
endif()

# 2. Static Analysis (cppcheck)
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(
        ${PROJECT_NAME}-cppcheck
        COMMAND ${CPPCHECK}
        --enable=all
        --suppress=missingIncludeSystem
        --suppress=unusedFunction
        --suppress=*:*/_deps/*
        --inline-suppr
        --std=c11
        -I ${PROJECT_SOURCE_DIR}/include
        -I ${caffeine-hal_SOURCE_DIR}/include
        ${ALL_HEADERS}
        COMMENT "Running cppcheck static analysis..."
    )
    if(NOT TARGET ${PROJECT_NAME}-analyze)
        add_custom_target(${PROJECT_NAME}-analyze COMMENT "Running full static analysis suite...")
    endif()
    add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-cppcheck)
endif()

# 3. Static Analysis (clang-tidy)
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    add_custom_target(
        ${PROJECT_NAME}-tidy
        COMMAND ${CLANG_TIDY}
        --config-file=${PROJECT_SOURCE_DIR}/caffeine-build/config/coding/.clang-tidy
        --header-filter="${PROJECT_SOURCE_DIR}/include/.*"
        ${ALL_HEADERS}
        ${PROJECT_SOURCE_DIR}/tests/compliance.c
        --
        -I${PROJECT_SOURCE_DIR}/include
        -I${caffeine-hal_SOURCE_DIR}/include
        -std=c11
        COMMENT "Running clang-tidy static analysis..."
    )
    
    if(TARGET ${PROJECT_NAME}-analyze)
        add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-tidy)
    endif()
endif()

# 4. C11 Compliance Check
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
