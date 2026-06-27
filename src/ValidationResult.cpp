//
// Created by ricz on 27.06.2026.
//

#include "../include/tgbot-validator/ValidationResult.hpp"

namespace TgBot::validator {
    void to_json(json &j, const ValidationResult &result) {
        j = json{
            {"is_valid", result.is_valid},
            {"errors", result.errors},
            {"warnings", result.warnings}
        };
    }
}