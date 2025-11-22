#include "stl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
static vec4 rand_color_tri(void) {
    float r = (float)(rand() % 256) / 255.0f;
    float g = (float)(rand() % 256) / 255.0f;
    float b = (float)(rand() % 256) / 255.0f;
    return (vec4){0.2f + 0.8f*r, 0.2f + 0.8f*g, 0.2f + 0.8f*b, 1.0f};
}
typedef struct {
    vec4* data;
    int   size;      
    int   capacity;  
} Vec4Dyn;

static void v4_init(Vec4Dyn* a) { a->data=NULL; a->size=0; a->capacity=0; }

static int v4_push(Vec4Dyn* a, vec4 v) {
    if (a->size == a->capacity) {
        int newcap = (a->capacity == 0) ? 1024 : (a->capacity * 2);
        vec4* p = (vec4*)realloc(a->data, newcap * sizeof(vec4));
        if (!p) return 0;
        a->data = p;
        a->capacity = newcap;
    }
    a->data[a->size++] = v;
    return 1;
}

static void compute_bbox(const Vec4Dyn* a, vec4* minv, vec4* maxv) {
    if (a->size <= 0) { *minv=(vec4){0,0,0,1}; *maxv=(vec4){0,0,0,1}; return; }
    vec4 mn = a->data[0], mx = a->data[0];
    for (int i=1;i<a->size;i++){
        vec4 p = a->data[i];
        if (p.x < mn.x) mn.x = p.x; if (p.x > mx.x) mx.x = p.x;
        if (p.y < mn.y) mn.y = p.y; if (p.y > mx.y) mx.y = p.y;
        if (p.z < mn.z) mn.z = p.z; if (p.z > mx.z) mx.z = p.z;
    }
    *minv = mn; *maxv = mx;
}

static void recenter_and_scale(Vec4Dyn* a) {
    if (a->size <= 0) return;
    vec4 mn, mx; compute_bbox(a, &mn, &mx);
    float cx = 0.5f*(mn.x + mx.x);
    float cy = 0.5f*(mn.y + mx.y);
    float cz = 0.5f*(mn.z + mx.z);

    float sx = (mx.x - mn.x);
    float sy = (mx.y - mn.y);
    float sz = (mx.z - mn.z);
    float maxSide = sx;
    if (sy > maxSide) maxSide = sy;
    if (sz > maxSide) maxSide = sz;
    if (maxSide <= 1e-7f) maxSide = 1.0f;

    float scale = 1.8f / maxSide;

    for (int i=0;i<a->size;i++){
        vec4 p = a->data[i];
        p.x = (p.x - cx) * scale;
        p.y = (p.y - cy) * scale;
        p.z = (p.z - cz) * scale;
        p.w = 1.0f;
        a->data[i] = p;
    }
}

static int looks_like_ascii(FILE* fp, long filesize) {

    char head[6] = {0};
    size_t n = fread(head, 1, 5, fp);
    if (n < 5) return 0;
    
    fseek(fp, 0, SEEK_SET);
    if (strncmp(head, "solid", 5) != 0) return 0;
    return 1;
}
static int parse_ascii_stl(FILE* fp, Vec4Dyn* verts, Vec4Dyn* cols) {
    char line[512];
    int vcount_in_face = 0;
    vec4 tri[3];

    while (fgets(line, sizeof(line), fp)) {
        char* s = line;
        while (*s && isspace((unsigned char)*s)) ++s;

        if (strncmp(s, "vertex", 6) == 0) {
            float x,y,z;
            if (sscanf(s+6, "%f %f %f", &x, &y, &z) == 3) {
                tri[vcount_in_face++] = (vec4){x, y, z, 1.0f};
                if (vcount_in_face == 3) {
                    vec4 col = rand_color_tri();
                    if (!v4_push(verts, tri[0])) return 0;
                    if (!v4_push(verts, tri[1])) return 0;
                    if (!v4_push(verts, tri[2])) return 0;

                    if (!v4_push(cols, col)) return 0;
                    if (!v4_push(cols, col)) return 0;
                    if (!v4_push(cols, col)) return 0;

                    vcount_in_face = 0;
                }
            }
        }

    }
    return 1;
}

#pragma pack(push,1)
typedef struct {
    float nx, ny, nz;
    float vx1, vy1, vz1;
    float vx2, vy2, vz2;
    float vx3, vy3, vz3;
    unsigned short attr;
} BinTri;
#pragma pack(pop)

static int parse_binary_stl(FILE* fp, long filesize, Vec4Dyn* verts, Vec4Dyn* cols) {
    unsigned char header[80];
    if (fread(header, 1, 80, fp) != 80) return 0;

    unsigned int ntri = 0;
    if (fread(&ntri, 4, 1, fp) != 1) return 0;

    long expect = 84L + 50L * (long)ntri;
    if (filesize >= 0 && filesize < expect) {

    }

    for (unsigned int i=0; i<ntri; ++i) {
        BinTri bt;
        size_t n = fread(&bt, 1, sizeof(BinTri), fp);
        if (n != sizeof(BinTri)) break;

        vec4 p0 = (vec4){bt.vx1, bt.vy1, bt.vz1, 1.0f};
        vec4 p1 = (vec4){bt.vx2, bt.vy2, bt.vz2, 1.0f};
        vec4 p2 = (vec4){bt.vx3, bt.vy3, bt.vz3, 1.0f};

        vec4 col = rand_color_tri();

        if (!v4_push(verts, p0)) return 0;
        if (!v4_push(verts, p1)) return 0;
        if (!v4_push(verts, p2)) return 0;

        if (!v4_push(cols, col)) return 0;
        if (!v4_push(cols, col)) return 0;
        if (!v4_push(cols, col)) return 0;
    }
    return 1;
}

int load_stl(const char* path, vec4** outVerts, vec4** outCols, int* outCount) {
    *outVerts = NULL; *outCols = NULL; *outCount = 0;

    FILE* fp = fopen(path, "rb");
    if (!fp) { fprintf(stderr, "Cannot open STL file: %s\n", path); return 0; }

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    Vec4Dyn verts, cols; v4_init(&verts); v4_init(&cols);

    int ok = 0;
    ok = parse_binary_stl(fp, filesize, &verts, &cols);
    if (!ok || verts.size == 0) {
        free(verts.data); free(cols.data);
        v4_init(&verts); v4_init(&cols);

        fseek(fp, 0, SEEK_SET);
        ok = parse_ascii_stl(fp, &verts, &cols);

        if (!ok || verts.size == 0) {
            free(verts.data); free(cols.data);
            fclose(fp);
            fprintf(stderr, "Failed to parse STL: %s\n", path);
            return 0;
        }
    }

    fclose(fp);
    recenter_and_scale(&verts);
    *outVerts = verts.data;
    *outCols  = cols.data;
    *outCount = verts.size;
    return 1;
}