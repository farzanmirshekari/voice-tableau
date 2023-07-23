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

inline void map_colours(float value, unsigned char * colours)
{
    if (value >= 0.0)
    {
        colours[0] = colours[1] = colours[2] = 255;
        return;
    }

    value = std::abs(value * 0.1);

    int index = static_cast<int>(std::floor(value));
    if (index >= ARRAY_LENGTH(colour_map) - 1)
    {
        colours[0] = colours[1] = colours[2] = 0;
        return;
    }

    float remainder = value - index;
    for (int i = 0; i < 3; ++i)
    {
        colours[i] = static_cast<unsigned char>(
            (1.0f - remainder) * colour_map[index][i] + remainder * colour_map[index + 1][i]
        );
    }
}

inline double magnitude_to_spectrogram(int length, int magnitude_index)
{
    double frequency = MINIMUM_FREQUENCY + (MAXIMUM_FREQUENCY - MINIMUM_FREQUENCY) * magnitude_index / (SPECTROGRAM_HEIGHT - 1);
    return (frequency * length / (SAMPLE_RATE / 2));
}

inline void map_spectrogram_to_magnitude(float *magnitude, const double *spectrogram, int length) 
{
    for (int k = 0; k < SPECTROGRAM_HEIGHT; ++k) 
    {
        double current = magnitude_to_spectrogram(length, k);
        double next = magnitude_to_spectrogram(length, k+1);

        if (current > length) 
        {	
            magnitude[k] = 0.0;
            return;
        }

        if (next > (current + 1)) 
        {	
            double count = 1.0 - (current - floor(current));
            double sum = spectrogram[(int) current] * count;

            while ((current += 1.0) < next && (int) current <= length) 
            {
            	sum += spectrogram[(int) current];
                count += 1.0;
            }
            if ((int) next <= length) 
            {
            	sum += spectrogram[(int) next] * (next - floor(next));
                count += next - floor(next);
            }
            magnitude[k] = sum / count;
        }
        else 
        {	
            magnitude[k] = spectrogram[(int) current] * (1.0 - (current - floor(current)))
                         + spectrogram[(int) current + 1] * (current - floor(current));
        }
    }
}