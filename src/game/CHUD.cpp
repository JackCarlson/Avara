#include "CHUD.h"

#include "CAbstractPlayer.h"
#include "CAvaraGame.h"
#include "CPlayerManager.h"

CHUD::CHUD(CAvaraGame *game) {
    itsGame = game;
}

const int CHAT_CHARS = 50;

void CHUD::Render(CViewParameters *view, NVGcontext *ctx) {
    CAbstractPlayer *player = itsGame->GetLocalPlayer();

    if (!player)
        return;

    int bufferWidth = view->viewPixelDimensions.h, bufferHeight = view->viewPixelDimensions.v;

    nvgBeginFrame(ctx, bufferWidth, bufferHeight, view->viewPixelRatio);

    CAbstractPlayer *eachPlayer = itsGame->playerList;

    int p = 0;
    float pY;
    int colorR, colorG, colorB;
    float fontsz_m = 15.0, fontsz_s = 10.0;
    while (eachPlayer) {
        if (p >= 6)
            break;
        pY = (bufferHeight - 72) + (11 * p);
        colorR = (eachPlayer->longTeamColor >> 16) & 0xff;
        colorG = (eachPlayer->longTeamColor >> 8) & 0xff;
        colorB = eachPlayer->longTeamColor & 0xff;

        std::string playerName((char *)eachPlayer->itsManager->playerName + 1, eachPlayer->itsManager->playerName[0]);
        std::string playerLives = std::to_string(eachPlayer->lives);

        std::string playerChat(eachPlayer->itsManager->lineBuffer.begin(), eachPlayer->itsManager->lineBuffer.end());//((char *)eachPlayer->itsManager->lineBuffer + 1, eachPlayer->itsManager->lineBuffer[0]);
        if (playerChat.length() > CHAT_CHARS) {
            playerChat = playerChat.substr(playerChat.length() - CHAT_CHARS, CHAT_CHARS);
        }

        nvgBeginPath(ctx);
        nvgRect(ctx, bufferWidth - 160, pY, 10.0, 10.0);
        nvgFillColor(ctx, nvgRGBA(colorR, colorG, colorB, 255));
        nvgFill(ctx);

        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgFontSize(ctx, fontsz_m);
        nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
        nvgText(ctx, bufferWidth - 148, pY - 3, playerName.c_str(), NULL);

        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
        nvgFontSize(ctx, fontsz_s);
        nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
        nvgText(ctx, bufferWidth - 162, pY - 3, playerLives.c_str(), NULL);


        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
        nvgFontSize(ctx, fontsz_m);
        nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
        nvgText(ctx, bufferWidth - 168, pY - 3, playerChat.c_str(), NULL);

        p++;
        eachPlayer = eachPlayer->nextPlayer;
    }

    int i, j;
    float g1X = (bufferWidth / 2.0) - 60.0;
    float gY = bufferHeight - 50.0;
    float g2X1 = (bufferWidth / 2.0) + 15.0;
    float g2Y1 = bufferHeight - 50.0;
    float g2X2 = (bufferWidth / 2.0) + 15.0 + 22.5;
    float g2Y2 = bufferHeight - 50.0 + 7.5;
    float g2X3 = (bufferWidth / 2.0) + 15.0 + 15.0;
    float g2Y3 = bufferHeight - 50.0 + 30.0;
    float g2X4 = (bufferWidth / 2.0) + 15.0 - 7.5;
    float g2Y4 = bufferHeight - 50.0 + 22.5;
    char scoreText[20];
    char timeText[9];
    float full = 30.0;
    float half = 15.0;
    float w_pow = 15.0;
    int secs, mins;
    long timeTemp = itsGame->timeInSeconds;
    float energy = ((float)player->energy / (float)player->maxEnergy) * full;
    float pow_l = ((float)player->gunEnergy[0] / (float)player->fullGunEnergy) * half;
    float pow_r = ((float)player->gunEnergy[1] / (float)player->fullGunEnergy) * half;
    float shields = ((float)player->shields / (float)player->maxShields) * full;
    float g2s = 3.75;
    float g1[4][4][2] = {{{g1X, gY}, {g1X, gY + energy}, {g1X + energy, gY + energy}, {g1X, gY}},
        {{g1X + half, gY + half},
            {g1X + half - pow_l, gY + half - pow_l},
            {g1X + half - pow_l + w_pow, gY + half - pow_l},
            {g1X + half + w_pow, gY + half}},
        {{g1X + half, gY + half},
            {g1X + half + pow_r, gY + half + pow_r},
            {g1X + half + pow_r + w_pow, gY + half + pow_r},
            {g1X + half + w_pow, gY + half}},
        {{g1X + w_pow + full - shields, gY + full - shields},
            {g1X + full + w_pow, gY + full},
            {g1X + full + w_pow, gY + full},
            {g1X + w_pow + full, gY + full - shields}}};
    float g2[4][4][4][2] = {{// boosters
                                {{g2X1 + (g2s * 0), g2Y1 + (g2s * 0)},
                                    {g2X1 + (g2s * 1), g2Y1 + (g2s * 1)},
                                    {g2X1 + (g2s * 7), g2Y1 + (g2s * 1)},
                                    {g2X1 + (g2s * 8), g2Y1 + (g2s * 0)}},
                                {{g2X1 + (g2s * 1), g2Y1 + (g2s * 1)},
                                    {g2X1 + (g2s * 2), g2Y1 + (g2s * 2)},
                                    {g2X1 + (g2s * 6), g2Y1 + (g2s * 2)},
                                    {g2X1 + (g2s * 7), g2Y1 + (g2s * 1)}},
                                {{g2X1 + (g2s * 2), g2Y1 + (g2s * 2)},
                                    {g2X1 + (g2s * 3), g2Y1 + (g2s * 3)},
                                    {g2X1 + (g2s * 5), g2Y1 + (g2s * 3)},
                                    {g2X1 + (g2s * 6), g2Y1 + (g2s * 2)}},
                                {{g2X1 + (g2s * 3), g2Y1 + (g2s * 3)},
                                    {g2X1 + (g2s * 4), g2Y1 + (g2s * 4)},
                                    {g2X1 + (g2s * 4), g2Y1 + (g2s * 4)},
                                    {g2X1 + (g2s * 5), g2Y1 + (g2s * 3)}}},
        {// grenades
            {{g2X2 - (g2s * 0), g2Y2 + (g2s * 0)},
                {g2X2 - (g2s * 1), g2Y2 + (g2s * 1)},
                {g2X2 - (g2s * 1), g2Y2 + (g2s * 7)},
                {g2X2 - (g2s * 0), g2Y2 + (g2s * 8)}},
            {{g2X2 - (g2s * 1), g2Y2 + (g2s * 1)},
                {g2X2 - (g2s * 2), g2Y2 + (g2s * 2)},
                {g2X2 - (g2s * 2), g2Y2 + (g2s * 6)},
                {g2X2 - (g2s * 1), g2Y2 + (g2s * 7)}},
            {{g2X2 - (g2s * 2), g2Y2 + (g2s * 2)},
                {g2X2 - (g2s * 3), g2Y2 + (g2s * 3)},
                {g2X2 - (g2s * 3), g2Y2 + (g2s * 5)},
                {g2X2 - (g2s * 2), g2Y2 + (g2s * 6)}},
            {{g2X2 - (g2s * 3), g2Y2 + (g2s * 3)},
                {g2X2 - (g2s * 4), g2Y2 + (g2s * 4)},
                {g2X2 - (g2s * 4), g2Y2 + (g2s * 4)},
                {g2X2 - (g2s * 3), g2Y2 + (g2s * 5)}}},
        {// lives
            {{g2X3 - (g2s * 0), g2Y3 - (g2s * 0)},
                {g2X3 - (g2s * 1), g2Y3 - (g2s * 1)},
                {g2X3 - (g2s * 7), g2Y3 - (g2s * 1)},
                {g2X3 - (g2s * 8), g2Y3 - (g2s * 0)}},
            {{g2X3 - (g2s * 1), g2Y3 - (g2s * 1)},
                {g2X3 - (g2s * 2), g2Y3 - (g2s * 2)},
                {g2X3 - (g2s * 6), g2Y3 - (g2s * 2)},
                {g2X3 - (g2s * 7), g2Y3 - (g2s * 1)}},
            {{g2X3 - (g2s * 2), g2Y3 - (g2s * 2)},
                {g2X3 - (g2s * 3), g2Y3 - (g2s * 3)},
                {g2X3 - (g2s * 5), g2Y3 - (g2s * 3)},
                {g2X3 - (g2s * 6), g2Y3 - (g2s * 2)}},
            {{g2X3 - (g2s * 3), g2Y3 - (g2s * 3)},
                {g2X3 - (g2s * 4), g2Y3 - (g2s * 4)},
                {g2X3 - (g2s * 4), g2Y3 - (g2s * 4)},
                {g2X3 - (g2s * 5), g2Y3 - (g2s * 3)}}},
        {// missiles
            {{g2X4 + (g2s * 0), g2Y4 - (g2s * 0)},
                {g2X4 + (g2s * 1), g2Y4 - (g2s * 1)},
                {g2X4 + (g2s * 1), g2Y4 - (g2s * 7)},
                {g2X4 + (g2s * 0), g2Y4 - (g2s * 8)}},
            {{g2X4 + (g2s * 1), g2Y4 - (g2s * 1)},
                {g2X4 + (g2s * 2), g2Y4 - (g2s * 2)},
                {g2X4 + (g2s * 2), g2Y4 - (g2s * 6)},
                {g2X4 + (g2s * 1), g2Y4 - (g2s * 7)}},
            {{g2X4 + (g2s * 2), g2Y4 - (g2s * 2)},
                {g2X4 + (g2s * 3), g2Y4 - (g2s * 3)},
                {g2X4 + (g2s * 3), g2Y4 - (g2s * 5)},
                {g2X4 + (g2s * 2), g2Y4 - (g2s * 6)}},
            {{g2X4 + (g2s * 3), g2Y4 - (g2s * 3)},
                {g2X4 + (g2s * 4), g2Y4 - (g2s * 4)},
                {g2X4 + (g2s * 4), g2Y4 - (g2s * 4)},
                {g2X4 + (g2s * 3), g2Y4 - (g2s * 5)}}}};
    NVGcolor g1c[] = {
        nvgRGBA(0, 143, 0, 255), nvgRGBA(163, 54, 0, 255), nvgRGBA(255, 54, 0, 255), nvgRGBA(0, 61, 165, 255)};
    NVGcolor g2c[] = {
        nvgRGBA(0, 0, 102, 255), nvgRGBA(0, 0, 153, 255), nvgRGBA(0, 0, 204, 255), nvgRGBA(0, 0, 255, 255)};
    for (i = 0; i < 4; i++) { // referred to as GrafPanel in original Avara
        nvgBeginPath(ctx);
        nvgMoveTo(ctx, g1[i][0][0], g1[i][0][1]);
        nvgLineTo(ctx, g1[i][1][0], g1[i][1][1]);
        nvgLineTo(ctx, g1[i][2][0], g1[i][2][1]);
        nvgLineTo(ctx, g1[i][3][0], g1[i][3][1]);
        nvgClosePath(ctx);
        nvgFillColor(ctx, g1c[i]);
        nvgFill(ctx);
    }
    for (i = 0; i < 4; i++) { // "pinwheel" indicator
        for (j = 0; j < 4; j++) {
            if (i == 0 && j >= player->boostsRemaining)
                continue;
            if (i == 1 && j >= (player->grenadeCount + 1) / 2)
                continue;
            if (i == 2 && j >= player->lives)
                continue;
            if (i == 3 && j >= player->missileCount)
                continue;
            nvgBeginPath(ctx);
            nvgMoveTo(ctx, g2[i][j][0][0], g2[i][j][0][1]);
            nvgLineTo(ctx, g2[i][j][1][0], g2[i][j][1][1]);
            nvgLineTo(ctx, g2[i][j][2][0], g2[i][j][2][1]);
            nvgLineTo(ctx, g2[i][j][3][0], g2[i][j][3][1]);
            nvgClosePath(ctx);
            nvgFillColor(ctx, g2c[j]);
            nvgFill(ctx);
        }
    }
    if (itsGame->veryLongWait) {
        nvgBeginPath(ctx);
        nvgRect(ctx, g1X, gY, 8.0, 8.0);
        nvgFillColor(ctx, nvgRGBA(255, 255, 0, 255));
        nvgFill(ctx);
    }
    if (itsGame->longWait) {
        nvgBeginPath(ctx);
        nvgRect(ctx, g1X + 4, gY + 4, 8.0, 8.0);
        nvgFillColor(ctx, nvgRGBA(255, 192, 0, 255));
        nvgFill(ctx);
    }
    nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
    nvgFontSize(ctx, fontsz_m);
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    snprintf(scoreText, sizeof(scoreText), "%ld", itsGame->scores[player->itsManager->slot]);
    nvgText(ctx, g1X + 22.5, gY + full + 10.0, scoreText, NULL);

    secs = timeTemp % 60;
    timeTemp /= 60;
    mins = timeTemp % 60;
    timeTemp /= 60;
    snprintf(timeText, sizeof(timeText), "%ld:%02d:%02d", timeTemp, mins, secs);
    nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgText(ctx, g2X1 + 7.5, gY + full + 10.0, timeText, NULL);

    nvgEndFrame(ctx);
}
