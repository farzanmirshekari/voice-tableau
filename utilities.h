#pragma once

#include <cmath>

const double SAMPLE_RATE = 16000;
const double MINIMUM_FREQUENCY = 0.0;
const double MAXIMUM_FREQUENCY = SAMPLE_RATE / 2.0;
const double TIME_COVERED_BY_WINDOW = 10;
const double TIME_STEP = 0.025;
const int WINDOW_SIZE = (int)(TIME_COVERED_BY_WINDOW * SAMPLE_RATE);
const int STEP_SIZE = (int)(TIME_STEP * SAMPLE_RATE);
const int RESERVE_SIZE = WINDOW_SIZE - STEP_SIZE;
const int SPECTROGRAM_HEIGHT = 768;
const int SPECTROGRAM_WIDTH = SPECTROGRAM_HEIGHT;

template<typename T, size_t N>
inline constexpr size_t ARRAY_LENGTH(const T (&array)[N]) {
    return N;
}

template<typename T>
inline constexpr T MAX(const T& x, const T& y) {
    return (x > y) ? x : y;
}

template<typename T>
inline constexpr T MIN(const T& x, const T& y) {
    return (x < y) ? x : y;
}

inline void colour_map(float value, unsigned char colours[3]) {	
    static unsigned char map[][3] = {
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

    float remainder;
    int index;

    if (value >= 0.0) {	
        colours[0] = colours[1] = colours[2] = 255;
        return;
    }

    value = abs(value * 0.1);
    index = round(floor(value));

    if (index >= ARRAY_LENGTH(map) - 1) {
        colours[0] = colours[1] = colours[2] = 0;
        return;
    }

    remainder = fmod(value, 1.0);

    colours[0] = round((1.0 - remainder) * map[index][0] + remainder * map[index + 1][0]);
    colours[1] = round((1.0 - remainder) * map[index][1] + remainder * map[index + 1][1]);
    colours[2] = round((1.0 - remainder) * map[index][2] + remainder * map[index + 1][2]);
}

inline double magnitude_to_spectrogram(int length, int magnitude_length, int magnitude_index, double minimum_frequency, double maximum_frequency, int sample_rate) {
    double frequency;
    frequency = minimum_frequency + (maximum_frequency - minimum_frequency) * magnitude_index / (magnitude_length - 1);
    return (frequency * length / (sample_rate / 2));
}

inline void map_spectrogram_to_magnitude(float *magnitude, int magnitude_length, const double *spectrogram, int length, double const minimum_frequency, double const maximum_frequency, int sample_rate) {
    for (int k = 0; k < magnitude_length; k++) {
        double current = magnitude_to_spectrogram(length, magnitude_length, k,
                                                   minimum_frequency, maximum_frequency, sample_rate);
        double next = magnitude_to_spectrogram(length, magnitude_length, k+1,
                                                minimum_frequency, maximum_frequency, sample_rate);

        if (current > length) {	
            magnitude[k] = 0.0;
            return;
        }

        if (next > (current + 1)) {	
            double count = 1.0 - (current - floor(current));
            double sum = spectrogram[(int) current] * count;

            while ((current += 1.0) < next && (int) current <= length) {
            	sum += spectrogram[(int) current];
                count += 1.0;
            }
            if ((int)next <= length) {
            	sum += spectrogram[(int) next] * (next - floor(next));
                count += next - floor(next);
            }
            magnitude[k] = sum / count;
        }
        else {	
            magnitude[k] = spectrogram[(int) current] * (1.0 - (current - floor(current)))
                         + spectrogram[(int) current + 1] * (current - floor(current));
        }
    }
}