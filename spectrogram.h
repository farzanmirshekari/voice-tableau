#pragma once

#include <fftw3.h>

typedef struct {
    int length;
    fftw_plan plan;

    double *window;
    double *time_domain;
    double *frequency_domain;
    double *magnitude;

    double data[];
} Spectrogram;

Spectrogram * intialize( int length );
void destroy( Spectrogram *spectrogram );
double get_magnitude( Spectrogram *spectrogram );