#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

enum MessageType {
    PLAYER_INTERACT = 0,
    FEED_SUCCESSFUL,
    TARGETED,
    NOT_TARGETED,
    SLEEP,
    END_SLEEP,
    RANDOM_TRAVEL
};

inline std::string msgToString(int msg) {
    switch (msg) {
    case PLAYER_INTERACT:
        return "Player interact";
    case FEED_SUCCESSFUL:
        return "Player fed successfully";
    case TARGETED:
        return "Actor has been targeted";
    case NOT_TARGETED:
        return "Actor no longer targeted";
    case RANDOM_TRAVEL:
        return "Try a random travel";
    case SLEEP:
        return "Player sleep";
    case END_SLEEP:
        return "End sleep";
    default:
        return "Unknown message";
    }
}

#endif // MESSAGES_H
