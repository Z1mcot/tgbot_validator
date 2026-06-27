//
// Created by ricz on 27.06.2026.
//

#ifndef TGBOT_VALIDATOR_VALIDATIONRESULT_HPP
#define TGBOT_VALIDATOR_VALIDATIONRESULT_HPP

#include <nlohmann/json_fwd.hpp>
#include <vector>
#include "ValidationIssue.hpp"

namespace TgBot::validator {
    struct ValidationResult {
        bool is_valid = true;
        std::vector<ValidationIssue> errors;
        std::vector<ValidationIssue> warnings;
    };

    void to_json(json& j, const ValidationResult& result);
}

#endif //TGBOT_VALIDATOR_VALIDATIONRESULT_HPP
