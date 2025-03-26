#pragma once
#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_color.h>

#include<fftw3.h>

#include<stdlib.h>
#include<complex.h>
#include<math.h>

#define FFT_BUFFER_SIZE 8192
#define FFT_OUTPUT_SIZE (FFT_BUFFER_SIZE / 2) + 1

typedef struct ch_vis_buffer ch_vis_buffer;

struct ch_vis_buffer {
    float left_waveform[1024];
    float right_waveform[1024];
    float left_fft_buffer[FFT_BUFFER_SIZE];
    float right_fft_buffer[FFT_BUFFER_SIZE];
    float left_fft_waveform[FFT_BUFFER_SIZE];
    float right_fft_waveform[FFT_BUFFER_SIZE];
    fftwf_complex left_fft_output[FFT_OUTPUT_SIZE];
    fftwf_complex right_fft_output[FFT_OUTPUT_SIZE];
    float left_frequencies[FFT_OUTPUT_SIZE];
    float right_frequencies[FFT_OUTPUT_SIZE];
    //float phases[FFT_OUTPUT_SIZE];
    fftwf_plan left_plan;
    fftwf_plan right_plan;
    size_t write_pos;
};

inline ch_vis_buffer* ch_vis_buffer_create() {
    ch_vis_buffer* vb = malloc(sizeof(ch_vis_buffer));    
    memset(vb, 0, sizeof(ch_vis_buffer));

    vb->left_plan = fftwf_plan_dft_r2c_1d(FFT_BUFFER_SIZE, vb->left_fft_waveform, vb->left_fft_output, FFTW_MEASURE);
    vb->right_plan = fftwf_plan_dft_r2c_1d(FFT_BUFFER_SIZE, vb->right_fft_waveform, vb->right_fft_output, FFTW_MEASURE);

    return vb;
}

// assumes 1024 STEREO samples at a time
void ch_vis_buffer_add_samples(ch_vis_buffer* vb, const float* samples);
void ch_vis_buffer_order_samples(ch_vis_buffer* vb);

void ch_vis_buffer_do_fft(ch_vis_buffer* vb);

double A_weighting(double freq); 

void draw_waveforms(ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *texture, ch_vis_buffer* vb);
void draw_frequency_bins(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP* texture, ch_vis_buffer* vis_buffer);