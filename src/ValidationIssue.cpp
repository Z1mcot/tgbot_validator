//
// Created by ricz on 27.06.2026.
//

#include "../include/tgbot-validator/ValidationIssue.hpp"
namespace TgBot::validator {
    void to_json(json &j, const ValidationIssue &issue) {
        j = json{
            {"code", toString(issue.code)},
            {"state", issue.state ? json(*issue.state) : json(nullptr)},
            {"message", issue.message},
            {"details", issue.details}
        };
        if (!issue.related_states.empty()) {
            j["related_states"] = issue.related_states;
        } else {
            j["related_states"] = json::array();
        }
    }
}