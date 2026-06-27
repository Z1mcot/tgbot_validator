# Включаем интеграцию макросов GTest в CMake
include(GoogleTest)

# Создаем тестовый бинарник
add_executable(tgbot_validator_tests
        smoke_test.cpp
        # Сюда дописывай новые файлы тестов по мере появления
)

# Линкуем ядро валидатора и gtest_main (он сам добавит функцию main для тестов)
target_link_libraries(tgbot_validator_tests
        PRIVATE
        tgbot_validator_core
        GTest::gtest_main
)

# Автоматически регистрируем тесты в утилите ctest
gtest_discover_tests(tgbot_validator_tests)