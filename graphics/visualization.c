#include "visualization.h"

void ch_vis_buffer_add_samples(ch_vis_buffer *vb, const float *samples) {
    size_t pos = vb->write_pos;
    
    for (int i = 0; i < 1024; i++) {
        vb->left_waveform[i] = samples[i*2];
        vb->right_waveform[i] = samples[i*2 + 1];

        vb->left_fft_buffer[i] = samples[i*2];
        vb->right_fft_buffer[i] = samples[i*2 + 1];
        pos = (pos + 1) % FFT_BUFFER_SIZE;
    }

    vb->write_pos = pos;
}

void ch_vis_buffer_order_samples(ch_vis_buffer* vb) {
    size_t pos = vb->write_pos;

    for (size_t i = 0; i < FFT_BUFFER_SIZE; i++) {
        vb->left_fft_waveform[i] = vb->left_fft_buffer[(pos + i) % FFT_BUFFER_SIZE];
        vb->right_fft_waveform[i] = vb->right_fft_buffer[(pos + i) % FFT_BUFFER_SIZE];
    }
}

void ch_vis_buffer_do_fft(ch_vis_buffer *vb)
{
    ch_vis_buffer_order_samples(vb);

    fftwf_execute(vb->left_plan);
    fftwf_execute(vb->right_plan);

    /*
    for (int i = samples; i > 0; i--) {
        sum_l += fbuf[0] * fbuf[0];
        sum_r += fbuf[1] * fbuf[1];
        fbuf += 2;
    }
    */

    for (int i = 0; i < FFT_OUTPUT_SIZE; i++) {
        float lr = vb->left_fft_output[i][0];
        float li = vb->left_fft_output[i][1];
        float rr = vb->right_fft_output[i][0];
        float ri = vb->right_fft_output[i][1];

        vb->left_frequencies[i] = sqrt(lr * lr + li * li);
        vb->right_frequencies[i] = sqrt(rr * rr + ri * ri);
        //vb->phases[i] = atan2(imag, real);
    }

    //rms_l = sqrt(sum_l / samples);
    //rms_r = sqrt(sum_r / samples);
}

double A_weighting(double freq) {
    double f_sq = freq * freq;
    double num = 12194.0 * 12194.0 * f_sq * f_sq;
    double den = (f_sq + 20.6 * 20.6) * sqrt((f_sq + 107.7 * 107.7) * (f_sq + 737.9 * 737.9)) * (f_sq + (12194.0 * 12194.0));

    return 20.0 * log10(num / den) + 2.0;
}

void draw_waveforms(ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *texture, ch_vis_buffer* vb) {
    float width = 512;//al_get_bitmap_width(texture);
    float height = 512;//al_get_bitmap_width(texture);

    al_set_target_bitmap(texture);
    al_clear_to_color(al_map_rgba_f(1, 1, 1, 0));

    for (int i = 0; i < 1024; i++) {
        al_draw_filled_circle((float)i*width / 1024.0, (height / 2.0) + (vb->left_waveform[i] * 100), 4, al_color_hsv(i % 360, 1, 1));
    }
    al_draw_circle(width / 2.0, height / 2.0, 50, al_map_rgb(255, 0, 255), 2);
    //for (int i = 0; i < 512; i++) {
        //al_draw_line((float)i / width, (height / 2) + (vb->left_waveform[i*2] * 10), (float)(i*2+1) / width, (height / 2) + (vb->left_waveform[i*2+1] * 10), al_color_hsv(i % 360, 1, 1), 1.5);
    //}

    al_set_target_backbuffer(display);
}

void draw_frequency_bins(ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *texture, ch_vis_buffer* vb) {
    static const float width = 512;
    static const float height = 512;

    al_set_target_bitmap(texture);
    al_clear_to_color(al_map_rgba_f(1, 1, 1, 0));

    float w = width / 300.0f;
    for (int i = 0; i < 300; i++) {
        float freq = i * (44100.0 / 4096.0);
        float correction = A_weighting(freq);
        float h = 2 + (vb->left_frequencies[i] * pow(10.0f, correction / 20.0f));
        al_draw_filled_rectangle(i*w, (height / 2) - h, i*w + w, (height / 2) + h, al_color_hsv(i % 360, 1, 1));
    }

    al_set_target_backbuffer(display);
}