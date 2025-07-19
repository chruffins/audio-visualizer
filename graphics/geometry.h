#pragma once
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_color.h>
#include<stdlib.h>
#include<stdio.h>

#include "obj_parser.h"

/**
 * @file geometry.h
 * @brief Provides structures and functions for 3D model geometry and rendering.
 */

/**
 * @typedef ch_model
 * Struct representing a 3D model.
 */
typedef struct ch_model ch_model;

/**
 * @typedef CH_VERTEX
 * Alias for ALLEGRO_VERTEX.
 */
typedef ALLEGRO_VERTEX CH_VERTEX;

/**
 * @struct ch_model
 * @brief Represents a 3D model with vertex and index buffers, texture, and rendering parameters.
 */
struct ch_model {
    ALLEGRO_VERTEX* vertex_data;                /**< Pointer to vertex data */
    ALLEGRO_VERTEX_BUFFER* vertex_buffer;       /**< Vertex buffer object */
    ALLEGRO_INDEX_BUFFER* index_buffer;         /**< Index buffer object */
    ALLEGRO_BITMAP* texture;                    /**< Texture bitmap */
    unsigned int num_vertices;                  /**< Number of vertices */
    unsigned int num_indices;                   /**< Number of indices */
    ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags;     /**< Buffer flags for primitives */
    ALLEGRO_PRIM_TYPE prim_type;                /**< Primitive type for rendering */
};

/**
 * @brief Creates a ch_model from vertex and index data.
 * @param vertices Pointer to vertex array
 * @param n_vertices Number of vertices
 * @param indices Pointer to index array
 * @param n_indices Number of indices
 * @param texture Texture bitmap
 * @param buffer_flags Buffer flags
 * @param prim_type Primitive type
 * @return Initialized ch_model
 */
ch_model ch_model_create(ALLEGRO_VERTEX* vertices, unsigned int n_vertices, int* indices, unsigned int n_indices, 
    ALLEGRO_BITMAP* texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type);

/**
 * @brief Initializes a ch_model struct with provided data.
 * @param model Pointer to ch_model
 * @param init_vertices Pointer to vertex array
 * @param index_buffer Pointer to index array
 * @param num_vertices Number of vertices
 * @param num_indices Number of indices
 * @param texture Texture bitmap
 * @param buffer_flags Buffer flags
 * @param prim_type Primitive type
 */
void ch_model_init(ch_model* model, ALLEGRO_VERTEX* init_vertices, int* index_buffer, unsigned int num_vertices, unsigned int num_indices, 
    ALLEGRO_BITMAP* texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type);

/**
 * @brief Draws the model using its buffers and texture.
 * @param model Pointer to ch_model
 */
void ch_model_draw(ch_model* model);

/**
 * @brief Frees resources associated with the model.
 * @param model Pointer to ch_model
 */
void ch_model_free(ch_model* model); // free resources

/**
 * @brief Initializes a cube model at the given position and size.
 * @param model Pointer to ch_model
 * @param texture Texture bitmap
 * @param lwh Length/width/height of the cube
 * @param x X position
 * @param y Y position
 * @param z Z position
 */
void ch_model_init_cube(ch_model* model, ALLEGRO_BITMAP* texture, double lwh, double x, double y, double z);

/**
 * @brief Loads a model from an OBJ file.
 * @param obj_filename Path to OBJ file
 * @param buffer_flags Buffer flags
 * @return Loaded ch_model
 */
ch_model ch_model_load(char* obj_filename, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags);