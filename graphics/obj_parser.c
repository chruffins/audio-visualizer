#include "obj_parser.h"

static int get_obj_counts(FILE* file, ch_obj* obj);
static int read_obj_data(FILE* file, ch_obj* obj);

static int get_obj_counts(FILE* file, ch_obj* obj) {
    if (file == NULL) return 1;

    rewind(file);
    char line_buffer[256];

    // first pass gets us the basic information but NOT face vertex count
    while (fgets(line_buffer, 256, file)) {
        if (strncmp(line_buffer, "v ", 2) == 0) {
            obj->v_count += 1;
        } else if (strncmp(line_buffer, "vt ", 3) == 0) {
            obj->vt_count += 1;
        } else if (strncmp(line_buffer, "vn ", 3) == 0) {
            obj->n_count += 1;
        } else if (strncmp(line_buffer, "f ", 2) == 0) {
            obj->face_count += 1;
        }
    }

    // second pass gets us face vertex count which we'll then immediately
    // use to allocate vertice counts and stuff
    obj->face_vertex_counts = calloc(sizeof(unsigned int), obj->face_count);

    rewind(file);
    int current_face = 0;
    int total_vertices = 0;
    while (fgets(line_buffer, 256, file)) {
        if (strncmp(line_buffer, "f ", 2) == 0) {
            // right strip the string to remove extraneous spaces
            int rstrip_done = 0;
            for (int i = strlen(line_buffer) - 1; i >= 0; i--) {
                switch (line_buffer[i])
                {
                case ' ':
                case '\r':
                case '\n': 
                    break;
                default:
                    rstrip_done = 1;
                    break;
                }
                if (rstrip_done) {
                    line_buffer[i+1] = 0;
                    break;
                }
            }

            int vertices = 0;
            for (int i = 0; line_buffer[i]; i++) {
                vertices += (line_buffer[i] == ' ');
            }
            obj->face_vertex_counts[current_face] = vertices;
            obj->triangle_count += (vertices - 2);
            total_vertices += vertices;
            current_face += 1;
        }
    }

    // now we can allocate the final piece which is face_vertices
    obj->face_vertices = calloc(sizeof(ch_obj_face_vertex), total_vertices);
    obj->face_vertex_sum = total_vertices;

    return 0;
}

