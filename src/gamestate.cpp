#pragma once

namespace gamestate
{
    int score = 0;
    int missed = 0;
    int hits = 0;

    bool paused = true;
    bool powerUpAvailable = true;
    float powerUpActiveUntil = -1.0f;
    float shootCooldown = -1.0f;
    float powerUp = 0.0f;
};