// state_machine_validator.cpp
// Валидатор графа конечного автомата (FSM) для структуры Telegram-бота.
// Стандарт: C++20. Зависимость: nlohmann/json (single header).

#include <nlohmann/json.hpp>

#include <iostream>
#include <string>

#include "../include/tgbot-validator/StateMachineValidator.hpp"


using nlohmann::json;

// ---------------------------------------------------------------------------
// CLI: читает JSON графа из stdin, печатает ValidationResult в stdout как JSON.
// Exit code: 0 — граф валиден, 1 — есть ошибки.

int main() {
    std::string input(
        (std::istreambuf_iterator<char>(std::cin)),
        std::istreambuf_iterator<char>());

    TgBot::validator::StateMachineValidator validator;
    TgBot::validator::ValidationResult result = validator.validate(input);

    std::cout << json(result).dump(2) << "\n";

    return result.is_valid ? 0 : 1;
}