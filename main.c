#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define MAX_SAMPLES_PER_UPDATE   4096


const int BIT_DEPTH_I = 32000;
const int FRAME_RATE_I = 44100;
const double BIT_DEPTH_F = (double)BIT_DEPTH_I;
const double FRAME_RATE_F = (double)FRAME_RATE_I;

short *g_history;
unsigned int g_historyLength = 10000;

unsigned int g_framesSoFar = 0;
double prevFreq = 0.0;
double frequency = 0.0;
double g_phaseOffset = 0.0;

double clamp(double a, double b) {
    if (a > b) {
        return b;
    }
    return a;
}


double sine_wave(double freq, double time) {
    if (prevFreq != frequency) {
        g_phaseOffset = -time;
        prevFreq = frequency;
        printf("new: %f\n", g_phaseOffset);
    }
    return sin(2.0 * M_PI * freq * (time + g_phaseOffset));
}

short CreateFrame(short *history, unsigned int historyLength, unsigned int framesSoFar) {
    double wave = sine_wave(frequency, (double)framesSoFar / FRAME_RATE_F);
    float prev = g_history[0];
    if (prev == 0.0) {
        return (short)(clamp(32000.0*wave, 32000.0));
    } else {
        return (short)(clamp(32000.0*wave + prev, 32000.0));
    }
}

void AudioInputCallback(void *buffer, int frames){
    short *d = (short *)buffer;
    for (int i = 0; i < frames; i++) {
        d[i] = CreateFrame(g_history, g_historyLength, g_framesSoFar);
        memmove(&g_history[0], &g_history[1], (g_historyLength-1)*sizeof(short));
        g_history[g_historyLength-1] = d[i];
        g_framesSoFar++;
    }
}

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Ivan's Cool Synthesizer");

    InitAudioDevice();

    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);

    AudioStream stream = LoadAudioStream(44100, 16, 1);

    SetAudioStreamCallback(stream, AudioInputCallback);
    SetAudioStreamVolume(stream, 0.2f);

    g_history = (short *)malloc(sizeof(short)*g_historyLength);

    PlayAudioStream(stream);

    Vector2 position = { 0, 0 };


    while (!WindowShouldClose())
    {

        if (IsKeyDown(KEY_A)) {
            prevFreq = frequency;
            frequency = 440.0;

        } else if (IsKeyDown(KEY_B)) {
            prevFreq = frequency;
            frequency = 493.0;

        } else if (IsKeyDown(KEY_C)) {
            prevFreq = frequency;
            frequency = 523.0;
        } else if (IsKeyDown(KEY_D)) {
            prevFreq = frequency;
            frequency = 587.0;
        } else if (IsKeyDown(KEY_E)) {
            prevFreq = frequency;
            frequency = 659.0;
        } else if (IsKeyDown(KEY_F)) {
            prevFreq = frequency;
            frequency = 698.0;
        } else if (IsKeyDown(KEY_G)) {
            prevFreq = frequency;
            frequency = 784.0;
        } else {
            prevFreq = frequency;
            frequency = 0.0;
        }

        BeginDrawing();

        ClearBackground(ColorFromHSV(0.16f, 0.46f, 0.16f));

        Vector2 lastPos = {0.0f, 250.0f};
        for (int i = 0; i < screenWidth; i++){
            position.x = (double)i;
            position.y = 540 + 200*g_history[i/4]/32000.0f;
            DrawLineBezier(lastPos, position, 3.0f, RED);
            lastPos = position;
        }

        EndDrawing();
    }

    free(g_history);

    UnloadAudioStream(stream);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}