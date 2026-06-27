//
// Created by ricz on 27.06.2026.
//

#ifndef TGBOT_VALIDATOR_SMOKE_TEST_HPP
#define TGBOT_VALIDATOR_SMOKE_TEST_HPP

#include <algorithm>
#include <gtest/gtest.h>
#include "../include/tgbot-validator/StateMachineValidator.hpp"

using namespace TgBot::validator;

// Вспомогательный класс-фиxture для тестов
class StateMachineValidatorTest : public ::testing::Test {
protected:
    StateMachineValidator validator;

    // Хелпер для поиска ошибки по коду
    bool hasError(const ValidationResult& result, ErrorCode code) {
        return std::any_of(result.errors.begin(), result.errors.end(),
            [code](const ValidationIssue& issue) { return issue.code == code; });
    }

    // Хелпер для поиска варнинга по коду
    bool hasWarning(const ValidationResult& result, ErrorCode code) {
        return std::any_of(result.warnings.begin(), result.warnings.end(),
            [code](const ValidationIssue& issue) { return issue.code == code; });
    }
};

// 1. Тест успешной валидации корректного графа
TEST_F(StateMachineValidatorTest, ValidGraphSuccess) {
    std::string valid_json = R"({
        "init_state": "main_menu",
        "states": [
            {
                "name": "main_menu",
                "keyboard_texts": ["О нас", "Помощь"],
                "keyboard_transitions": ["about", "help"]
            },
            {
                "name": "about",
                "keyboard_texts": ["Назад"],
                "keyboard_transitions": ["main_menu"]
            },
            {
                "name": "help",
                "keyboard_texts": ["Назад"],
                "keyboard_transitions": ["main_menu"]
            }
        ]
    })";

    auto result = validator.validate(valid_json);

    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
    EXPECT_TRUE(result.warnings.empty());
}

// 2. Тест на полностью невалидный JSON синтаксис
TEST_F(StateMachineValidatorTest, InvalidJsonSyntax) {
    std::string broken_json = R"({ "init_state": "main", "states": [ )"; // Незакрытые скобки

    auto result = validator.validate(broken_json);

    EXPECT_FALSE(result.is_valid);
    ASSERT_FALSE(result.errors.empty());
    EXPECT_TRUE(hasError(result, ErrorCode::InvalidJson));
}

// 3. Тест на отсутствие обязательного поля init_state
TEST_F(StateMachineValidatorTest, MissingInitStateField) {
    std::string no_init = R"({
        "states": [
            { "name": "main", "keyboard_texts": [], "keyboard_transitions": [] }
        ]
    })";

    auto result = validator.validate(no_init);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::MissingInitState));
}

// 4. Тест на отсутствие обязательного массива states
TEST_F(StateMachineValidatorTest, MissingStatesField) {
    std::string no_states = R"({
        "init_state": "main"
    })";

    auto result = validator.validate(no_states);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::MissingStates));
}

// 5. Тест на элемент внутри states без поля "name"
TEST_F(StateMachineValidatorTest, MissingStateName) {
    std::string missing_name = R"({
        "init_state": "main",
        "states": [
            { "keyboard_texts": [], "keyboard_transitions": [] }
        ]
    })";

    auto result = validator.validate(missing_name);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::MissingStateName));
}

// 6. Тест: начальное состояние (init_state) не объявлено в списке состояний
TEST_F(StateMachineValidatorTest, InitStateNotFoundInList) {
    std::string unknown_init = R"({
        "init_state": "start_state",
        "states": [
            { "name": "main", "keyboard_texts": [], "keyboard_transitions": [] }
        ]
    })";

    auto result = validator.validate(unknown_init);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::InitStateNotFound));
}

// 7. Тест на дублирование имен состояний
TEST_F(StateMachineValidatorTest, DuplicateStateNamesDetected) {
    std::string duplicates = R"({
        "init_state": "main",
        "states": [
            { "name": "main", "keyboard_texts": [], "keyboard_transitions": [] },
            { "name": "about", "keyboard_texts": [], "keyboard_transitions": [] },
            { "name": "main", "keyboard_texts": [], "keyboard_transitions": [] }
        ]
    })";

    auto result = validator.validate(duplicates);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::DuplicateStateName));

    // Проверим, что в деталях сохранилось правильное количество повторений
    auto it = std::find_if(result.errors.begin(), result.errors.end(),
        [](const ValidationIssue& issue) { return issue.code == ErrorCode::DuplicateStateName; });

    ASSERT_NE(it, result.errors.end());
    EXPECT_EQ(it->state, "main");
    EXPECT_EQ(it->details["occurrences"], 2);
}

// 8. Тест: Рассинхронизация размеров массивов текстов кнопок и переходов
TEST_F(StateMachineValidatorTest, KeyboardSizeMismatchError) {
    std::string mismatch = R"({
        "init_state": "main",
        "states": [
            {
                "name": "main",
                "keyboard_texts": ["Кнопка 1", "Кнопка 2"],
                "keyboard_transitions": ["next_state"]
            },
            { "name": "next_state", "keyboard_texts": [], "keyboard_transitions": [] }
        ]
    })";

    auto result = validator.validate(mismatch);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::KeyboardSizeMismatch));
}

// 9. Тест на висячие переходы (ссылка на несуществующий стейт)
TEST_F(StateMachineValidatorTest, DanglingTransitionError) {
    std::string dangling = R"({
        "init_state": "main",
        "states": [
            {
                "name": "main",
                "keyboard_texts": ["Перейти"],
                "keyboard_transitions": ["ghost_state"]
            }
        ]
    })";

    auto result = validator.validate(dangling);

    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(hasError(result, ErrorCode::DanglingTransition));
}

// 10. Тест на предупреждение о недостижимом состоянии (Unreachable State)
TEST_F(StateMachineValidatorTest, UnreachableStateWarning) {
    std::string unreachable = R"({
        "init_state": "main",
        "states": [
            { "name": "main", "keyboard_texts": [], "keyboard_transitions": [] },
            { "name": "isolated_island", "keyboard_texts": [], "keyboard_transitions": [] }
        ]
    })";

    auto result = validator.validate(unreachable);

    // Поскольку недостижимость — это warning, общий флаг зависит от наличия ошибок.
    // В данном случае ошибок нет, но граф содержит предупреждения.
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
    EXPECT_TRUE(hasWarning(result, ErrorCode::UnreachableState));
}

// 11. Тест на предупреждение о тупиковом состоянии (Dead End)
TEST_F(StateMachineValidatorTest, DeadEndWarning) {
    std::string dead_end = R"({
        "init_state": "main",
        "states": [
            {
                "name": "main",
                "keyboard_texts": ["В тупик"],
                "keyboard_transitions": ["finish"]
            },
            {
                "name": "finish",
                "keyboard_texts": [],
                "keyboard_transitions": []
            }
        ]
    })";

    auto result = validator.validate(dead_end);

    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(hasWarning(result, ErrorCode::DeadEnd));
}

#endif //TGBOT_VALIDATOR_SMOKE_TEST_HPP
