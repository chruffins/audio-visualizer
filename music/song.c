#include "song.h"

static const char* SPECIAL_PROPERTIES[] = {"ALBUMARTIST", NULL};

static const char* safe_str(const char* str) {
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
    const TagLib_AudioProperties *properties = taglib_file_audioproperties(file);
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

    md->duration = taglib_audioproperties_length(properties);

    // getting some special properties now

    for (int i = 0; SPECIAL_PROPERTIES[i]; i++) {
        char **prop = taglib_property_get(file, SPECIAL_PROPERTIES[i]);
        if (prop) {
            for (int j = 0; prop[j]; j++) {
                md->album_artist = strdup(prop[j]);
                break;
            }
        }
        taglib_property_free(prop);
    }
    
    taglib_tag_free_strings();
    taglib_file_free(file);

    return 0;
}

void ch_metadata_print(ch_metadata* md) {
    printf("Title: %s\n", safe_str(md->title));
    printf("Artist: %s\n", safe_str(md->artist));
    printf("Album: %s\n", safe_str(md->album));
    printf("Album Artist: %s\n", safe_str(md->album_artist));
    printf("Genre: %s\n", safe_str(md->genre));
    printf("Year: %u\n", md->year);
    printf("Track: %u\n", md->track);
    printf("Duration: %u\n", md->duration);

    printf("Comment: %s\n", safe_str(md->title));
}

void ch_song_print(ch_song* song) {
    printf("Song\n");
    printf("Filename: %s\n", safe_str(song->filename));
    printf("Title: %s\n", safe_str(song->title));
    printf("Album ID: %d\n", song->album_id);
    printf("Track No. %d\n", song->album_id);
    printf("Comment: %s\n", safe_str(song->comment));
    printf("Duration: %d\n", song->duration);
}



int ch_song_track_comparator(const void* va, const void* vb) {
    const ch_song* a = *(ch_song**)va;
    const ch_song* b = *(ch_song**)vb;

    if (a->track < b->track) {
        return -1;
    } else if (a->track > b->track) {
        return 1;
    } else {
        return 0;
    }
}