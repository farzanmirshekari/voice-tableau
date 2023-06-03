#include <cmath>

const int sample_rate = 16000;
const double minimum_frequency = 0.0;
const double maximum_frequency = (double)sample_rate / 2.0;
const double TIME_COVERED_BY_WINDOW = 10;
const double TIME_STEP = 0.025;
const int WIN_SIZE = (int)(TIME_COVERED_BY_WINDOW * sample_rate);
const int STEP_SIZE = (int)(TIME_STEP * sample_rate);
const int RESERVE_SIZE = WIN_SIZE - STEP_SIZE;
const double floor_decibels = -180.0;
const double LINEAR_SPEC_FLOOR = pow(10.0, floor_decibels / 20.0);
const int PIXEL_WIDTH_PER_SECOND = 100;
const int SPECTROGRAM_W = PIXEL_WIDTH_PER_SECOND * TIME_COVERED_BY_WINDOW;
const int SPECTROGRAM_H = 512;
const double MAG_TO_NORMALIZE = 100.0;