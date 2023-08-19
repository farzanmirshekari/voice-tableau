#pragma once

#include <cmath>

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

const double SAMPLE_RATE = 16000;
const double MINIMUM_FREQUENCY = 0.0;
const double MAXIMUM_FREQUENCY = SAMPLE_RATE / 2.0;
const double TIME_COVERED_BY_WINDOW = 10;
const double TIME_STEP = 0.065;
const int WINDOW_SIZE = (int)(TIME_COVERED_BY_WINDOW * SAMPLE_RATE);
const int STEP_SIZE = (int)(TIME_STEP * SAMPLE_RATE);
const int RESERVE_SIZE = WINDOW_SIZE - STEP_SIZE;
const int SPECTROGRAM_HEIGHT = 768;
const int SPECTROGRAM_WIDTH = SPECTROGRAM_HEIGHT;

static const unsigned char colour_map[][3] = {
    { 255, 255, 255 },
    { 255, 104, 123 },
    { 255, 175, 89 },
    { 255, 232, 59 },
    { 160, 237, 57 },
    { 78, 237, 118 },
    { 57, 207, 237 },
    { 110, 120, 239 },
    { 185, 88, 240 },
    { 220, 60, 200 },
    { 255, 78, 168 },
    { 255, 110, 110 },
    { 255, 160, 128 },
    { 255, 214, 139 },
    { 252, 255, 130 },
    { 217, 255, 106 },
    { 156, 255, 97 },
    { 99, 255, 148 },
    { 137, 137, 137 },
};

void map_colours(float value, unsigned char * colours);
void map_spectrogram_to_magnitude(float *magnitude, const double *spectrogram, int length);