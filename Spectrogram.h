#pragma once

#include "utilities.h"

#include <fftw3.h>

typedef struct {	
	int length;
	fftw_plan plan;

	double* time_domain;
	double* window;
	double* frequency_domain;
	double* magnitude;

	double data[];
} Spectrogram;

Spectrogram * initialize(int length);
void destroy(Spectrogram * spectrogram);
double get_magnitude(Spectrogram * spectrogram);
void hanning_window(double * data, int data_length);
