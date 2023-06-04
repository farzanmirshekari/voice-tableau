#include "Spectrogram.h"
#include "utilities.h"
#include "constants.h"

#include <fftw3.h>
#include <portaudio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
    PaStreamParameters input_parameters;
    PaError err;
    PaStream *stream;

    err = Pa_Initialize();
    if (err != paNoError) {
        Pa_Terminate();
        return 1;
    }

    input_parameters.device = Pa_GetDefaultInputDevice();
    if (input_parameters.device == paNoDevice) {
        Pa_Terminate();
        return 1;
    }

    input_parameters.channelCount = 1;
    input_parameters.sampleFormat = paFloat32;
    input_parameters.suggestedLatency = Pa_GetDeviceInfo(input_parameters.device)->defaultLowInputLatency;
    input_parameters.hostApiSpecificStreamInfo = nullptr;
    err = Pa_OpenStream(&stream, &input_parameters, nullptr, SAMPLE_RATE, 1024, paClipOff, nullptr, nullptr);
    if (err != paNoError) {
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        Pa_Terminate();
        return 1;
    }

    if (RESERVE_SIZE < 0) {
        return 1;
    }

    int length = SPECTROGRAM_HEIGHT * (SAMPLE_RATE / 20 / SPECTROGRAM_HEIGHT + 1);
    Spectrogram *spectrogram = new Spectrogram(length);
    float **magnitude = new float *[SPECTROGRAM_WIDTH];
    for (int i = 0; i < SPECTROGRAM_WIDTH; ++i) {
        magnitude[i] = new float[SPECTROGRAM_HEIGHT];
    }

    float *clip = new float[WINDOW_SIZE];
    float *clip_fill_in_position = clip + RESERVE_SIZE;
    float *clip_step_in_position = clip + STEP_SIZE;
    Pa_ReadStream(stream, clip, RESERVE_SIZE);

    cv::Mat image(SPECTROGRAM_HEIGHT, SPECTROGRAM_WIDTH, CV_8UC3);
    unsigned char *image_data = image.data;
    unsigned char colour[3] = {0, 0, 0};

    while (true) {
        Pa_ReadStream(stream, clip_fill_in_position, STEP_SIZE);

        for (int j = 0; j < SPECTROGRAM_WIDTH; ++j) {
            int data_length = 2 * length;
            double *data = spectrogram->get_time_domain();
            for (int i = 0; i < data_length; ++i) {
                data[i] = 0.0;
            }

            int start = (j * WINDOW_SIZE) / SPECTROGRAM_WIDTH - length;
            if (start >= 0) {
                int copy_length = 0;
                if (start + data_length > WINDOW_SIZE) {
                    copy_length = WINDOW_SIZE - start;
                }
                else {
                    copy_length = data_length;
                }
                for (int i = 0; i < copy_length; ++i) {
                    data[i] = clip[i + start];
                }
            }
            else {
                start = -start;
                data += start;
                data_length -= start;
                for (int i = 0; i < data_length; ++i) {
                    data[i] = clip[i];
                }
            }
            spectrogram->calculate_magnitude();
            map_spectrogram_to_magnitude(magnitude[j], SPECTROGRAM_HEIGHT, spectrogram->get_magnitude_array(), length, MINIMUM_FREQUENCY, MAXIMUM_FREQUENCY, SAMPLE_RATE);
        }

        for (int j = 0; j < SPECTROGRAM_WIDTH; ++j) {
            for (int k = 0; k < SPECTROGRAM_HEIGHT; ++k) {
                magnitude[j][k] /= 100.0;
                magnitude[j][k] = (magnitude[j][k] < pow(10.0, FLOOR_DECIBELS / 20.0)) ? FLOOR_DECIBELS : 20.0 * log10(magnitude[j][k]);
                colour_map(magnitude[j][k], FLOOR_DECIBELS, colour);
                image_data[((SPECTROGRAM_HEIGHT - 1 - k) * image.cols + j) * 3] = colour[2];
                image_data[((SPECTROGRAM_HEIGHT - 1 - k) * image.cols + j) * 3 + 1] = colour[1];
                image_data[((SPECTROGRAM_HEIGHT - 1 - k) * image.cols + j) * 3 + 2] = colour[0];
            }
        }
        cv::imshow("Voice Tableau", image);
        unsigned char key = cv::waitKey(10);
        if (key == 27) {
            break;
        }
        for (int i = 0; i < RESERVE_SIZE; ++i) {
            clip[i] = clip_step_in_position[i];
        }
    }


    delete spectrogram;
    for (int i = 0; i < SPECTROGRAM_WIDTH; ++i) {
        delete[] magnitude[i];
    }
    delete[] magnitude;

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        Pa_Terminate();
        return 1;
    }

    Pa_Terminate();
    delete[] clip;
    return 0;
}