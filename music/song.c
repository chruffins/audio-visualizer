#include "song.h"

static char* safe_str(char* str) {
    if (str == NULL) {
        return "(null)";
    } else if (strcmp(str, "") == 0) {
        return "(blank)";
    } else {
        return str;
    }
}

ch_metadata ch_metadata_create()
{
    return (ch_metadata){ 0 };
}

int ch_metadata_populate(ch_metadata* md, char* filename) {
    TagLib_File *file = taglib_file_new(filename);
    if (file == NULL) {
        printf("taglib couldn't find the file!\n");
        return 1;
    }

    TagLib_Tag *tag = taglib_file_tag(file);
    if (tag == NULL) {
        printf("taglib failed to make tags!\n");
        taglib_file_free(file);
        return 2;
    }

    // TODO: refactor to allow custom allocators :skull:
    md->title = strdup(taglib_tag_title(tag));
    md->artist = strdup(taglib_tag_artist(tag));
    md->album = strdup(taglib_tag_album(tag));
    md->genre = strdup(taglib_tag_genre(tag));
    md->year = taglib_tag_year(tag);
    md->track = taglib_tag_track(tag);
    md->comment = strdup(taglib_tag_comment(tag));

    taglib_tag_free_strings();
    taglib_file_free(file);

    return 0;
}

void ch_metadata_print(ch_metadata* md) {
    printf("Title: %s\n", safe_str(md->title));
    printf("Artist: %s\n", safe_str(md->artist));
    printf("Album: %s\n", safe_str(md->album));
    printf("Genre: %s\n", safe_str(md->genre));
    printf("Year: %u\n", md->year);
    printf("Track: %u\n", md->track);
    printf("Comment: %s\n", safe_str(md->title));
}

void ch_song_print(ch_song* song) {
    printf("Song\n");
    printf("Filename: %s\n", safe_str(song->filename));
    ch_metadata_print(&song->metadata);
}

ch_song *ch_song_create(char *filename) {
    ch_song* s = malloc(sizeof(ch_song));
    if (!s) {
        return NULL;
    }

    s->filename = filename;
    s->metadata = ch_metadata_create();
    if (filename) {
        ch_metadata_populate(&s->metadata, filename);
    }

    return s;
}