#include "Spectrogram.h"
#include "utilities.h"

#include <fftw3.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <portaudio.h>

#include <cmath>

int main() 
{
    PaStreamParameters input_parameters;
    PaError error;
    PaStream* stream;

    error = Pa_Initialize();
    if (error != paNoError) 
    {
        Pa_Terminate();
        return 1;
    }

    input_parameters.device = Pa_GetDefaultInputDevice();
    if (input_parameters.device == paNoDevice) 
    {
        Pa_Terminate();
        return 1;
    }

    input_parameters.channelCount = 1;
    input_parameters.sampleFormat = paFloat32;
    input_parameters.suggestedLatency = Pa_GetDeviceInfo(input_parameters.device)->defaultLowInputLatency;
    input_parameters.hostApiSpecificStreamInfo = nullptr;
    error = Pa_OpenStream(&stream, &input_parameters, nullptr, SAMPLE_RATE, 1024, paClipOff, nullptr, nullptr);
    if (error != paNoError) 
    {
        Pa_Terminate();
        return 1;
    }

    error = Pa_StartStream(stream);
    if (error != paNoError) 
    {
        Pa_Terminate();
        return 1;
    }

    int length = SPECTROGRAM_HEIGHT * (SAMPLE_RATE / 20 / SPECTROGRAM_HEIGHT + 1);
    Spectrogram* spectrogram = new Spectrogram(length);

    float** magnitude = new float*[SPECTROGRAM_WIDTH];
    for (int i = 0; i < SPECTROGRAM_WIDTH; ++i) 
    {
        magnitude[i] = new float[SPECTROGRAM_HEIGHT];
    }

    float* clip = new float[WINDOW_SIZE];
    float* clip_fill_in_position = clip + RESERVE_SIZE;
    float* clip_step_in_position = clip + STEP_SIZE;

    cv::Mat image(SPECTROGRAM_HEIGHT, SPECTROGRAM_WIDTH, CV_8UC3);
    unsigned char colours[3] = {0, 0, 0};

    const double spectrogram_radius = 0.95 * (SPECTROGRAM_HEIGHT / 2.0);
    const double minimum_amplitude = pow(10.0, -180.0 / 20.0);

    while (true) 
    {
        Pa_ReadStream(stream, clip_fill_in_position, STEP_SIZE);

        for (int j = 0; j < SPECTROGRAM_WIDTH; ++j) 
        {
            int data_length = 2 * length;
            double* data = spectrogram->get_time_domain();
            memset(data, 0, data_length * sizeof(double));

            int start = (j * WINDOW_SIZE) / SPECTROGRAM_WIDTH - length;
            if (start >= 0) 
            {
                int copy_length = 0;
                if (start + data_length > WINDOW_SIZE) 
                {
                    copy_length = WINDOW_SIZE - start;
                }
                else 
                {
                    copy_length = data_length;
                }
                for (int i = 0; i < copy_length; ++i) 
                {
                    data[i] = clip[i + start];
                }
            }
            else 
            {
                start *= -1;
                data += start;
                data_length -= start;
                for (int i = 0; i < data_length; ++i) 
                {
                    data[i] = clip[i];
                }
            }
            spectrogram->calculate_magnitude();
            map_spectrogram_to_magnitude(magnitude[j], spectrogram->get_magnitude_array(), length);
        }

        for (int j = 0; j < SPECTROGRAM_WIDTH; ++j) 
        {
            for (int k = 0; k < SPECTROGRAM_HEIGHT; ++k) 
            {
                magnitude[j][k] /= 100.0;
                magnitude[j][k] = (magnitude[j][k] < minimum_amplitude) ? 
                    -180.0 : 
                    20.0 * log10(magnitude[j][k]);

                double radius = 0.95 * (SPECTROGRAM_HEIGHT / 2.0) * (1.0 - k / static_cast<double>(SPECTROGRAM_HEIGHT));
                double angle = 2.0 * M_PI * j / SPECTROGRAM_WIDTH;
                int x = SPECTROGRAM_WIDTH / 2 + radius * cos(angle);
                int y = SPECTROGRAM_HEIGHT / 2 + radius * sin(angle);

                if (x >= 0 && x < SPECTROGRAM_WIDTH && y >= 0 && y < SPECTROGRAM_HEIGHT) 
                {
                    double distance = sqrt(pow(x - SPECTROGRAM_WIDTH / 2, 2) + pow(y - SPECTROGRAM_HEIGHT / 2, 2));
                    int pixel_radius = distance < spectrogram_radius / 2.5 ? 1 : 4;
                    map_colours(magnitude[j][k], colours);
                    cv::circle(image, cv::Point(x, y), pixel_radius, cv::Scalar(colours[2], colours[1], colours[0]));
                }
            }
        }

        cv::imshow("Voice Tableau", image);
        unsigned char key = cv::waitKey(10);
        if (key == 32) 
        {
            std::time_t currentTime = std::time(nullptr);
            std::tm* localTime = std::localtime(&currentTime);
            char filename[100];
            std::strftime(filename, 100, "tableau_%Y-%m-%d_%H-%M-%S.png", localTime);
            cv::imwrite(filename, image);
        }
        if (key == 27) 
        {
            break;
        }
        for (int i = 0; i < RESERVE_SIZE; ++i) 
        {
            clip[i] = clip_step_in_position[i];
        }
    }

    delete spectrogram;
    for (int i = 0; i < SPECTROGRAM_WIDTH; ++i) 
    {
        delete[] magnitude[i];
    }
    delete[] magnitude;

    error = Pa_CloseStream(stream);
    if (error != paNoError) 
    {
        Pa_Terminate();
        return 1;
    }

    Pa_Terminate();
    delete[] clip;
    return 0;
}