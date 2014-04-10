#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>

enum MessageType {
    PLAYER_FED,
    FEED_SUCCESSFUL,
    TARGETED,
    NOT_TARGETED
};

inline std::string msgToString(int msg) {
    switch (msg) {
    case PLAYER_FED:
        return "Player fed from actor";
    case FEED_SUCCESSFUL:
        return "Player fed successfully";
    case TARGETED:
        return "Actor has been targeted";
    case NOT_TARGETED:
        return "Actor no longer targeted";
    }
}

#endif // MESSAGES_H
