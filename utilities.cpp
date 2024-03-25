#include "utilities.h"

void map_colours(float value, unsigned char * colours)
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
        colours[i] = static_cast<unsigned char>((1.0f - remainder) * colour_map[index][i] + remainder * colour_map[index + 1][i]);
    }
}

void map_spectrogram_to_magnitude(float *magnitude, const double *spectrogram, int length)
{
    const auto magnitude_to_spectrogram = [](int length, int magnitude_index) -> double {
        double frequency = MINIMUM_FREQUENCY + (MAXIMUM_FREQUENCY - MINIMUM_FREQUENCY) * magnitude_index / (SPECTROGRAM_HEIGHT - 1);
        return (frequency * length / (SAMPLE_RATE / 2));
    };

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