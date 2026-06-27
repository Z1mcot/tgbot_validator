//
// Created by ricz on 27.06.2026.
//

#ifndef TGBOT_VALIDATOR_ERROR_CODE_HPP
#define TGBOT_VALIDATOR_ERROR_CODE_HPP

#include <string>

namespace TgBot::validator {
    /// Типичные ошибки в JSON'e бота
    enum class ErrorCode {
        /// вход не парсится как JSON
        InvalidJson,

        /// нет поля init_state / неверный тип
        MissingInitState,

        /// нет поля states / неверный тип
        MissingStates,

        /// элемент states без корректного поля name
        MissingStateName,

        /// init_state не совпадает ни с одним name
        InitStateNotFound,

        /// имя состояния встречается более одного раза
        DuplicateStateName,

        /// keyboard_texts.size() != keyboard_transitions.size()
        KeyboardSizeMismatch,

        /// переход ссылается на несуществующее состояние
        DanglingTransition,

        /// состояние недостижимо из init_state (warning)
        UnreachableState,

        /// состояние без переходов (warning)
        DeadEnd,
    };

    std::string toString(ErrorCode code);
}

#endif //TGBOT_VALIDATOR_ERROR_CODE_HPP
