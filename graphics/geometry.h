#pragma once
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_color.h>
#include<stdlib.h>
#include<stdio.h>

#include "obj_parser.h"
#include "vector.h"

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
 * @brief Defines a vector type for ch_model.
 */
#include "../util/vector.h"
DEF_VECTOR(ch_model)

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
    vector3 position;                           /**< Position of the model in 3D space */
    vector3 rotation;                           /**< Rotation vector (optional) */
    vector3 scale;                              /**< Scale vector (optional) */
    ALLEGRO_TRANSFORM transform;                /**< Transformation matrix for the model */
    int transform_dirty;                        /**< Flag indicating if the transform needs to be recalculated */
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

/**
 * @brief Moves the model by the specified offsets.
 * @param model Pointer to ch_model
 * @param dx Offset in X
 * @param dy Offset in Y
 * @param dz Offset in Z
 */
void ch_model_move(ch_model* model, vector3 offset);

/**
 * @brief Sets the model's transformation matrix.
 * @param model Pointer to ch_model
 * @param position New position vector
 */
void ch_model_set_position(ch_model* model, vector3 position);

/**
 * @brief Rotates the model by the specified angles (in radians).
 * @param model Pointer to ch_model
 * @param d_pitch Rotation around X axis
 * @param d_yaw Rotation around Y axis
 * @param d_roll Rotation around Z axis
 */
void ch_model_rotate(ch_model* model, vector3 angles);

/**
 * @brief Sets the model's transformation matrix.
 * @param model Pointer to ch_model
 * @param rotation New rotation vector
 */
void ch_model_set_rotation(ch_model* model, vector3 rotation);

/**
 * @brief Recalculates the model's transformation matrix from its position and rotation.
 * @param model Pointer to ch_model
 */
void ch_model_recalculate_transform(ch_model* model);

/**
 * @brief Initializes a ch_model_vec.
 * @return Initialized ch_model_vec
 */
inline ch_model_vec ch_model_vec_init() {
    ch_model_vec vec;
    VECTOR_INIT(vec);
    return vec;
}

/**
 * @brief Returns a pointer to the model at the given index.
 * @param vec Pointer to ch_model_vec
 * @param index Index of the model
 * @return Pointer to ch_model
 */
inline ch_model* ch_model_vec_at(ch_model_vec *vec, size_t index) {
    return VECTOR_AT(*vec, index);
}

/**
 * @brief Pushes a model onto the vector.
 * @param vec Pointer to ch_model_vec
 * @param model Model to add
 */
inline void ch_model_vec_push(ch_model_vec *vec, ch_model model) {
    VECTOR_PUSH(ch_model, *vec, model);
}

/**
 * @brief Reserves capacity for the vector.
 * @param vec Pointer to ch_model_vec
 * @param capacity Number of elements to reserve
 */
inline void ch_model_vec_reserve(ch_model_vec *vec, size_t capacity) {
    VECTOR_RESERVE(ch_model, *vec, capacity);
}

/**
 * @brief Frees memory used by the vector.
 * @param vec Pointer to ch_model_vec
 */
inline void ch_model_vec_free(ch_model_vec *vec) {
    VECTOR_FREE(*vec);
}