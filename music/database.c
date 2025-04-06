#include "database.h"

static int execute_sql(sqlite3* db, const char* sql);
static int add_to_junction_table(sqlite3* db, const char* table_name, const char* col1, const char* col2, int id1, int id2);
static int get_last_position_in_playlist(sqlite3* db, int playlist_id);

/* don't use this for ANYTHING parameterized */
static int execute_sql(sqlite3* db, const char* sql) {
    char* err_msg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "SQL executed successfully.\n");
    }

    return rc;
}

/* this function will only be called in contexts guaranteed to not have SQL injections... right? */
static int add_to_junction_table(sqlite3* db, const char* table_name, const char* col1, const char* col2, int id1, int id2) {
    char sql[256];
    sqlite3_stmt* stmt;
    int rc = snprintf(sql, sizeof(sql), "INSERT INTO %s (%s, %s) VALUES (?, ?)", table_name, col1, col2);

    if (rc >= 256) {
        // fail it
        return -1;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare %s statement: %s\n", table_name, sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, id1);
    sqlite3_bind_int(stmt, 2, id2);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute %s statement: %s\n", table_name, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

static int get_last_position_in_playlist(sqlite3* db, int playlist_id) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT MAX(position) FROM playlist_songs WHERE playlist_id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int position = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare get position statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, playlist_id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
            position = sqlite3_column_int(stmt, 0);
        } else {
            position = 1; // yeah we're one indexing, problem?
        }
    } else {
        fprintf(stderr, "get max position query failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return position;
}

int init_database(sqlite3* db) {
    char* err_msg = NULL;
    int retcode = sqlite3_exec(db, INIT_SQL, 0, 0, &err_msg);

    if (retcode != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    printf("database initialized!\n");

    return 0;
}

int add_genre(sqlite3 *db, const char *name) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO genres (name) VALUES (?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare genre statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute genre statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

int add_playlist(sqlite3 *db, const char *name, const char *picture_path, const char *description) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO playlists (name, picture_path, desc) VALUES (?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare playlist statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, picture_path, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, description, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute playlist statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

int add_artist(sqlite3 *db, const char *name, const char *picture_path, const char *description) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO artists (name, picture_path, desc) VALUES (?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare artist statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, picture_path, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, description, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute artist statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

int add_album(sqlite3 *db, const char *name, int year, const char *picture_path)
{
    // year <= 0 is considered invalid
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO albums (name, year, picture_path) VALUES (?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare album statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, year > 0 ? year : 0);
    sqlite3_bind_text(stmt, 3, picture_path, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute album statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

int add_song(sqlite3 *db, const char *song_path, const char *title, int album_id, int track, const char *comment, int duration) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO songs (song_path, title, album_id, track, comment, duration)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare song statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, song_path, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, title, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, album_id);
    sqlite3_bind_int(stmt, 4, track > 0 ? track : 0);
    sqlite3_bind_text(stmt, 5, comment, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, duration > 0 ? track : 0);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute song statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

int add_song_artist(sqlite3* db, int song_id, int artist_id) {
    return add_to_junction_table(db, "song_artists", "song_id", "artist_id", song_id, artist_id);
}

int add_album_artist(sqlite3* db, int album_id, int artist_id) {
    return add_to_junction_table(db, "album_artists", "album_id", "artist_id", album_id, artist_id);
}

int add_song_genre(sqlite3* db, int song_id, int genre_id) {
    return add_to_junction_table(db, "song_genres", "song_id", "genre_id", song_id, genre_id);
}

int add_playlist_song(sqlite3* db, int playlist_id, int song_id, int position) {
    // we can't even add to junction table because we have to worry about the third field order
    // also we need to grab order if the order isn't ordered (means that we will insert immediately after)
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO playlist_songs (playlist_id, song_id, position) VALUES (?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    // means that we'll need to determine the position for ourselves
    if (position <= 0) {
        position = get_last_position_in_playlist(db, playlist_id);
        if (position == -1) {
            fprintf(stderr, "failed to find good position value: %s\n", sqlite3_errmsg(db));
            return -1;
        }
    }

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare playlist_songs statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, playlist_id);
    sqlite3_bind_int(stmt, 2, song_id);
    sqlite3_bind_int(stmt, 3, position);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute playlist_songs statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return position;
}

int get_artist_id_by_name(sqlite3 *db, const char *name) {
    static const char* sql = "SELECT id FROM artists WHERE name = ?";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare get artist by name statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int artist_id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return artist_id;
    } else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    } else {
        fprintf(stderr, "failed to execute get artist by name statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
}

ch_playlist* get_playlist_from_stmt(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    const char* name = strdup((const char*)sqlite3_column_text(stmt, 1));
    const char* picture_path = strdup((const char*)sqlite3_column_text(stmt, 2));
    const char* description = strdup((const char*)sqlite3_column_text(stmt, 3));
    ch_playlist* playlist = ch_playlist_create(name, picture_path, description);
    playlist->id = id;

    return playlist;
}

size_t count_songs_in_playlist(sqlite3* db, int playlist_id) {
    static const char* sql = "SELECT COUNT(*) FROM playlist_songs WHERE playlist_id = ?";

    sqlite3_stmt *stmt;
    size_t count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "playlist song count failed: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, playlist_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

ch_playlist* get_playlist_by_id(sqlite3* db, int id) {
    static const char* playlist_sql = "SELECT id, name, picture_path, desc FROM playlists WHERE id = ?";
    static const char* playlist_song_sql = "SELECT songs.id, songs.song_path, songs.title, songs.album_id, songs.track, songs.comment, songs.duration"
        "FROM playlist_songs JOIN songs ON playlist_songs.song_id = songs.id WHERE playlist_songs.playlist_id = ?";
    sqlite3_stmt* stmt;
    ch_playlist* playlist;

    int rc = sqlite3_prepare_v2(db, playlist_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare get artist by name statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(db, 1, id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        fprintf(stderr, "searched for playlist id %d which does not exist in db!\n", id);
        sqlite3_finalize(stmt);
        return NULL;
    } else if (rc != SQLITE_ROW) {
        fprintf(stderr, "failed to execute get playlist by id statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    // we have a row, pull out the information now
    playlist = get_playlist_from_stmt(stmt);
    // now we can just finalize this stmt, right?
    sqlite3_finalize(stmt);

    // now we need to grab the songs
    size_t num_songs = count_songs_in_playlist(db, id); // independent statement stuff goes inside here
    ch_song_vec_reserve(&playlist->songs, num_songs+1);

    int rc = sqlite3_prepare_v2(db, playlist_song_sql, -1, &stmt, NULL);



    sqlite3_finalize(stmt);
    return playlist;
}

ch_album* get_album_by_id(sqlite3* db, int id) {

}

ch_song* get_song_from_stmt(sqlite3_stmt* stmt) {
    // songs.id, songs.song_path, songs.title, songs.album_id, songs.track, songs.comment, songs.duration
    ch_song* s = malloc(sizeof(ch_song));
    if (!s) {
        fprintf(stderr, "exited during get_song_from_stmt because of failed malloc\n");
        exit(1);
        //return NULL;
    }

    s->id = sqlite3_column_int(stmt, 0);
    s->filename = strdup(sqlite3_column_text(stmt, 1));
    s->metadata = ch_metadata_create();
    
    s->metadata.title = strdup(sqlite3_column_text(stmt, 2));
    // need to do something about album id...........
    s->metadata.track = sqlite3_column_int(stmt, 4);
    s->metadata.comment = strdup(sqlite3_column_text(stmt, 5));
    s->metadata.duration = sqlite3_column_int(stmt, 6);

    return s;
}

ch_song* get_song_by_id(sqlite3* db, int id) {

}
