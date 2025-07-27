#include "event_handler.h"

#include <allegro5/allegro.h>
#include <stdio.h>

#include "inputstate.h"
#include "menu.h"
#include "app_state.h"
#include "../graphics/camera.h"

void handle_display_resize(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    al_acknowledge_resize(app->display);
}

void handle_display_close(ALLEGRO_EVENT *event) {
    //ch_app_state* app = get_app_state();
    // unfinished is a local variable in main loop, so this should be handled via a pointer or callback.
    // If you want to set a flag in app_state, add a member like 'bool unfinished;' to ch_app_state.
    // For now, leave a comment.
    // app->unfinished = false;
}

void handle_audio_stream_finished(ALLEGRO_EVENT *event) {
    printf("song finished!\n");
}

void handle_key_down(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    app->input.key_down[event->keyboard.keycode] = 1;
    app->input.key_state[event->keyboard.keycode] = 1;

    if (event->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
        // mode is a local variable in main loop, so this should be handled via a pointer or callback.
        // If you want to set a flag in app_state, add a member like 'int mode;' to ch_app_state.
        // For now, leave a comment.
        // app->mode = (app->mode + 1) % 4;
        printf("mode changed\n");
    } else if (event->keyboard.keycode == ALLEGRO_KEY_SPACE) {
        al_set_audio_stream_playing(app->stream, !al_get_audio_stream_playing(app->stream));
    } else if (event->keyboard.keycode == ALLEGRO_KEY_UP) {
        al_set_audio_stream_gain(app->stream, al_get_audio_stream_gain(app->stream) + 0.1);
        printf("new gain: %f\n", al_get_audio_stream_gain(app->stream));
    } else if (event->keyboard.keycode == ALLEGRO_KEY_DOWN) {
        al_set_audio_stream_gain(app->stream, al_get_audio_stream_gain(app->stream) - 0.1);
        printf("new gain: %f\n", al_get_audio_stream_gain(app->stream));
    } else if (event->keyboard.keycode == ALLEGRO_KEY_O) {
        al_set_audio_stream_pan(app->stream, al_get_audio_stream_pan(app->stream) - 0.1 < -1 ? -1 : al_get_audio_stream_pan(app->stream) - 0.1);
        printf("new pan: %f\n", al_get_audio_stream_pan(app->stream));
    } else if (event->keyboard.keycode == ALLEGRO_KEY_P) {
        al_set_audio_stream_pan(app->stream, al_get_audio_stream_pan(app->stream) + 0.1 > 1 ? 1 : al_get_audio_stream_pan(app->stream) + 0.1);
        printf("new pan: %f\n", al_get_audio_stream_pan(app->stream));
    } else if (event->keyboard.keycode == ALLEGRO_KEY_R) {
        app->song = ch_song_vec_rand(&app->songs);

        if (app->stream) al_destroy_audio_stream(app->stream);
        app->stream = al_load_audio_stream(app->song->filename, 4, 4096);
        al_attach_audio_stream_to_mixer(app->stream, app->mixer);
    }
}

void handle_key_up(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    app->input.key_state[event->keyboard.keycode] = 0;
}

void handle_timer(ALLEGRO_EVENT *event) {
    // Timer logic is more complex and involves particles, etc.
    // This should be refactored later.
    // For now, leave empty or add a comment.
}

void handle_menu_click(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    switch (event->user.data1) {
        case MENU_FILE_OPEN:
            // filepath is a local variable in main loop, so this should be handled via a pointer
            // or callback. If you want to set a flag in app_state, add a member like 'char* filepath;' to ch_app_state.
            // For now, leave a comment.
            // app->filepath = choose_audio_file();
            // if (app->filepath) {
        case MENU_FILE_FOLDER:
            // filepath = choose_album_folder();
            // if (filepath) {
            //     import_music_from_folder_recursive(app->db, filepath);
            //     free(filepath);
            // }
            break;
        case MENU_FILE_EXIT:
            // app->unfinished = false;
            break;
        case DEBUG_MENU_LIST_ALBUMS:
            // print_albums(app->db);
            break;
        case DEBUG_MENU_LIST_ARTISTS:
            // print_artists(app->db);
            break;
        case DEBUG_MENU_LIST_GENRES:    
            // print_genres(app->db);
            break;
        case 91: // About
            // Show about dialog or similar functionality
            al_show_native_message_box(app->display, "About", "Audio Visualizer", "This is an audio visualizer application.", NULL, ALLEGRO_MESSAGEBOX_OK_CANCEL);
            break;
        default:
            printf("Unhandled menu item: %ld\n", event->user.data1);
            break;
    }
}

void handle_mouse_button_down(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    if (event->mouse.button == 1) {
        printf("mouse down @ %d %d!\n", event->mouse.x, event->mouse.y);
    }
    app->input.mouse_buttons[event->mouse.button] = 1;

    ch_ray ray;
    camera_screen_point_to_ray(&app->cam, event->mouse.x, event->mouse.y, app->width, app->height, &ray);
}

void handle_mouse_button_up(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    if (event->mouse.button == 1) {
        printf("mouse up @ %d %d!\n", event->mouse.x, event->mouse.y);
    }
    app->input.mouse_buttons[event->mouse.button] = 0;
}

void handle_mouse_axes(ALLEGRO_EVENT *event) {
    ch_app_state* app = get_app_state();
    app->input.mouse_dx += event->mouse.dx;
    app->input.mouse_dy += event->mouse.dy;
}
