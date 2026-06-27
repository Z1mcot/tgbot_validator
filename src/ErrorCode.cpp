//
// Created by ricz on 27.06.2026.
//

#include "../include/tgbot-validator/ErrorCode.hpp"

namespace TgBot::validator {
    std::string toString(const ErrorCode code) {
        switch (code) {
            case ErrorCode::InvalidJson:          return "INVALID_JSON";
            case ErrorCode::MissingInitState:     return "MISSING_INIT_STATE";
            case ErrorCode::MissingStates:        return "MISSING_STATES";
            case ErrorCode::MissingStateName:     return "MISSING_STATE_NAME";
            case ErrorCode::InitStateNotFound:    return "INIT_STATE_NOT_FOUND";
            case ErrorCode::DuplicateStateName:   return "DUPLICATE_STATE_NAME";
            case ErrorCode::KeyboardSizeMismatch: return "KEYBOARD_SIZE_MISMATCH";
            case ErrorCode::DanglingTransition:   return "DANGLING_TRANSITION";
            case ErrorCode::UnreachableState:     return "UNREACHABLE_STATE";
            case ErrorCode::DeadEnd:              return "DEAD_END";
        }
        return "UNKNOWN";
    }
}