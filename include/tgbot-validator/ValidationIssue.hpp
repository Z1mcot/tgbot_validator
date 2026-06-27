//
// Created by ricz on 27.06.2026.
//

#ifndef TGBOT_VALIDATOR_VALIDATIONISSUE_HPP
#define TGBOT_VALIDATOR_VALIDATIONISSUE_HPP

#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

#include "ErrorCode.hpp"

namespace TgBot::validator {
    using nlohmann::json;

    /// Одна структурированная проблема (ошибка или предупреждение).
    struct ValidationIssue {
        ErrorCode code;

        /// основной стейт, к которому относится проблема (если есть)
        std::optional<std::string> state;

        /// дополнительные затронутые стейты (например, дубликаты)
        std::vector<std::string> related_states;

        /// человекочитаемое описание
        std::string message;

        /// произвольные доп. данные (target, expected/actual и т.д.)
        json details;
    };

    void to_json(json& j, const ValidationIssue& issue);
}

#endif //TGBOT_VALIDATOR_VALIDATIONISSUE_HPP
