#pragma once

#include <math.h>

#define ARRAY_LENGTH(x) ((int) (sizeof (x) / sizeof (x[0])))

typedef struct {
    double real;
    double imaginary;
} Complex;

inline void colour_map ( float value, double spectrogram_floor, unsigned char colour[3] ) {	
    static unsigned char map[][3] = {
        {	255,	255,	255   },	/* -0dB */
        {	240,	254,	216	  },	/* -10dB */
        {	242,	251,	185	  },	/* -20dB */
        {	253,	245,	143	  },	/* -30dB */
        {	253,	200,	102	  },	/* -40dB */
        {	252,	144,	66	  },	/* -50dB */
        {	252,	75,		32	  },	/* -60dB */
        {	237,	28,		41	  },	/* -70dB */
        {	214,	3,		64	  },	/* -80dB */
        {	183,	3,		101	  },	/* -90dB */
        {	157,	3,		122	  },	/* -100dB */
        {	122,	3,		126	  },	/* -110dB */
        {	80,		2,		110	  },	/* -120dB */
        {	45,		2,		89	  },	/* -130dB */
        {	19,		2,		70	  },	/* -140dB */
        {	1,		3,		53	  },	/* -150dB */
        {	1,		3,		37	  },	/* -160dB */
        {	1,		2,		19	  },	/* -170dB */
        {	0,		0,		0	  },	/* -180dB */
    } ;

    float remainder;
    int index;

    if (value >= 0.0) {
        colour[0] = colour[1] = colour[2] = 255;
        return;
    }

    value = fabs(value * (-180.0 / spectrogram_floor) * 0.1);

    index = lrintf(floor(value));

    if (index < 0) {
        exit(1);
    };

    if (index >= ARRAY_LENGTH(map) - 1) {	
        colour[0] = colour[1] = colour[2] = 0;
        return;
    }

    remainder = fmod(value, 1.0);

    colour[0] = lrintf((1.0 - remainder) * map [index][0] + remainder * map [index + 1][0]);
    colour[1] = lrintf((1.0 - remainder) * map [index][1] + remainder * map [index + 1][1]);
    colour[2] = lrintf((1.0 - remainder) * map [index][2] + remainder * map [index + 1][2]);

    return;
}