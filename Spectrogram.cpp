#include "Spectrogram.h"

#include <cmath>

void Spectrogram::hanning_window(int data_length) {
	for (int k = 0; k < data_length; k++) {
		window[k] = 0.5 * (1.0 - cos(2.0 * M_PI * k / (data_length - 1)));
	}
}

Spectrogram::Spectrogram(int length) {	
	this->length = length;
	this->time_domain = new double[2 * length + 1];
	this->window = new double[2 * length];
	this->frequency_domain = new double[2 * length];
	this->magnitude = new double[length + 1];
	this->plan = fftw_plan_r2r_1d(2 * length, this->time_domain, this->frequency_domain, FFTW_R2HC, FFTW_MEASURE | FFTW_PRESERVE_INPUT);
	hanning_window(this->length * 2);
}


Spectrogram::~Spectrogram() {
	fftw_destroy_plan(this->plan);
	delete[] this->time_domain;
	delete[] this->window;
	delete[] this->frequency_domain;
	delete[] this->magnitude;
}

void Spectrogram::calculate_magnitude() {
	int k, frequency_length;

	frequency_length = 2 * this->length;

	for (k = 0; k < 2 * this->length; k++) {
		this->time_domain[k] *= this->window[k];
	}

	fftw_execute(this->plan);

	for (k = 1; k < this->length; k++) {
		double real = this->frequency_domain[k];
		double imaginary = this->frequency_domain[frequency_length - k];
		this->magnitude[k] = sqrt(real * real + imaginary * imaginary);
	}
	this->magnitude[this->length] = abs(this->frequency_domain[this->length]);
}

double * Spectrogram::get_magnitude_array() {
	return this->magnitude;
}

double * Spectrogram::get_time_domain() {
	return this->time_domain;
}