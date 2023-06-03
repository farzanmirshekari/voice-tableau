#pragma once

#include <math.h>

#define ARRAY_LEN(x) ((int) (sizeof (x) / sizeof (x[0])))

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

inline void colour_map (float value, double floor_decibels, unsigned char colour[3]) {	
    static unsigned char map[][3] = {
        {	255,	255,	255	},	/* -0dB */
        {	240,	254,	216	},	/* -10dB */
        {	242,	251,	185	},	/* -20dB */
        {	253,	245,	143	},	/* -30dB */
        {	253,	200,	102	},	/* -40dB */
        {	252,	144,	66	},	/* -50dB */
        {	252,	75,		32	},	/* -60dB */
        {	237,	28,		41	},	/* -70dB */
        {	214,	3,		64	},	/* -80dB */
        {	183,	3,		101	},	/* -90dB */
        {	157,	3,		122	},	/* -100dB */
        {	122,	3,		126	},	/* -110dB */
        {	80,		2,		110	},	/* -120dB */
        {	45,		2,		89	},	/* -130dB */
        {	19,		2,		70	},	/* -140dB */
        {	1,		3,		53	},	/* -150dB */
        {	1,		3,		37	},	/* -160dB */
        {	1,		2,		19	},	/* -170dB */
        {	0,		0,		0	},	/* -180dB */
    };

    float remainder;
    int index;

    if (value >= 0.0) {	
        colour[0] = colour[1] = colour[2] = 255;
        return;
    }

    value = fabs(value * (-180.0 / floor_decibels) * 0.1);

    index = lrintf(floor(value));

    if (index >= ARRAY_LEN (map) - 1) {	
        colour[0] = colour[1] = colour[2] = 0;
        return;
    }

    remainder = fmod(value, 1.0);

    colour[0] = lrintf((1.0 - remainder) * map[index][0] + remainder * map[index + 1][0]);
    colour[1] = lrintf((1.0 - remainder) * map[index][1] + remainder * map[index + 1][1]);
    colour[2] = lrintf((1.0 - remainder) * map[index][2] + remainder * map[index + 1][2]);

    return;
}

inline double magnitude_to_spectrogram (int length, int magnitude_length, int magnitude_index, double minimum_frequency, double maximum_frequency, int sample_rate) {
    double frequency;
    frequency = minimum_frequency + (maximum_frequency - minimum_frequency) * magnitude_index / (magnitude_length - 1);
    return (frequency * length / (sample_rate / 2));
}

inline void map_spectrogram_to_magnitude (float * magnitude, int magnitude_length, const double *spectrogram, int length, const double minimum_frequency, const double maximum_frequency, int sample_rate) {
    int k;
    for (k = 0; k < magnitude_length; k++) {
        double current = magnitude_to_spectrogram (length, magnitude_length, k,
                                                   minimum_frequency, maximum_frequency, sample_rate);
        double next = magnitude_to_spectrogram (length, magnitude_length, k+1,
                                                minimum_frequency, maximum_frequency, sample_rate);

        if (current > length) {	
            magnitude[k] = 0.0;
            return;
        }

        if (next > (current + 1)) {	
            double count = 1.0 - (current - floor (current));
            double sum = spectrogram[(int) current] * count;

            while ((current += 1.0) < next && (int) current <= length) {
            	sum += spectrogram[(int) current];
                count += 1.0;
            }
            if ((int) next <= length) {
            	sum += spectrogram[(int) next] * (next - floor (next));
                count += next - floor (next);
            }
            magnitude[k] = sum / count;
        }
        else {	
            magnitude[k] = spectrogram[(int) current] * (1.0 - (current - floor (current)))
                         + spectrogram[(int) current + 1] * (current - floor (current));
        }
    }
    return;
}