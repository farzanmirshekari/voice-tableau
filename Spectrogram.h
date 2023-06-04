#pragma once

#include "utilities.h"

#include <fftw3.h>

class Spectrogram {
	public:
		Spectrogram(int length);
		~Spectrogram();

		double get_magnitude();

		double * get_magnitude_array();
		double * get_frequency_domain();
		double * get_time_domain();
		double * get_window();
		double* get_data();
	private:
		int length;
		fftw_plan plan;
		double* time_domain;
		double* window;
		double* frequency_domain;
		double* magnitude;
		double data[];

		void hanning_window(int data_length);
};
