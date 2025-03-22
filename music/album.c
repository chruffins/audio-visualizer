#include "album.h"

static const char* safe_str(const char* str) {
    if (str == NULL) {
        return "(null)";
    } else if (strcmp(str, "") == 0) {
        return "(blank)";
    } else {
        return str;
    }
}

void ch_album_print(ch_album* album) {
    printf("Album: %s\n", album->name);
    printf("Artist: %s\n", album->album_artist);
    printf("Picture path: %s\n", safe_str(album->picture_path));
    printf("# Songs: %zu\n", album->songs.size);
}

void ch_album_sort(ch_album* album) {
    qsort(album->songs.data, album->songs.size, sizeof(ch_song*), ch_song_track_comparator);
}