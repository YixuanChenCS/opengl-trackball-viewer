#ifndef _MYLIB_H_

#define _MYLIB_H_

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
} mat4;

// Insert function signatures after this line

void print_v4(vec4);

vec4 scale_vec_mul(float a, vec4 v);
vec4 vec_vec_add(vec4 v1, vec4 v2);
vec4 vec_vec_sub(vec4 v1, vec4 v2);
float magnitude(vec4 v);
vec4 normalize(vec4 v);
float dot_product(vec4 v1, vec4 v2);
vec4 cross_product(vec4 v1, vec4 v2);
void print_m4(mat4 m);
mat4 scale_mat_mul(float a, mat4 m);
mat4 mat_mat_add(mat4 m1,mat4 m2);
mat4 mat_mat_sub(mat4 m1,mat4 m2);

mat4 mat_mat_mul(mat4 m1, mat4 m2);
mat4 minor(mat4 m);
mat4 cofactor(mat4 m);
float determinant(mat4 m, mat4 cofactor);
mat4 mat_transpose(mat4 m);
mat4 inverse(mat4 m);
vec4 mat_vec_mul(mat4 m, vec4 v);
mat4 identity();
mat4 translate(float ax, float ay, float az);
mat4 scale_m(float bx, float by, float bz);
mat4 rotate_x_rad(float theta);
mat4 rotate_y_rad(float theta);
mat4 rotate_z_rad(float theta);

// Do not put anything after this line

#endif
