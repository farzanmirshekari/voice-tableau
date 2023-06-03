#include "constants.h"
#include "Spectrogram.h"

#include <portaudio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
    int i, j, k;

    PaStreamParameters input_audio_parameters;
    PaError err;
    PaStream *stream;

    err = Pa_Initialize();
    if (err != paNoError) {
        printf("Error: Pa_Initialize() failed with %s\n", Pa_GetErrorText(err));
        return 1;
    }

    input_audio_parameters.device = Pa_GetDefaultInputDevice();
    if (input_audio_parameters.device == paNoDevice) {
        printf("Error: No default input device.\n");
        return 1;
    }

    input_audio_parameters.channelCount = 1;
    input_audio_parameters.sampleFormat = paFloat32;
    input_audio_parameters.suggestedLatency = Pa_GetDeviceInfo(input_audio_parameters.device)->defaultLowInputLatency;
    input_audio_parameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &input_audio_parameters, NULL, SAMPLE_RATE, 1024, paClipOff, NULL, NULL);
    if (err != paNoError) {
        printf("Error: Pa_OpenStream() failed with %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        printf("Error: Pa_StartStream() failed with %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    int const spectrogram_length = WINDOW_HEIGHT * (SAMPLE_RATE / 20 / WINDOW_HEIGHT + 1);
    Spectrogram *spectrogram = intialize(spectrogram_length);
    float ** spectrogram_magnitude = (float **) calloc(WINDOW_WIDTH, sizeof(float *));

    float * clip = (float *) calloc(WINDOW_SIZE, sizeof(float));
    float * clip_fill_in_position = clip + RESERVE_SIZE;
    float * clip_step_in_position = clip + STEP_SIZE;
    int frame_index = 0;

    Pa_ReadStream(stream, clip, RESERVE_SIZE)
    
    cv::Mat im(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
    unsigned char * im_data = im.data;
    unsigned char colour[3] = {0, 0, 0};

    while (true) {
        
    }
}