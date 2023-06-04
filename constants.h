#pragma once

#include <cmath>

const double SAMPLE_RATE = 16000;
const double MINIMUM_FREQUENCY = 0.0;
const double MAXIMUM_FREQUENCY = (double)SAMPLE_RATE / 2.0;
const double TIME_COVERED_BY_WINDOW = 10;
const double TIME_STEP = 0.025;
const double FLOOR_DECIBELS = -180.0;
const int WINDOW_SIZE = (int)(TIME_COVERED_BY_WINDOW * SAMPLE_RATE);
const int STEP_SIZE = (int)(TIME_STEP * SAMPLE_RATE);
const int RESERVE_SIZE = WINDOW_SIZE - STEP_SIZE;
const int PIXEL_WIDTH_PER_SECOND = 100;
const int SPECTROGRAM_WIDTH = PIXEL_WIDTH_PER_SECOND * TIME_COVERED_BY_WINDOW;
const int SPECTROGRAM_HEIGHT = 512;