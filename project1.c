#ifdef __APPLE__  // Mac OS X
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else // Linux/Windows
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "initShader.h"
#include "myLib.h"
#include "stl_loader.h"

#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))

static vec4 *vertices = NULL;
static vec4 *colors   = NULL;
static int   num_vertices = 0;

static GLuint ctm_location;
static mat4   ctm;
static int   draw_mode = 1;
static int   win_w = 800, win_h = 800;

static const float SCALE_STEP = 1.02f;
static int  tb_dragging = 0;   
static int  tb_has_last = 0;   // if last is vaild
static vec4 tb_last;           // last sphere vector
static GLint   STL1_OFFSET = 0, STL2_OFFSET = 0;
static GLsizei STL1_COUNT  = 0, STL2_COUNT  = 0;
#define PI  3.14159265358979323846f
#define TAU 6.2831853071795864769f
//sphere
#define SP_U 36
#define SP_V 36
#define SPHERE_TRI   (2 * (SP_U) * (SP_V))
#define SPHERE_COUNT (SPHERE_TRI * 3)
//torus
#define TO_U 36
#define TO_V 36
#define TORUS_TRI    (2 * (TO_U) * (TO_V))
#define TORUS_COUNT  (TORUS_TRI * 3)
//spring
#define SPR_U 360         
#define SPR_V 36          
#define SPR_TURNS 6       //total y=turns
#define SPR_HEIGHT 1.8f   //total height
#define SPR_TRI     (2 * SPR_U * SPR_V + 2 *36)
#define SPR_COUNT   (SPR_TRI * 3)

#define SPHERE_OFFSET  0
#define TORUS_OFFSET   (SPHERE_OFFSET + SPHERE_COUNT)
#define SPRING_OFFSET  (TORUS_OFFSET  + TORUS_COUNT)

#define TOTAL_VERTS (SPHERE_COUNT + TORUS_COUNT + SPR_COUNT)

