//
// Created by ricz on 27.06.2026.
//

#include "../include/tgbot-validator/StateMachineValidator.hpp"

#include <queue>
#include <optional>
#include <unordered_set>
#include "../include/tgbot-validator/StateNode.hpp"

namespace TgBot::validator {
    ValidationResult StateMachineValidator::validate(const std::string &json_str) {
        ValidationResult result;

        json root;
        try {
            root = json::parse(json_str);
        } catch (const json::parse_error& e) {
            result.is_valid = false;
            result.errors.push_back(ValidationIssue{
                .code = ErrorCode::InvalidJson,
                .state = std::nullopt,
                .message = std::string("Невалидный JSON: ") + e.what()
            });
            return result;
        }

        states_.clear();
        order_.clear();
        init_state_.clear();

        if (!parseRoot(root, result)) {
            result.is_valid = false;
            return result;
        }

        checkStructure(result);
        checkKeyboardConsistency(result);
        checkDanglingTransitions(result);

        // Обход графа возможен только если init_state валиден и существует.
        if (states_.contains(init_state_)) {
            checkReachability(result);
        }
        checkDeadEnds(result);

        result.is_valid = result.errors.empty();
        return result;
    }

    bool StateMachineValidator::parseRoot(const json &root, ValidationResult &result) {
        if (!root.contains("init_state") || !root["init_state"].is_string()) {
            result.errors.push_back(ValidationIssue{
                .code = ErrorCode::MissingInitState,
                .state = std::nullopt,
                .message = "Отсутствует или некорректно поле 'init_state'."
            });
            return false;
        }
        init_state_ = root["init_state"].get<std::string>();

        if (!root.contains("states") || !root["states"].is_array()) {
            result.errors.push_back(ValidationIssue{
                .code = ErrorCode::MissingStates,
                .state = std::nullopt,
                .message = "Отсутствует или некорректно поле 'states'."
            });
            return false;
        }

        int unnamed_index = 0;
        for (const auto& s : root["states"]) {
            StateNode node;

            if (!s.contains("name") || !s["name"].is_string()) {
                ValidationIssue issue{
                    .code = ErrorCode::MissingStateName,
                    .state = std::nullopt,
                    .message = "Найден стейт без корректного поля 'name' (индекс в массиве states: " +
                               std::to_string(unnamed_index) + ")."
                };
                issue.details["index"] = unnamed_index;
                result.errors.push_back(std::move(issue));
                ++unnamed_index;
                continue;
            }
            ++unnamed_index;
            node.name = s["name"].get<std::string>();

            if (s.contains("keyboard_texts") && s["keyboard_texts"].is_array()) {
                for (const auto& t : s["keyboard_texts"]) {
                    node.keyboard_texts.push_back(t.is_string() ? t.get<std::string>() : t.dump());
                }
            }
            if (s.contains("keyboard_transitions") && s["keyboard_transitions"].is_array()) {
                for (const auto& t : s["keyboard_transitions"]) {
                    node.keyboard_transitions.push_back(t.is_string() ? t.get<std::string>() : t.dump());
                }
            }

            // Дубликаты имен фиксируются в checkStructure(); здесь просто не перезатираем первую запись.
            if (!states_.contains(node.name)) {
                order_.push_back(node.name);
                states_.emplace(node.name, std::move(node));
            } else {
                order_.push_back(node.name);
                states_[node.name] = std::move(node); // последний экземпляр побеждает для дальнейших проверок
            }
        }
        return true;
    }

    void StateMachineValidator::checkStructure(ValidationResult &result) {
        if (!states_.contains(init_state_)) {
            ValidationIssue issue{
                .code = ErrorCode::InitStateNotFound,
                .state = init_state_,
                .message = "init_state '" + init_state_ + "' не найден среди состояний в 'states'."
            };
            result.errors.push_back(std::move(issue));
        }

        std::unordered_map<std::string, int> counts;
        for (const auto& name : order_) {
            counts[name]++;
        }
        for (const auto& [name, count] : counts) {
            if (count > 1) {
                ValidationIssue issue{
                    .code = ErrorCode::DuplicateStateName,
                    .state = name,
                    .message = "Дублирующееся имя состояния: '" + name + "' встречается " +
                               std::to_string(count) + " раз(а)."
                };
                issue.details["occurrences"] = count;
                result.errors.push_back(std::move(issue));
            }
        }
    }

    void StateMachineValidator::checkKeyboardConsistency(ValidationResult &result) {
        for (const auto& [name, node] : states_) {
            if (node.keyboard_texts.size() != node.keyboard_transitions.size()) {
                ValidationIssue issue{
                    .code = ErrorCode::KeyboardSizeMismatch,
                    .state = name,
                    .message = "Состояние '" + name + "': размер keyboard_texts (" +
                               std::to_string(node.keyboard_texts.size()) +
                               ") не совпадает с размером keyboard_transitions (" +
                               std::to_string(node.keyboard_transitions.size()) + ")."
                };
                issue.details["keyboard_texts_size"] = node.keyboard_texts.size();
                issue.details["keyboard_transitions_size"] = node.keyboard_transitions.size();
                result.errors.push_back(std::move(issue));
            }
        }
    }

    void StateMachineValidator::checkDanglingTransitions(ValidationResult &result) {
        for (const auto& [name, node] : states_) {
            for (const auto& target : node.keyboard_transitions) {
                if (!states_.contains(target)) {
                    ValidationIssue issue{
                        .code = ErrorCode::DanglingTransition,
                        .state = name,
                        .related_states = {target},
                        .message = "Состояние '" + name + "' ссылается на несуществующее состояние '" +
                                   target + "'."
                    };
                    issue.details["target"] = target;
                    result.errors.push_back(std::move(issue));
                }
            }
        }
    }

    void StateMachineValidator::checkReachability(ValidationResult &result) {
        std::unordered_set<std::string> visited;
        std::queue<std::string> queue;

        queue.push(init_state_);
        visited.insert(init_state_);

        while (!queue.empty()) {
            std::string current = queue.front();
            queue.pop();

            auto it = states_.find(current);
            if (it == states_.end()) continue; // несуществующая цель — уже отмечена как error

            for (const auto& target : it->second.keyboard_transitions) {
                if (states_.contains(target) && visited.insert(target).second) {
                    queue.push(target);
                }
            }
        }

        for (const auto& name : order_) {
            if (!visited.contains(name)) {
                ValidationIssue issue{
                    .code = ErrorCode::UnreachableState,
                    .state = name,
                    .message = "Состояние '" + name + "' недостижимо из init_state ('" + init_state_ + "')."
                };
                issue.details["init_state"] = init_state_;
                result.warnings.push_back(std::move(issue));
            }
        }
    }

    void StateMachineValidator::checkDeadEnds(ValidationResult &result) {
        for (const auto& name : order_) {
            const auto& node = states_.at(name);
            if (node.keyboard_transitions.empty()) {
                ValidationIssue issue{
                    .code = ErrorCode::DeadEnd,
                    .state = name,
                    .message = "Состояние '" + name + "' не имеет переходов (keyboard_transitions пуст) — "
                               "потенциальный тупик для пользователя."
                };
                result.warnings.push_back(std::move(issue));
            }
        }
    }
}
