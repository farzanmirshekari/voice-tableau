#pragma once

#include "utilities.h"

#include <fftw3.h>

class Spectrogram 
{
	public:
		Spectrogram(int length);
		~Spectrogram();

		void calculate_magnitude();

		double* get_magnitude_array();
		double* get_time_domain();
	private:
		int length;
		fftw_plan plan;
		double* time_domain;
		double* window;
		double* frequency_domain;
		double* magnitude;

		void hanning_window(int data_length);
};