//Map mouse to NDC, project to unit sphere to get 3D points p
static int trackball_map_to_sphere(int x, int y, vec4* out)
{
    float nx =  2.0f *(float)x / (float)win_w - 1.0f;
    float ny =  1.0f -2.0f * (float)y/(float)win_h;

    float d2 = nx*nx+ny*ny;
    if (d2 > 1.0f) {
        return 0;//out of bound
    }
    float nz = sqrtf(1.0f - d2);
    *out = (vec4){nx, ny, nz, 0.0f}; //unit pointer
    return 1;
}
static mat4 rotate_axis_angle(vec4 axis, float theta)
{
    //normalize
    float len = sqrtf(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    if (len < 1e-8f) return identity();
    float ax = axis.x / len, ay = axis.y / len, az = axis.z / len;

    float d = sqrtf(ay*ay + az*az);

    mat4 Rx_pos, Rx_neg;  // Rx, -Rx
    if (d < 1e-8f) {
        Rx_pos = identity();
        Rx_neg = identity();
    } else {
        float cx = az / d;   // cos
        float sx = ay / d;   // sin
        Rx_pos.x = (vec4){1, 0,   0, 0};
        Rx_pos.y = (vec4){0, cx,  sx, 0};
        Rx_pos.z = (vec4){0,-sx,  cx, 0};
        Rx_pos.w = (vec4){0, 0,   0, 1};

        Rx_neg.x = (vec4){1, 0,  0, 0};
        Rx_neg.y = (vec4){0, cx,-sx, 0};
        Rx_neg.z = (vec4){0, sx, cx, 0};
        Rx_neg.w = (vec4){0, 0,  0, 1};
    }

    float cy = d;   // cos
    float sy = ax;  // sin

    mat4 Ry_neg, Ry_pos;   // -Ry, Ry
    Ry_neg.x = (vec4){ cy, 0,  sy, 0};
    Ry_neg.y = (vec4){  0, 1,   0, 0};
    Ry_neg.z = (vec4){-sy, 0,  cy, 0};
    Ry_neg.w = (vec4){  0, 0,   0, 1};

    Ry_pos.x = (vec4){ cy, 0, -sy, 0};
    Ry_pos.y = (vec4){  0, 1,   0, 0};
    Ry_pos.z = (vec4){ sy, 0,  cy, 0};
    Ry_pos.w = (vec4){  0, 0,   0, 1};

    float cz = cosf(theta), sz = sinf(theta);
    mat4 Rz;
    Rz.x = (vec4){ cz,  sz, 0, 0};
    Rz.y = (vec4){-sz,  cz, 0, 0};
    Rz.z = (vec4){  0, 0, 1, 0};
    Rz.w = (vec4){  0, 0, 0, 1};

    mat4 R = mat_mat_mul(Rx_neg,mat_mat_mul(Ry_pos,mat_mat_mul(Rz,mat_mat_mul(Ry_neg,Rx_pos))));

    return R;
}
static vec4 rand_color_tri(void) {
    float r = (float)(rand() % 256) / 255.0f;
    float g = (float)(rand() % 256) / 255.0f;
    float b = (float)(rand() % 256) / 255.0f;
    return (vec4){0.2f + 0.8f*r, 0.2f + 0.8f*g, 0.2f + 0.8f*b, 1.0f};
}


static void build_sphere(int slices, int stacks,vec4 **outVerts, vec4 **outCols, int *outTotal) {
    //slices: horizontal, stacks:vertical
    // number of trianges = 2 * slices * stacks
    //first slices,then stacks 
    int tri_cnt = 2 * slices * stacks;
    int total   = tri_cnt * 3;

    vec4 *v = (vec4*)malloc(total * sizeof(vec4));
    vec4 *c = (vec4*)malloc(total * sizeof(vec4));
    if (!v || !c) { fprintf(stderr, "malloc failed\n"); exit(1); }

    const float dphi = PI  / (float)stacks;  // [0, π]
    const float dth  = TAU / (float)slices;  // [0, 2π)

    // Base vector: starting from the north pole (0, 1, 0, 1)
    const vec4 base = (vec4){0.0f, 1.0f, 0.0f, 1.0f};

    int idx = 0;

    for (int i = 0; i < stacks; ++i) {
        float phi0 = i      * dphi;  // cur
        float phi1 = (i+1)  * dphi;  // next

        mat4 Rx0 = rotate_x_rad(phi0);
        mat4 Rx1 = rotate_x_rad(phi1);

        for (int j = 0; j < slices; ++j) {
            float th0 = j      * dth;   // cur
            float th1 = (j+1)  * dth;   // next

            mat4 Ry0 = rotate_y_rad(th0);
            mat4 Ry1 = rotate_y_rad(th1);

            // p(a,b) = Ry() * Rx() * base
            // Start from north pole (0,1,0)
            // First rotate around X to move along latitude
            // then rotate around Y to sweep out longitude and form the sphere
            vec4 p00 = mat_vec_mul( mat_mat_mul(Ry0, Rx0), base );
            vec4 p01 = mat_vec_mul( mat_mat_mul(Ry1, Rx0), base );
            vec4 p10 = mat_vec_mul( mat_mat_mul(Ry0, Rx1), base );
            vec4 p11 = mat_vec_mul( mat_mat_mul(Ry1, Rx1), base );

            vec4 col1 = rand_color_tri();
            vec4 col2 = rand_color_tri();
            // tri1
            v[idx] = p00; c[idx++] = col1;
            v[idx] = p10; c[idx++] = col1;
            v[idx] = p11; c[idx++] = col1;

            // tri2
            v[idx] = p00; c[idx++] = col2;
            v[idx] = p11; c[idx++] = col2;
            v[idx] = p01; c[idx++] = col2;
        }
    }

    *outVerts = v;
    *outCols  = c;
    *outTotal = total;
}
static void build_torus(int us, int vs, float R, float r,
                        vec4 **outVerts, vec4 **outCols, int *outTotal)
{
// First rotate around z axis to make a circle
// Apply translation T to shift center to (R,0,0)
// Rotate around Y axis to make the torus
    const int tri_cnt = 2 * us * vs;
    const int total   = tri_cnt * 3;

    vec4 *v = (vec4*)malloc(total * sizeof(vec4));
    vec4 *c = (vec4*)malloc(total * sizeof(vec4));
    if (!v || !c) { fprintf(stderr, "malloc failed\n"); exit(1); }

    int idx = 0;
//base vector(x--r)
    const vec4 base = (vec4){ r, 0.0f, 0.0f, 1.0f };
    const mat4 T = translate(R, 0.0f, 0.0f);

    const float du =  TAU / (float)us;
    const float dv = TAU / (float)vs;   

    for (int i = 0; i < us; ++i) {
        float u0 = i* du;
        float u1 = (i+1)* du;

        mat4 Ry0 = rotate_y_rad(u0);
        mat4 Ry1 = rotate_y_rad(u1);

        for (int j = 0; j < vs; ++j) {
            float v0 = j * dv;
            float v1 = (j+1) * dv;

            mat4 Rx0 = rotate_z_rad(v0);
            mat4 Rx1 = rotate_z_rad(v1);

            mat4 M00 = mat_mat_mul(Ry0, mat_mat_mul(T, Rx0)); // (u0, v0)
            mat4 M10 = mat_mat_mul(Ry1, mat_mat_mul(T, Rx0)); // (u1, v0)
            mat4 M01 = mat_mat_mul(Ry0, mat_mat_mul(T, Rx1)); // (u0, v1)
            mat4 M11 = mat_mat_mul(Ry1, mat_mat_mul(T, Rx1)); // (u1, v1)

            vec4 p00 = mat_vec_mul(M00, base);
            vec4 p10 = mat_vec_mul(M10, base);
            vec4 p01 = mat_vec_mul(M01, base);
            vec4 p11 = mat_vec_mul(M11, base);

            vec4 col1 = rand_color_tri();
            vec4 col2 = rand_color_tri();

            // tri1: p00, p10, p11
            v[idx] = p00; c[idx++] = col1;
            v[idx] = p10; c[idx++] = col1;
            v[idx] = p11; c[idx++] = col1;

            // tri2: p00, p11, p01
            v[idx] = p00; c[idx++] = col2;
            v[idx] = p11; c[idx++] = col2;
            v[idx] = p01; c[idx++] = col2;
        }
    }

    *outVerts = v;
    *outCols  = c;
    *outTotal = total;
}

  static void build_spring(int us, int vs, float R, float r, int turns, float height,vec4 **outVerts, vec4 **outCols, int *outTotal)
{
    const int capSeg   = 36;                 
    const int tri_cnt  = 2 * us * vs + 2 * capSeg;
    const int total    = tri_cnt * 3;

    vec4 *v = (vec4*)malloc(total * sizeof(vec4));
    vec4 *c = (vec4*)malloc(total * sizeof(vec4));
    if (!v || !c) { fprintf(stderr, "malloc failed\n"); exit(1); }

    int idx = 0;

    const vec4  base  = (vec4){ r, 0.0f, 0.0f, 1.0f };
    const float total_u = TAU * (float)turns;
    const float pitch   = height / total_u;

    const float du = total_u / (float)us;     
    const float dv = TAU / (float)vs;

    const mat4 T_R = translate(R, -0.9f, 0.0f);

    for (int i = 0; i < us; ++i) {
        float u0 =  i * du;
        float u1 = (i + 1) * du;

        mat4 Ty0 = translate(0.0f, pitch * u0, 0.0f);
        mat4 Ty1 = translate(0.0f, pitch * u1, 0.0f);

        mat4 Ry0 = rotate_y_rad(u0);
        mat4 Ry1 = rotate_y_rad(u1);

        for (int j = 0; j < vs; ++j) {
            float v0 =  j * dv;
            float v1 = (j + 1) * dv;

            mat4 Rz0 = rotate_z_rad(v0);
            mat4 Rz1 = rotate_z_rad(v1);

            mat4 M00 = mat_mat_mul(Ty0, mat_mat_mul(Ry0, mat_mat_mul(T_R, Rz0))); // (u0,v0)
            mat4 M10 = mat_mat_mul(Ty1, mat_mat_mul(Ry1, mat_mat_mul(T_R, Rz0))); // (u1,v0)
            mat4 M01 = mat_mat_mul(Ty0, mat_mat_mul(Ry0, mat_mat_mul(T_R, Rz1))); // (u0,v1)
            mat4 M11 = mat_mat_mul(Ty1, mat_mat_mul(Ry1, mat_mat_mul(T_R, Rz1))); // (u1,v1)

            vec4 p00 = mat_vec_mul(M00, base);
            vec4 p10 = mat_vec_mul(M10, base);
            vec4 p01 = mat_vec_mul(M01, base);
            vec4 p11 = mat_vec_mul(M11, base);

            vec4 col1 = rand_color_tri();
            vec4 col2 = rand_color_tri();

            // tri1
            v[idx] = p00; c[idx++] = col1;
            v[idx] = p10; c[idx++] = col1;
            v[idx] = p11; c[idx++] = col1;

            // tri2
            v[idx] = p00; c[idx++] = col2;
            v[idx] = p11; c[idx++] = col2;
            v[idx] = p01; c[idx++] = col2;
        }
    }

    const vec4 O = (vec4){0,0,0,1};
    const float da = TAU / (float)capSeg;

    {
        float u = 0.0f;
        mat4 Ty = translate(0.0f, pitch * u, 0.0f);
        mat4 Ry = rotate_y_rad(u);
        vec4 center = mat_vec_mul(mat_mat_mul(Ty, mat_mat_mul(Ry, T_R)), O);

        for (int k = 0; k < capSeg; ++k) {
            float a0 =  k   * da;
            float a1 = (k + 1) * da;

            mat4 Rz0 = rotate_z_rad(a0);
            mat4 Rz1 = rotate_z_rad(a1);

            vec4 p0 = mat_vec_mul(mat_mat_mul(Ty, mat_mat_mul(Ry, mat_mat_mul(T_R, Rz0))), base);
            vec4 p1 = mat_vec_mul(mat_mat_mul(Ty, mat_mat_mul(Ry, mat_mat_mul(T_R, Rz1))), base);

            vec4 col = rand_color_tri();
        
            v[idx] = center;c[idx++] = col;
            v[idx] = p0;  
            c[idx++] = col;
            v[idx] = p1;     
            c[idx++] = col;
        }
    }

    {
        float u = total_u;
        mat4 Ty = translate(0.0f, pitch * u, 0.0f);
        mat4 Ry = rotate_y_rad(u);
        vec4 center = mat_vec_mul(mat_mat_mul(Ty, mat_mat_mul(Ry, T_R)), O);

        for (int k = 0; k < capSeg; ++k) {
            float a0 =  k  * da;
            float a1 = (k + 1) * da;

            mat4 Rz0 = rotate_z_rad(a0);
            mat4 Rz1 = rotate_z_rad(a1);

            vec4 q0 = mat_vec_mul(mat_mat_mul(Ty, mat_mat_mul(Ry, mat_mat_mul(T_R, Rz0))), base);
            vec4 q1 = mat_vec_mul(mat_mat_mul(Ty, mat_mat_mul(Ry, mat_mat_mul(T_R, Rz1))), base);

            vec4 col = rand_color_tri();
            v[idx] = center; c[idx++] = col;
            v[idx] = q1;     
            c[idx++] = col;
            v[idx] = q0;     
            c[idx++] = col;
        }
    }
    *outVerts = v;
    *outCols  = c;
    *outTotal = total;
}

static void build_geometry_with_stl(vec4 **outVerts, vec4 **outCols, int *outTotal)
{
    vec4 *vS = NULL, *cS = NULL; int nS = 0;
    vec4 *vT = NULL, *cT = NULL; int nT = 0;
    vec4 *vP = NULL, *cP = NULL; int nP = 0;

    build_sphere(SP_U, SP_V, &vS, &cS, &nS);                         // r=1
    build_torus (TO_U, TO_V, 0.6f, 0.4f, &vT, &cT, &nT);             // R=0.6 r=0.4
    build_spring(SPR_U, SPR_V, 0.2f, 0.12f, SPR_TURNS, SPR_HEIGHT,&vP, &cP, &nP);

    vec4 *vF = NULL, *cF = NULL; int nF = 0; // CL_whole
    vec4 *vG = NULL, *cG = NULL; int nG = 0; // Little-darth-vader

    if (!load_stl("CL_whole.stl", &vF, &cF, &nF)) {
        fprintf(stderr, "Warn: cannot load CL_whole.stl, skip.\n");
        vF = cF = NULL; nF = 0;
    }
    if (!load_stl("Little-darth-vader.STL", &vG, &cG, &nG)) {
        fprintf(stderr, "Warn: cannot load Little-darth-vader.STL, skip.\n");
        vG = cG = NULL; nG = 0;
    }
    STL1_OFFSET = SPRING_OFFSET + SPR_COUNT;
    STL1_COUNT  = nF;
    STL2_OFFSET = STL1_OFFSET + STL1_COUNT;
    STL2_COUNT  = nG;

    int total = STL2_OFFSET + STL2_COUNT;

    vec4 *v = (vec4*)malloc(total * sizeof(vec4));
    vec4 *c = (vec4*)malloc(total * sizeof(vec4));
    if (!v || !c) { fprintf(stderr, "malloc failed\n"); exit(1); }

    // sphere
    for (int i = 0; i < nS; ++i) { v[SPHERE_OFFSET + i] = vS[i]; c[SPHERE_OFFSET + i] = cS[i]; }
    // torus
    for (int i = 0; i < nT; ++i) { v[TORUS_OFFSET  + i] = vT[i]; c[TORUS_OFFSET + i] = cT[i]; }
    // spring
    for (int i = 0; i < nP; ++i) { v[SPRING_OFFSET + i] = vP[i]; c[SPRING_OFFSET + i] = cP[i]; }
    // STL #1 (CL_whole)
    for (int i = 0; i < nF; ++i) { v[STL1_OFFSET   + i] = vF[i]; c[STL1_OFFSET + i] = cF[i]; }
    // STL #2 (Darth Vader)
    for (int i = 0; i < nG; ++i) { v[STL2_OFFSET   + i] = vG[i]; c[STL2_OFFSET + i] = cG[i]; }

    free(vS); free(cS);
    free(vT); free(cT);
    free(vP); free(cP);
    if (vF) free(vF); if (cF) free(cF);
    if (vG) free(vG); if (cG) free(cG);

    *outVerts  = v;
    *outCols   = c;
    *outTotal  = total;
}
static void special_keys(int key, int x, int y)
{
    mat4 S_up= scale_m(SCALE_STEP,SCALE_STEP,SCALE_STEP);
    mat4 S_down = scale_m(1.0f/SCALE_STEP,1.0f/SCALE_STEP,1.0f/SCALE_STEP);

    switch (key) {
    case GLUT_KEY_UP:
    case GLUT_KEY_RIGHT:
        ctm = mat_mat_mul(S_up, ctm);
        break;
    case GLUT_KEY_DOWN:
    case GLUT_KEY_LEFT:
        ctm = mat_mat_mul(S_down, ctm);
        break;
    default:
        return;
    }
    glutPostRedisplay();
}
static void init(void) {
    srand(42); 
     //build_geometry(&vertices, &colors, &num_vertices);
     build_geometry_with_stl(&vertices, &colors, &num_vertices);
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);
    GLuint vao;
#ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
#else
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#endif
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(vec4)+num_vertices * sizeof(vec4), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,       num_vertices * sizeof(vec4), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, num_vertices * sizeof(vec4), num_vertices * sizeof(vec4), colors);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(num_vertices * sizeof(vec4)));

    ctm_location = glGetUniformLocation(program, "ctm");
    ctm = identity();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1, 0);
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK,  GL_LINE);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat*)&ctm);

    if (draw_mode == 1) { // Sphere
        glDrawArrays(GL_TRIANGLES, SPHERE_OFFSET, SPHERE_COUNT);
    }
    if (draw_mode == 2) { // Torus
        glDrawArrays(GL_TRIANGLES, TORUS_OFFSET, TORUS_COUNT);
    }
    if (draw_mode == 3) { // Spring
        glDrawArrays(GL_TRIANGLES, SPRING_OFFSET, SPR_COUNT);
    }
    if (draw_mode == 4 && STL1_COUNT > 0) { // CL
        glDrawArrays(GL_TRIANGLES, STL1_OFFSET, STL1_COUNT);
    }
    if (draw_mode == 5 && STL2_COUNT > 0) { // Little-darth-vader
        glDrawArrays(GL_TRIANGLES, STL2_OFFSET, STL2_COUNT);
    }
    glutSwapBuffers();
}

