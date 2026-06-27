cmake_minimum_required(VERSION 4.2)
project(tgbot_validator_core
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "Validator for tgbot-framework state-machine"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Для Windows: предотвращает конфликты с флагами рантайма MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(COMPILE_BINARY 1)

add_library(tgbot_validator_core
        include/tgbot-validator/ErrorCode.hpp
        src/ErrorCode.cpp
        include/tgbot-validator/ValidationIssue.hpp
        include/tgbot-validator/ValidationResult.hpp
        include/tgbot-validator/StateNode.hpp
        include/tgbot-validator/StateMachineValidator.hpp
        src/StateMachineValidator.cpp
        src/ValidationIssue.cpp
        src/ValidationResult.cpp)

target_include_directories(tgbot_validator_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ============================================================================
# 1. Хитрая линковка: разделяем FetchContent и имя внешнего пакета
# ============================================================================
target_link_libraries(tgbot_validator_core PUBLIC
        $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
        $<INSTALL_INTERFACE:nlohmann_json> # Убираем двоеточия! Для экспорта это будет просто строка-требование
)

if(COMPILE_BINARY)
    add_executable(tgbot_validator src/main.cpp)
    target_link_libraries(tgbot_validator PRIVATE nlohmann_json::nlohmann_json tgbot_validator_core)
    install(TARGETS tgbot_validator EXPORT ${PROJECT_NAME}-targets RUNTIME DESTINATION bin)
endif()

enable_testing()
add_subdirectory(tests)

# ============================================================================
# 2. Раскомментированный и полностью рабочий инсталл
# ============================================================================
#install(TARGETS tgbot_validator_core
#        EXPORT ${PROJECT_NAME}-targets
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib
#        INCLUDES DESTINATION include
#)
#
#install(DIRECTORY include/
#        DESTINATION include
#        FILES_MATCHING PATTERN "*.hpp"
#)
#
## Чтобы у пользователя автоматически подтягивался find_package(nlohmann_json),
## мы пишем специальный хелпер, который добавит эту команду в сгенерированный Config.cmake
#include(CMakePackageConfigHelpers)
#
#install(EXPORT ${PROJECT_NAME}-targets
#        FILE ${PROJECT_NAME}Config.cmake
#        NAMESPACE ${PROJECT_NAME}::
#        DESTINATION lib/cmake/${PROJECT_NAME}
#)