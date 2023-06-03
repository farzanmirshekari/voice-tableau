#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fftw3.h>
#include <portaudio.h>

#include "Spectrogram.h"
#include "utilities.h"
#include "constants.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
    int i = 0, j = 0, k = 0;

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
    input_parameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(&stream, &input_parameters, NULL, sample_rate, 1024, paClipOff, NULL, NULL);
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

    int length = SPECTROGRAM_H * (sample_rate / 20 / SPECTROGRAM_H + 1);
    Spectrogram *spectrogram = initialize(length);
    float **magnitude = (float **)calloc(SPECTROGRAM_W, sizeof(float *));
    for (i = 0; i < SPECTROGRAM_W; ++i) {
        if ((magnitude[i] = (float *)calloc(SPECTROGRAM_H, sizeof(float))) == NULL) {
            return 1;
        }
    }

    float *clip = (float *)calloc(WIN_SIZE, sizeof(float));
    float *clip_fill_in_position = clip + RESERVE_SIZE;
    float *clip_step_in_position = clip + STEP_SIZE;
    int frame_index = 0;
    Pa_ReadStream(stream, clip, RESERVE_SIZE);

    cv::Mat image(SPECTROGRAM_H, SPECTROGRAM_W, CV_8UC3);
    unsigned char *image_data = image.data;
    unsigned char colour[3] = {0, 0, 0};

    while (true) {
        Pa_ReadStream(stream, clip_fill_in_position, STEP_SIZE);

        for (j = 0; j < SPECTROGRAM_W; ++j) {
            int data_length = 2 * length;
            double *data = spectrogram->time_domain;
            for (i = 0; i < data_length; ++i) {
                data[i] = 0.0;
            }

            int start = (j * WIN_SIZE) / SPECTROGRAM_W - length;
            if (start >= 0) {
                int copy_length = 0;
                if (start + data_length > WIN_SIZE) {
                    copy_length = WIN_SIZE - start;
                }
                else {
                    copy_length = data_length;
                }
                for (i = 0; i < copy_length; ++i) {
                    data[i] = clip[i + start];
                }
            }
            else {
                start = -start;
                data += start;
                data_length -= start;
                for (i = 0; i < data_length; ++i) {
                    data[i] = clip[i];
                }
            }
            get_magnitude(spectrogram);
            map_spectrogram_to_magnitude(magnitude[j], SPECTROGRAM_H, spectrogram->magnitude, length, minimum_frequency, maximum_frequency, sample_rate);
        }

        for (j = 0; j < SPECTROGRAM_W; ++j) {
            for (k = 0; k < SPECTROGRAM_H; ++k) {
                magnitude[j][k] /= MAG_TO_NORMALIZE;
                magnitude[j][k] = (magnitude[j][k] < LINEAR_SPEC_FLOOR) ? floor_decibels : 20.0 * log10(magnitude[j][k]);
                colour_map(magnitude[j][k], floor_decibels, colour);
                image_data[((SPECTROGRAM_H - 1 - k) * image.cols + j) * 3] = colour[2];
                image_data[((SPECTROGRAM_H - 1 - k) * image.cols + j) * 3 + 1] = colour[1];
                image_data[((SPECTROGRAM_H - 1 - k) * image.cols + j) * 3 + 2] = colour[0];
            }
        }
        cv::imshow("Voice Tableau", image);
        unsigned char key = cv::waitKey(10);
        if (key == 27) {
            break;
        }
        for (i = 0; i < RESERVE_SIZE; ++i) {
            clip[i] = clip_step_in_position[i];
        }
        ++frame_index;
    }


    destroy(spectrogram);
    for (i = 0; i < SPECTROGRAM_W; ++i) {
        free(magnitude[i]);
    }
    free(magnitude);

    err = Pa_CloseStream(stream);
    if(err != paNoError) {
        Pa_Terminate();
        return 1;
    }

    Pa_Terminate();
    free(clip);
    return 0;
}