static int read_obj_data(FILE* file, ch_obj* obj) {
    // pre suppose that the obj counts are filled out since get_obj_counts fills them out
    if (file == NULL) return 1;

    rewind(file);
    char line_buffer[256];

    int v = 1; // accept the one-based indexing life
    int vt = 1;
    int n = 1;
    int f = 0;
    int fv = 0;

    int tris = 0;
    int quads = 0;

    int rc;

    printf("allocating space for: %u v's %u vt's %u vn's %u f's %u fv's\n", obj->v_count + 1, obj->vt_count + 1, obj->n_count + 1, obj->face_count, obj->face_vertex_sum);

    // we can pre-allocate the other arrays now...
    obj->vertices = calloc(sizeof(ch_obj_vertex), obj->v_count + 1);
    obj->vts = calloc(sizeof(ch_obj_vt), obj->vt_count + 1);
    obj->normals = calloc(sizeof(ch_obj_normal), obj->n_count + 1);

    while (fgets(line_buffer, 256, file)) {
        if (strncmp(line_buffer, "v ", 2) == 0) {
            // reading a vertex!
            float vs[6];

            rc = sscanf(line_buffer, "v %f %f %f %f %f %f", vs, vs+1, vs+2, vs+3, vs+4, vs+5);
            obj->vertices[v].r = 1;
            obj->vertices[v].g = 1;
            obj->vertices[v].b = 1;
            switch (rc)
            {
            case 6:
                // interpret as x y z r g b (x y z gets read in the next bits)
                obj->vertices[v].r = vs[3];
                obj->vertices[v].g = vs[4];
                obj->vertices[v].b = vs[5];
            case 3:
            case 4:
                // interpret as x y z [w] (no support for w)
                obj->vertices[v].x = vs[0];
                obj->vertices[v].y = vs[1];
                obj->vertices[v].z = vs[2];
                break;
            default:
                // invalid vertex declaration, just blow up
                fprintf(stderr, "bad vertex format: %s\n", line_buffer);
                return 1;
                break;
            }

            v++;
        } else if (strncmp(line_buffer, "vt ", 3) == 0) {
            float uv[2];
            rc = sscanf(line_buffer, "vt %f %f", uv, uv+1);
            switch (rc)
            {
            case 2:
                obj->vts[vt].v = uv[1];
            case 1:
                obj->vts[vt].u = uv[0];
                break;
            default:
                fprintf(stderr, "bad vertex texture format: %s\n", line_buffer);
                return 1;
            }
            vt++;
        } else if (strncmp(line_buffer, "vn ", 3) == 0) {
            float xyz[3];
            rc = sscanf(line_buffer, "vn %f %f %f", xyz, xyz+1, xyz+2);
            switch (rc)
            {
            case 3:
                obj->normals[n].x = xyz[0];
                obj->normals[n].y = xyz[2];
                obj->normals[n].z = xyz[2];
                break;
            default:
                fprintf(stderr, "bad vertex normal format: %s\n", line_buffer);
                return 1;
            }
            n++;
        } else if (strncmp(line_buffer, "f ", 2) == 0) {
            char *token;
            char *rest = line_buffer + 2;
            int expected = obj->face_vertex_counts[f];
            int actual = 0;
            //printf("%s\n", line_buffer + 2);

            while ((token = strtok_r(rest, " \r\n", &rest))) {
                unsigned int vi = 0, vti = 0, vni = 0;
                //printf("%d\n", token[0]);
                if ((sscanf(token, "%u/%u/%u", &vi, &vti, &vni) == 3) | 
                    (sscanf(token, "%u//%u", &vi, &vni) == 2) |
                    (sscanf(token, "%u/%u", &vi, &vni) == 2) |
                    (sscanf(token, "%u", &vi) == 1)
                ) {
                    obj->face_vertices[fv].v = vi;
                    obj->face_vertices[fv].vt = vti;
                    obj->face_vertices[fv].vn = vni;
                } else {
                    fprintf(stderr, "bad face format: %s %s\n", token, rest);
                    return 1;
                }
                fv += 1;
                actual += 1;
            }

            if (expected != actual) {
                printf("didnt read full line: expected %d got %d\n", expected, actual);
                printf("%s\n", line_buffer);
            } else {
                if (expected == 3) {
                    tris += 1;
                } else if (expected == 4) {
                    quads += 1;
                }
            }

            f++;
        }
    }

    printf("read %u v's %u vt's %u vn's %u f's %u f vertices\n", v, vt, n, f, fv);
    printf("we have %d tris and %d quads\n", tris, quads);

    return 0;
}

void ch_obj_destroy(ch_obj* obj) {
    if (!obj) return;

    if (obj->vertices) {
        free(obj->vertices);
    }

    if (obj->normals) {
        free(obj->normals);
    }

    if (obj->vts) {
        free(obj->vts);
    }

    if (obj->face_vertex_counts) {
        free(obj->face_vertex_counts);
    }

    if (obj->face_vertices) {
        free(obj->face_vertices);
    }

    free(obj);
}

ch_obj* ch_obj_load(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) return NULL;
    int error;

    ch_obj* obj = calloc(sizeof(ch_obj), 1);
    if (obj == NULL) return NULL;

    error = get_obj_counts(file, obj);
    if (error) {
        ch_obj_destroy(obj);
        return NULL;
    }

    error = read_obj_data(file, obj);
    if (error) {
        ch_obj_destroy(obj);
        return NULL;
    }

    return obj;
}