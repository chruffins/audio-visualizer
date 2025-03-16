#pragma once
#include<sqlite3.h>

#include<stdio.h>

#define INIT_SQL "CREATE TABLE IF NOT EXISTS songs (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "song_path TEXT NOT NULL, "\
    "title TEXT, " \
    "artist_id INTEGER, " \
    "album_id INTEGER, " \
    "comment TEXT, " \
    "duration INTEGER, " \
    "FOREIGN KEY(artist_id) REFERENCES artists(id), " \
    "FOREIGN KEY(album_id) REFERENCES albums(id));" \
    \
    "CREATE TABLE IF NOT EXISTS artists (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL, " \
    "picture_path TEXT);" \
    \
    "CREATE TABLE IF NOT EXISTS albums (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL, " \
    "artist_id TEXT NOT NULL, " \
    "year INTEGER, " \
    "picture_path TEXT);" \
    \
    "CREATE TABLE IF NOT EXISTS genres(" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "name TEXT NOT NULL," \
    "parent_id INTEGER" \
    ");" \
    \
    "CREATE TABLE IF NOT EXISTS playlists (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "picture_path TEXT," \
    "name TEXT NOT NULL);" \
    \
    "CREATE TABLE IF NOT EXISTS song_artists(" \
    "song_id INTEGER NOT NULL, " \
    "artist_id INTEGER NOT NULL, " \
    "FOREIGN KEY(song_id) REFERENCES songs(id), " \
    "FOREIGN KEY(artist_id) REFERENCES artists(id), " \
    "PRIMARY KEY(song_id, artist_id)" \
    ");" \
    \
    "CREATE TABLE IF NOT EXISTS album_artists(" \
    "album_id INTEGER NOT NULL, " \
    "artist_id INTEGER NOT NULL, " \
    "FOREIGN KEY(album_id) REFERENCES albums(id), " \
    "FOREIGN KEY(artist_id) REFERENCES artists(id), " \
    "PRIMARY KEY(album_id, artist_id)" \
    ");" \
    \
    "CREATE TABLE IF NOT EXISTS song_genres (" \
    "song_id INTEGER NOT NULL, " \
    "genre_id INTEGER NOT NULL, " \
    "FOREIGN KEY(song_id) REFERENCES songs(id), " \
    "FOREIGN KEY(genre_id) REFERENCES genres(id), " \
    "PRIMARY KEY(song_id, genre_id));" \
    \
    "CREATE TABLE IF NOT EXISTS playlist_songs (" \
    "playlist_id INTEGER NOT NULL, " \
    "song_id INTEGER NOT NULL, " \
    "position INTEGER NOT NULL, " \
    "FOREIGN KEY(playlist_id) REFERENCES playlists(id), " \
    "FOREIGN KEY(song_id) REFERENCES songs(id), " \
    "PRIMARY KEY(playlist_id, song_id));"

int init_database(sqlite3* db);

int add_genre(sqlite3* db, const char* name);
int add_artist(sqlite3* db, const char* name, const char* picture_path);
int add_album(sqlite3* db, const char* album_name, int artist_id);
int add_song(sqlite3* db, const char* song_path, const char* title, int artist_id, int album_id, const char* comment, int duration);
int add_playlist(sqlite3* db, const char* name, const char* picture_path);