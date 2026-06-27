//
// Created by ricz on 27.06.2026.
//

#ifndef TGBOT_VALIDATOR_STATENODE_HPP
#define TGBOT_VALIDATOR_STATENODE_HPP

#include <string>
#include <vector>

namespace TgBot::validator {
    /// Описание одного состояния графа после парсинга JSON.
    struct StateNode {
        std::string name;
        std::vector<std::string> keyboard_texts;
        std::vector<std::string> keyboard_transitions;
    };
}

#endif //TGBOT_VALIDATOR_STATENODE_HPP
