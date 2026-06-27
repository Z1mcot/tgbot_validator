//
// Created by ricz on 27.06.2026.
//

#ifndef TGBOT_VALIDATOR_STATEMACHINEVALIDATOR_HPP
#define TGBOT_VALIDATOR_STATEMACHINEVALIDATOR_HPP

#include <string>
#include <unordered_map>

#include "StateNode.hpp"
#include "ValidationResult.hpp"

namespace TgBot::validator {
    class StateMachineValidator {
    public:
        ValidationResult validate(const std::string& json_str);

    private:
        std::unordered_map<std::string, StateNode> states_;
        std::vector<std::string> order_; // сохраняем порядок появления состояний во входных данных
        std::string init_state_;

        /// Разбор верхнего уровня JSON в states_ / init_state_.
        /// Возвращает false при отсутствии обязательных полей или некорректном типе,
        /// что делает дальнейшую валидацию невозможной.
        bool parseRoot(const json& root, ValidationResult& result);

        /// Базовая валидация: init_state существует, имена состояний уникальны.
        void checkStructure(ValidationResult& result);

        /// Размер keyboard_texts должен строго совпадать с keyboard_transitions.
        void checkKeyboardConsistency(ValidationResult& result);

        /// Все цели переходов должны существовать среди известных состояний.
        void checkDanglingTransitions(ValidationResult& result);

        /// BFS от init_state: все недостижимые состояния — warning.
        void checkReachability(ValidationResult& result);

        /// Состояние без переходов — потенциальный тупик для пользователя.
        void checkDeadEnds(ValidationResult& result);
    };
}

#endif //TGBOT_VALIDATOR_STATEMACHINEVALIDATOR_HPP
