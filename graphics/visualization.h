#pragma once
/**
 * @file visualization.h
 * @brief Provides structures and functions for audio visualization, including FFT processing and waveform rendering.
 */

#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_color.h>
#include<allegro5/allegro_font.h>

#include<fftw3.h>

#include<stdlib.h>
#include<complex.h>
#include<math.h>

#include "../music/song.h"

#define FFT_BUFFER_SIZE 8192
#define FFT_OUTPUT_SIZE (FFT_BUFFER_SIZE / 2) + 1

/**
 * @typedef ch_vis_buffer
 * Struct holding buffers and FFT plans for audio visualization.
 */
typedef struct ch_vis_buffer ch_vis_buffer;

/**
 * @struct ch_vis_buffer
 * @brief Contains buffers for waveform and FFT data, FFT plans, and write position for visualization.
 */
struct ch_vis_buffer {
    float left_waveform[1024];           /**< Left channel waveform samples */
    float right_waveform[1024];          /**< Right channel waveform samples */
    float left_fft_buffer[FFT_BUFFER_SIZE];      /**< Left channel FFT input buffer */
    float right_fft_buffer[FFT_BUFFER_SIZE];     /**< Right channel FFT input buffer */
    float left_fft_waveform[FFT_BUFFER_SIZE];    /**< Left channel FFT waveform */
    float right_fft_waveform[FFT_BUFFER_SIZE];   /**< Right channel FFT waveform */
    fftwf_complex left_fft_output[FFT_OUTPUT_SIZE];   /**< Left channel FFT output */
    fftwf_complex right_fft_output[FFT_OUTPUT_SIZE];  /**< Right channel FFT output */
    float left_frequencies[FFT_OUTPUT_SIZE];      /**< Left channel frequency bins */
    float right_frequencies[FFT_OUTPUT_SIZE];     /**< Right channel frequency bins */
    //float phases[FFT_OUTPUT_SIZE];
    fftwf_plan left_plan;                        /**< FFT plan for left channel */
    fftwf_plan right_plan;                       /**< FFT plan for right channel */
    size_t write_pos;                            /**< Write position in buffer */
};

/**
 * @brief Allocates and initializes a ch_vis_buffer, including FFT plans.
 * @return Pointer to initialized ch_vis_buffer
 */
inline ch_vis_buffer* ch_vis_buffer_create() {
    ch_vis_buffer* vb = malloc(sizeof(ch_vis_buffer));    
    memset(vb, 0, sizeof(ch_vis_buffer));

    vb->left_plan = fftwf_plan_dft_r2c_1d(FFT_BUFFER_SIZE, vb->left_fft_waveform, vb->left_fft_output, FFTW_MEASURE);
    vb->right_plan = fftwf_plan_dft_r2c_1d(FFT_BUFFER_SIZE, vb->right_fft_waveform, vb->right_fft_output, FFTW_MEASURE);

    return vb;
}

/**
 * @brief Adds stereo samples to the visualization buffer.
 * @param vb Pointer to ch_vis_buffer
 * @param samples Pointer to stereo samples (1024 samples)
 */
void ch_vis_buffer_add_samples(ch_vis_buffer* restrict vb, const float* restrict samples);

/**
 * @brief Orders samples in the visualization buffer for processing.
 * @param vb Pointer to ch_vis_buffer
 */
void ch_vis_buffer_order_samples(ch_vis_buffer* restrict vb);

/**
 * @brief Performs FFT on the visualization buffer.
 * @param vb Pointer to ch_vis_buffer
 */
void ch_vis_buffer_do_fft(ch_vis_buffer* restrict vb);

/**
 * @brief Computes A-weighting for a given frequency.
 * @param freq Frequency in Hz
 * @return A-weighting value
 */
double A_weighting(double freq); 

/**
 * @brief Draws waveforms to the display using the visualization buffer.
 * @param display Pointer to ALLEGRO_DISPLAY
 * @param texture Pointer to ALLEGRO_BITMAP texture
 * @param vb Pointer to ch_vis_buffer
 */
void draw_waveforms(ALLEGRO_DISPLAY *restrict display, ALLEGRO_BITMAP *restrict texture, ch_vis_buffer* restrict vb);

/**
 * @brief Draws frequency bins to the display using the visualization buffer.
 * @param display Pointer to ALLEGRO_DISPLAY
 * @param texture Pointer to ALLEGRO_BITMAP texture
 * @param vis_buffer Pointer to ch_vis_buffer
 */
void draw_frequency_bins(ALLEGRO_DISPLAY* restrict display, ALLEGRO_BITMAP* restrict texture, ch_vis_buffer* restrict vis_buffer);

/**
 * @brief Draws song status information to the display.
 * @param display Pointer to ALLEGRO_DISPLAY
 * @param texture Pointer to ALLEGRO_BITMAP texture
 * @param font Pointer to ALLEGRO_FONT
 * @param song Pointer to ch_song
 * @param x X position
 * @param y Y position
 */
void draw_song_status(ALLEGRO_DISPLAY* restrict display, 
    ALLEGRO_BITMAP* restrict texture, ALLEGRO_FONT* restrict font, ch_song* restrict song,
    int x, int y);