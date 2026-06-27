cmake_minimum_required(VERSION 4.2)
project(tgbot_validator
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

set(COMPILE_BINARY 1)

if(COMPILE_BINARY)
    add_executable(tgbot_validator src/main.cpp
            include/tgbot-validator/ErrorCode.hpp
            src/ErrorCode.cpp
            include/tgbot-validator/ValidationIssue.hpp
            include/tgbot-validator/ValidationResult.hpp
            include/tgbot-validator/StateNode.hpp
            include/tgbot-validator/StateMachineValidator.hpp
            src/StateMachineValidator.cpp
            src/ValidationIssue.cpp
            src/ValidationResult.cpp)
else()
    add_library(tgbot_validator src/main.cpp
            include/tgbot-validator/ErrorCode.hpp
            src/ErrorCode.cpp
            include/tgbot-validator/ValidationIssue.hpp
            include/tgbot-validator/ValidationResult.hpp
            include/tgbot-validator/StateNode.hpp
            include/tgbot-validator/StateMachineValidator.hpp
            src/StateMachineValidator.cpp
            src/ValidationIssue.cpp
            src/ValidationResult.cpp)
endif()
target_link_libraries(tgbot_validator PRIVATE nlohmann_json::nlohmann_json)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
)
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
)