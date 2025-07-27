#pragma once
#include<sqlite3.h>
#include<stdio.h>
#include<string.h>
#include "album.h"
#include "playlist.h"
#include "song.h"

/**
 * @file database.h
 * @brief Provides database schema definitions and functions for music library management.
 */

/**
 * @def INIT_SQL
 * @brief SQL statements for initializing the database schema.
 */

/**
 * @def INIT_ALBUM_TYPES
 * @brief SQL statement for initializing album types.
 */

#define INIT_SQL "CREATE TABLE IF NOT EXISTS songs (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "song_path TEXT NOT NULL UNIQUE, "\
    "title TEXT, " \
    "album_id INTEGER, " \
    "track INTEGER, " \
    "comment TEXT, " \
    "duration INTEGER, " \
    "FOREIGN KEY(album_id) REFERENCES albums(id) ON DELETE CASCADE);" \
    \
    "CREATE TABLE IF NOT EXISTS artists (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL UNIQUE, " \
    "picture_path TEXT, " \
    "desc TEXT);" \
    \
    "CREATE TABLE IF NOT EXISTS albums (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "release_type INTEGER DEFAULT 1, " \
    "name TEXT NOT NULL, " \
    "year INTEGER, " \
    "picture_path TEXT, " \
    "artist_id INTEGER, " \
    "FOREIGN KEY(artist_id) REFERENCES artists(id) ON DELETE CASCADE);" \
    \
    "CREATE TABLE IF NOT EXISTS genres(" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL UNIQUE," \
    "parent_id INTEGER" \
    ");" \
    \
    "CREATE TABLE IF NOT EXISTS playlists (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL, " \
    "picture_path TEXT," \
    "desc TEXT);" \
    \
    "CREATE TABLE IF NOT EXISTS album_types (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL UNIQUE" \
    ");" \
    \
    "CREATE TABLE IF NOT EXISTS song_artists(" \
    "song_id INTEGER NOT NULL, " \
    "artist_id INTEGER NOT NULL, " \
    "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE, " \
    "FOREIGN KEY(artist_id) REFERENCES artists(id) ON DELETE CASCADE, " \
    "PRIMARY KEY(song_id, artist_id)" \
    ");" \
    \
    "CREATE TABLE IF NOT EXISTS song_genres (" \
    "song_id INTEGER NOT NULL, " \
    "genre_id INTEGER NOT NULL, " \
    "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE, " \
    "FOREIGN KEY(genre_id) REFERENCES genres(id) ON DELETE CASCADE, " \
    "PRIMARY KEY(song_id, genre_id));" \
    \
    "CREATE TABLE IF NOT EXISTS playlist_songs (" \
    "playlist_id INTEGER NOT NULL, " \
    "song_id INTEGER NOT NULL, " \
    "position INTEGER NOT NULL, " \
    "FOREIGN KEY(playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, " \
    "FOREIGN KEY(song_id) REFERENCES songs(id) ON DELETE CASCADE, " \
    "PRIMARY KEY(playlist_id, position));"

#define INIT_ALBUM_TYPES "INSERT INTO album_types (name) VALUES ('album'), ('ep'), ('single'), ('compilation');"

/**
 * @brief Initializes the database schema.
 * @param db Pointer to sqlite3 database
 * @return 0 on success, nonzero on failure
 */
int init_database(sqlite3* db);

/**
 * @brief Adds a genre to the database.
 * @param db Pointer to sqlite3 database
 * @param name Genre name
 * @return Genre ID or error code
 */
int add_genre(sqlite3* db, const char* name);

/**
 * @brief Adds an artist to the database.
 * @param db Pointer to sqlite3 database
 * @param name Artist name
 * @param picture_path Path to artist picture
 * @param description Artist description
 * @return Artist ID or error code
 */
int add_artist(sqlite3 *db, const char *name, const char *picture_path, const char *description);

/**
 * @brief Adds a playlist to the database.
 * @param db Pointer to sqlite3 database
 * @param name Playlist name
 * @param picture_path Path to playlist picture
 * @param description Playlist description
 * @return Playlist ID or error code
 */
int add_playlist(sqlite3* db, const char* name, const char* picture_path, const char* description);

/**
 * @brief Adds an album to the database.
 * @param db Pointer to sqlite3 database
 * @param album_name Album name
 * @param artist_id Artist ID
 * @param picture_path Path to album picture
 * @return Album ID or error code
 */
int add_album(sqlite3* db, const char* album_name, int artist_id, const char* picture_path); // TODO: add release type?

/**
 * @brief Adds a song to the database.
 * @param db Pointer to sqlite3 database
 * @param song_path Path to song file
 * @param title Song title
 * @param album_id Album ID
 * @param track Track number
 * @param comment Song comment
 * @param duration Song duration in seconds
 * @return Song ID or error code
 */
int add_song(sqlite3* db, const char* song_path, const char* title, int album_id, int track, const char* comment, int duration);

/**
 * @brief Associates a song with an artist.
 * @param db Pointer to sqlite3 database
 * @param song_id Song ID
 * @param artist_id Artist ID
 * @return 0 on success, nonzero on failure
 */
int add_song_artist(sqlite3* db, int song_id, int artist_id);

//int add_album_artist(sqlite3* db, int album_id, int artist_id);

/**
 * @brief Associates a song with a genre.
 * @param db Pointer to sqlite3 database
 * @param song_id Song ID
 * @param genre_id Genre ID
 * @return 0 on success, nonzero on failure
 */
int add_song_genre(sqlite3* db, int song_id, int genre_id);

/**
 * @brief Adds a song to a playlist at a given position.
 * @param db Pointer to sqlite3 database
 * @param playlist_id Playlist ID
 * @param song_id Song ID
 * @param position Position in playlist
 * @return Position or error code
 */
int add_playlist_song(sqlite3* db, int playlist_id, int song_id, int position);

/* ASSUMED: SQLITE_ROW, COLUMNS (id, name, picture_path, desc), DOES NOT CONSUME */
ch_playlist* get_playlist_from_stmt(sqlite3_stmt* stmt);

/**
 * @brief Counts the number of songs in a playlist.
 * @param db Pointer to sqlite3 database
 * @param playlist_id Playlist ID
 * @return Number of songs
 */
size_t count_songs_in_playlist(sqlite3* db, int playlist_id);

/* ASSUMED: SQLITE_ROW, COLUMNS (id, name, picture_path, desc) */
void get_album_from_stmt(sqlite3_stmt* stmt, ch_album* album);

/**
 * @brief Creates a song struct from a prepared statement.
 * @param stmt Pointer to sqlite3_stmt
 * @return Pointer to ch_song
 */
ch_song get_song_from_stmt(sqlite3_stmt* stmt);


/* these set of functions are used to initialize the application by pulling all data from the database */

void get_artists(sqlite3* db);
void get_genres(sqlite3* db);
ch_song_vec get_songs(sqlite3* db);
ch_album_vec get_albums(sqlite3* db);

//ch_genre* get_genre_by_id(sqlite3* db, int id);
//ch_artist* get_artist_by_id(sqlite3* db, int id);
ch_playlist* get_playlist_by_id(sqlite3* db, int id);
ch_album* get_album_by_id(sqlite3* db, int id);
ch_song* get_song_by_id(sqlite3* db, int id);

/**
 * @brief Retrieves a genre ID by name.
 * @param db Pointer to sqlite3 database
 * @param name Genre name
 * @return Genre ID or error code
 */
int get_genre_by_name(sqlite3* db, const char* name);

/**
 * @brief Retrieves an artist ID by name.
 * @param db Pointer to sqlite3 database
 * @param name Artist name
 * @return Artist ID or error code
 */
int get_artist_id_by_name(sqlite3* db, const char* name);

/**
 * @brief Retrieves a playlist ID by name.
 * @param db Pointer to sqlite3 database
 * @param name Playlist name
 * @return Playlist ID or error code
 */
int get_playlist(sqlite3* db, const char* name);

/**
 * @brief Retrieves an album ID by name and artist.
 * @param db Pointer to sqlite3 database
 * @param name Album name
 * @param artist_id Artist ID
 * @return Album ID or error code
 */
int get_album_id_by_name_and_artist(sqlite3* db, const char* name, int artist_id);

/**
 * @brief Retrieves a song ID by name.
 * @param db Pointer to sqlite3 database
 * @param name Song name
 * @return Song ID or error code
 */
int get_song(sqlite3* db, const char* name);