static void keyboard(unsigned char key, int x, int y) {
    if (key == 'q' || key == 27) exit(0);

    if (key == '+') {
        ctm = mat_mat_mul(scale_m(SCALE_STEP, SCALE_STEP, SCALE_STEP), ctm);
    } else if (key == '-') {
        ctm = mat_mat_mul(scale_m(1.0f/SCALE_STEP, 1.0f/SCALE_STEP, 1.0f/SCALE_STEP), ctm);
    } else if (key == '1') {
        draw_mode = 1;           // sphere
    } else if (key == '2') {
        draw_mode = 2;           // torus
    } else if (key == '3') {
        draw_mode = 3;           // spring
    } else if (key == 'f') {
        draw_mode = 4;           // CL
    } else if (key == 'g') {
        draw_mode = 5;     // Little-darth-vader
    }
    glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            tb_dragging = 1;
            tb_has_last = trackball_map_to_sphere(x, y, &tb_last);
        } else {
            tb_dragging = 0;
            tb_has_last = 0;
        }
    }
    glutPostRedisplay();
}

static void motion(int x, int y)
{
    if (!tb_dragging) return;
    vec4 curr;
    if (!trackball_map_to_sphere(x, y, &curr)) {
        tb_has_last = 0;
        return;
    }
    if (!tb_has_last) {
// If there was no valid last point
// set the current point as the new tb_last and establish a starting point
        tb_last = curr;
        tb_has_last = 1;
        return;
    }

    vec4 axis = cross_product(tb_last, curr);  
    float dot = tb_last.x*curr.x + tb_last.y*curr.y + tb_last.z*curr.z; 
    if (dot >  1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    float ax = axis.x, ay = axis.y, az = axis.z;
    float s = sqrtf(ax*ax + ay*ay + az*az);   //magnitude of cross product
    if (s > 1e-8f) {
        float theta = atan2f(s, dot);         
        mat4 R = rotate_axis_angle(axis, theta);
        ctm = mat_mat_mul(R, ctm);
    }
    tb_last = curr;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(win_w, win_h);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Project 1");

#ifndef __APPLE__
    glewInit();
#endif
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSpecialFunc(special_keys);
    glutMainLoop();
    return 0;
}
