cmake_minimum_required(VERSION 3.25)
project(tgbot_validator_core
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "Validator for tgbot-framework state-machine"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

#set(COMPILE_BINARY 1)

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

target_link_libraries(tgbot_validator_core PUBLIC nlohmann_json::nlohmann_json)

if(COMPILE_BINARY)
    add_executable(tgbot_validator src/main.cpp)
    target_link_libraries(tgbot_validator PRIVATE tgbot_validator_core)

    install(TARGETS tgbot_validator EXPORT ${PROJECT_NAME}-targets RUNTIME DESTINATION bin)
endif()

enable_testing()
add_subdirectory(tests)

install(TARGETS tgbot_validator_core
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
)

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
)

include(CMakePackageConfigHelpers)

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
)