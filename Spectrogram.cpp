#include "Spectrogram.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

inline void hanning_window(double * data, int data_length) {
	for (int k = 0; k < data_length; k++) {
		data[k] = 0.5 * (1.0 - cos(2.0 * M_PI * k / (data_length - 1)));
	}
	return;
}

Spectrogram* initialize(int length) {	
	Spectrogram *spectrogram;

	spectrogram = (Spectrogram *)calloc(1, sizeof (Spectrogram));

	spectrogram->length = length;
	spectrogram->time_domain = (double *)calloc(2 * length + 1, sizeof (double));
	spectrogram->window = (double *)calloc(2 * length, sizeof (double));
	spectrogram->frequency_domain = (double *)calloc(2 * length, sizeof (double));
	spectrogram->magnitude = (double *)calloc(length + 1, sizeof (double));
	spectrogram->plan = fftw_plan_r2r_1d(2 * length, spectrogram->time_domain, spectrogram->frequency_domain, FFTW_R2HC, FFTW_MEASURE | FFTW_PRESERVE_INPUT);
	hanning_window(spectrogram->window, 2 * length);

	return spectrogram;
}


void destroy (Spectrogram * spectrogram) {
	fftw_destroy_plan(spectrogram->plan);
	free(spectrogram->time_domain);
	free(spectrogram->window);
	free(spectrogram->frequency_domain);
	free(spectrogram->magnitude);
	free(spectrogram);
}

double get_magnitude(Spectrogram * spectrogram) {
	double max;
	int k, frequency_length;

	frequency_length = 2 * spectrogram->length;

	for (k = 0; k < 2 * spectrogram->length; k++) {
		spectrogram->time_domain[k] *= spectrogram->window[k];
	}

	fftw_execute(spectrogram->plan);

	max = spectrogram->magnitude[0] = fabs(spectrogram->frequency_domain[0]);

	for (k = 1; k < spectrogram->length; k++) {
		double re = spectrogram->frequency_domain[k];
		double image = spectrogram->frequency_domain[frequency_length - k];
		spectrogram->magnitude[k] = sqrt(re * re + image * image);
		max = MAX (max, spectrogram->magnitude[k]);
	}
	spectrogram->magnitude[spectrogram->length] = fabs(spectrogram->frequency_domain[spectrogram->length]);

	return max;
}

