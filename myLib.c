#include "myLib.h"
#include <stdio.h>
#include<math.h>
#include <stdlib.h>
// Print a 4D vector
void print_v4(vec4 v) {
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n", v.x, v.y, v.z, v.w);
}
// Scalar multiplication: a * v
vec4 scale_vec_mul(float a, vec4 v){
    vec4 res;
    res.x=v.x*a;
    res.y=v.y*a;
    res.z=v.z*a;
    res.w=v.w*a;
    return res;
}
// Vector addition: v1 + v2
vec4 vec_vec_add(vec4 v1, vec4 v2){
    vec4 res;
    res.x=v1.x+v2.x;
    res.y=v1.y+v2.y;
    res.z=v1.z+v2.z;
    res.w=v1.w+v2.w;
    return res;
}
// Vector subtraction: v1 - v2
vec4 vec_vec_sub(vec4 v1, vec4 v2){
    vec4 res;
    res.x=v1.x-v2.x;
    res.y=v1.y-v2.y;
    res.z=v1.z-v2.z;
    res.w=v1.w-v2.w;
    return res;
}
// Vector magnitude 
float magnitude(vec4 v){
    float res;
    res=sqrt((pow(v.x, 2)) + (pow(v.y, 2)) + (pow(v.z, 2)) + (pow(v.w, 2)));
    return res;
}
// Normalize a vector
vec4 normalize(vec4 v){
    vec4 n;
    float coe=1/magnitude(v);
    n=scale_vec_mul(coe,v);
    return n;
}
// Dot product
float dot_product(vec4 v1, vec4 v2){
    float res;
    res=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z+v1.w*v2.w;
    return res;
}
// Cross product
vec4 cross_product(vec4 v1, vec4 v2){
    vec4 res;
    res.x=(v1.y*v2.z)-(v1.z*v2.y);
    res.y=(v1.z * v2.x) - (v1.x * v2.z);
	res.z=(v1.x * v2.y) - (v1.y * v2.x);
	res.w=0;
    return res;
}
// Matrix and its operation
// Print a 4x4 matrix
void print_m4(mat4 m){
	printf("[%7.3f %7.3f %7.3f %7.3f]\n", m.x.x, m.y.x, m.z.x, m.w.x);
	printf("[%7.3f %7.3f %7.3f %7.3f]\n", m.x.y, m.y.y, m.z.y, m.w.y);
	printf("[%7.3f %7.3f %7.3f %7.3f]\n", m.x.z, m.y.z, m.z.z, m.w.z);
	printf("[%7.3f %7.3f %7.3f %7.3f]\n", m.x.w, m.y.w, m.z.w, m.w.w);	
}
// Scalar multiplication: a * m
mat4 scale_mat_mul(float a, mat4 m){
mat4 res;
res.x.x=a*m.x.x;
res.x.y=a*m.x.y;
res.x.z=a*m.x.z;
res.x.w=a*m.x.w;

res.y.x=a*m.y.x;
res.y.y=a*m.y.y;
res.y.z=a*m.y.z;
res.y.w=a*m.y.w;

res.z.x=a*m.z.x;
res.z.y=a*m.z.y;
res.z.z=a*m.z.z;
res.z.w=a*m.z.w;

res.w.x=a*m.w.x;
res.w.y=a*m.w.y;
res.w.z=a*m.w.z;
res.w.w=a*m.w.w;

return res;
}
// Matrix addition: m1 + m2
mat4 mat_mat_add(mat4 m1,mat4 m2){
    m1.x.x += m2.x.x;
	m1.y.x += m2.y.x;
	m1.z.x += m2.z.x;
	m1.w.x += m2.w.x;

    m1.x.y += m2.x.y;
	m1.y.y += m2.y.y;
	m1.z.y += m2.z.y;
	m1.w.y += m2.w.y;

    m1.x.z += m2.x.z;
	m1.y.z += m2.y.z;
	m1.z.z += m2.z.z;
	m1.w.z += m2.w.z;

    m1.x.w += m2.x.w;
	m1.y.w += m2.y.w;
	m1.z.w += m2.z.w;
	m1.w.w += m2.w.w;
	
	return m1;
}
// Matrix subtraction: m1 - m2
mat4 mat_mat_sub(mat4 m1,mat4 m2){
    m1.x.x -= m2.x.x;
	m1.y.x -= m2.y.x;
	m1.z.x -= m2.z.x;
	m1.w.x -= m2.w.x;
	
	m1.x.y -= m2.x.y;
	m1.y.y -= m2.y.y;
	m1.z.y -= m2.z.y;
	m1.w.y -= m2.w.y;

	m1.x.z -= m2.x.z;
	m1.y.z -= m2.y.z;
	m1.z.z -= m2.z.z;
	m1.w.z -= m2.w.z;
	
	m1.x.w -= m2.x.w;
	m1.y.w -= m2.y.w;
	m1.z.w -= m2.z.w;
	m1.w.w -= m2.w.w;
	
	return m1;
}
// Matrix multiplication: m1 * m2
mat4 mat_mat_mul(mat4 m1, mat4 m2) {
    mat4 res;
    res.x.x = (m1.x.x * m2.x.x) + (m1.y.x * m2.x.y) + (m1.z.x * m2.x.z) + (m1.w.x * m2.x.w);
    res.x.y = (m1.x.y * m2.x.x) + (m1.y.y * m2.x.y) + (m1.z.y * m2.x.z) + (m1.w.y * m2.x.w);
    res.x.z = (m1.x.z * m2.x.x) + (m1.y.z * m2.x.y) + (m1.z.z * m2.x.z) + (m1.w.z * m2.x.w);
    res.x.w = (m1.x.w * m2.x.x) + (m1.y.w * m2.x.y) + (m1.z.w * m2.x.z) + (m1.w.w * m2.x.w);

    res.y.x = (m1.x.x * m2.y.x) + (m1.y.x * m2.y.y) + (m1.z.x * m2.y.z) + (m1.w.x * m2.y.w);
    res.y.y = (m1.x.y * m2.y.x) + (m1.y.y * m2.y.y) + (m1.z.y * m2.y.z) + (m1.w.y * m2.y.w);
    res.y.z = (m1.x.z * m2.y.x) + (m1.y.z * m2.y.y) + (m1.z.z * m2.y.z) + (m1.w.z * m2.y.w);
    res.y.w = (m1.x.w * m2.y.x) + (m1.y.w * m2.y.y) + (m1.z.w * m2.y.z) + (m1.w.w * m2.y.w);

    res.z.x = (m1.x.x * m2.z.x) + (m1.y.x * m2.z.y) + (m1.z.x * m2.z.z) + (m1.w.x * m2.z.w);
    res.z.y = (m1.x.y * m2.z.x) + (m1.y.y * m2.z.y) + (m1.z.y * m2.z.z) + (m1.w.y * m2.z.w);
    res.z.z = (m1.x.z * m2.z.x) + (m1.y.z * m2.z.y) + (m1.z.z * m2.z.z) + (m1.w.z * m2.z.w);
    res.z.w = (m1.x.w * m2.z.x) + (m1.y.w * m2.z.y) + (m1.z.w * m2.z.z) + (m1.w.w * m2.z.w);

    res.w.x = (m1.x.x * m2.w.x) + (m1.y.x * m2.w.y) + (m1.z.x * m2.w.z) + (m1.w.x * m2.w.w);
    res.w.y = (m1.x.y * m2.w.x) + (m1.y.y * m2.w.y) + (m1.z.y * m2.w.z) + (m1.w.y * m2.w.w);
    res.w.z = (m1.x.z * m2.w.x) + (m1.y.z * m2.w.y) + (m1.z.z * m2.w.z) + (m1.w.z * m2.w.w);
    res.w.w = (m1.x.w * m2.w.x) + (m1.y.w * m2.w.y) + (m1.z.w * m2.w.z) + (m1.w.w * m2.w.w);

    return res;
}
// Matrix of minors
mat4 minor(mat4 m){
	
	mat4 res;
	
	res.x.x = (m.y.y * m.z.z * m.w.w) + (m.y.z * m.z.w * m.w.y) + (m.y.w * m.z.y * m.w.z) - (m.y.w * m.z.z * m.w.y) - (m.y.z * m.z.y * m.w.w) - (m.y.y * m.z.w * m.w.z);
	res.x.y = (m.y.x * m.z.z * m.w.w) + (m.y.z * m.z.w * m.w.x) + (m.y.w * m.z.x * m.w.z) - (m.y.w * m.z.z * m.w.x) - (m.y.z * m.z.x * m.w.w) - (m.y.x * m.z.w * m.w.z);
	res.x.z = (m.y.x * m.z.y * m.w.w) + (m.y.y * m.z.w * m.w.x) + (m.y.w * m.z.x * m.w.y) - (m.y.w * m.z.y * m.w.x) - (m.y.y * m.z.x * m.w.w) - (m.y.x * m.z.w * m.w.y);
	res.x.w = (m.y.x * m.z.y * m.w.z) + (m.y.y * m.z.z * m.w.x) + (m.y.z * m.z.x * m.w.y) - (m.y.z * m.z.y * m.w.x) - (m.y.y * m.z.x * m.w.z) - (m.y.x * m.z.z * m.w.y);

	res.y.x = (m.x.y * m.z.z * m.w.w) + (m.x.z * m.z.w * m.w.y) + (m.x.w * m.z.y * m.w.z) - (m.x.w * m.z.z * m.w.y) - (m.x.z * m.z.y * m.w.w) - (m.x.y * m.z.w * m.w.z);
	res.y.y = (m.x.x * m.z.z * m.w.w) + (m.x.z * m.z.w * m.w.x) + (m.x.w * m.z.x * m.w.z) - (m.x.w * m.z.z * m.w.x) - (m.x.z * m.z.x * m.w.w) - (m.x.x * m.z.w * m.w.z);
	res.y.z = (m.x.x * m.z.y * m.w.w) + (m.x.y * m.z.w * m.w.x) + (m.x.w * m.z.x * m.w.y) - (m.x.w * m.z.y * m.w.x) - (m.x.y * m.z.x * m.w.w) - (m.x.x * m.z.w * m.w.y);
	res.y.w = (m.x.x * m.z.y * m.w.z) + (m.x.y * m.z.z * m.w.x) + (m.x.z * m.z.x * m.w.y) - (m.x.z * m.z.y * m.w.x) - (m.x.y * m.z.x * m.w.z) - (m.x.x * m.z.z * m.w.y);
	
	res.z.x = (m.x.y * m.y.z * m.w.w) + (m.x.z * m.y.w * m.w.y) + (m.x.w * m.y.y * m.w.z) - (m.x.w * m.y.z * m.w.y) - (m.x.z * m.y.y * m.w.w) - (m.x.y * m.y.w * m.w.z);
	res.z.y = (m.x.x * m.y.z * m.w.w) + (m.x.z * m.y.w * m.w.x) + (m.x.w * m.y.x * m.w.z) - (m.x.w * m.y.z * m.w.x) - (m.x.z * m.y.x * m.w.w) - (m.x.x * m.y.w * m.w.z);
	res.z.z = (m.x.x * m.y.y * m.w.w) + (m.x.y * m.y.w * m.w.x) + (m.x.w * m.y.x * m.w.y) - (m.x.w * m.y.y * m.w.x) - (m.x.y * m.y.x * m.w.w) - (m.x.x * m.y.w * m.w.y);
	res.z.w = (m.x.x * m.y.y * m.w.z) + (m.x.y * m.y.z * m.w.x) + (m.x.z * m.y.x * m.w.y) - (m.x.z * m.y.y * m.w.x) - (m.x.y * m.y.x * m.w.z) - (m.x.x * m.y.z * m.w.y);
	
	res.w.x = (m.x.y * m.y.z * m.z.w) + (m.x.z * m.y.w * m.z.y) + (m.x.w * m.y.y * m.z.z) - (m.x.w * m.y.z * m.z.y) - (m.x.z * m.y.y * m.z.w) - (m.x.y * m.y.w * m.z.z);
	res.w.y = (m.x.x * m.y.z * m.z.w) + (m.x.z * m.y.w * m.z.x) + (m.x.w * m.y.x * m.z.z) - (m.x.w * m.y.z * m.z.x) - (m.x.z * m.y.x * m.z.w) - (m.x.x * m.y.w * m.z.z);
	res.w.z = (m.x.x * m.y.y * m.z.w) + (m.x.y * m.y.w * m.z.x) + (m.x.w * m.y.x * m.z.y) - (m.x.w * m.y.y * m.z.x) - (m.x.y * m.y.x * m.z.w) - (m.x.x * m.y.w * m.z.y);
	res.w.w = (m.x.x * m.y.y * m.z.z) + (m.x.y * m.y.z * m.z.x) + (m.x.z * m.y.x * m.z.y) - (m.x.z * m.y.y * m.z.x) - (m.x.y * m.y.x * m.z.z) - (m.x.x * m.y.z * m.z.y);

	return res;
}
// Matrix of cofactors
mat4 cofactor(mat4 m){
    mat4 res;
    res.x.x=m.x.x;
    res.x.y=(-1)* m.x.y;
    res.x.z = m.x.z;
	res.x.w = (-1) * m.x.w;

    res.y.x=(-1)*m.y.x;
    res.y.y=m.y.y;
    res.y.z=(-1)*m.y.z;
    res.y.w=m.y.w;

    res.z.x=m.z.x;
    res.z.y=(-1)*m.z.y;
    res.z.z=m.z.z;
    res.z.w=(-1)*m.z.w;

    res.w.x=(-1)*m.w.x;
    res.w.y=m.w.y;
    res.w.z=(-1)*m.w.z;
    res.w.w=m.w.w;

    return res;
}
// Determinant, given m and its cofactor matrix
float determinant(mat4 m, mat4 cofactor){
	
	float res, a, b, c, d;
	
	a = m.x.x * cofactor.x.x;
	b = m.x.y * cofactor.x.y;
	c = m.x.z * cofactor.x.z;
	d = m.x.w * cofactor.x.w;
	
	res = a + b + c + d;
	return res;
}

// Transpose of a matrix
mat4 mat_transpose(mat4 m) {
    mat4 res;

    res.x.x = m.x.x;  res.y.x = m.x.y;  res.z.x = m.x.z;  res.w.x = m.x.w;
    res.x.y = m.y.x;  res.y.y = m.y.y;  res.z.y = m.y.z;  res.w.y = m.y.w;
    res.x.z = m.z.x;  res.y.z = m.z.y;  res.z.z = m.z.z;  res.w.z = m.z.w;
    res.x.w = m.w.x;  res.y.w = m.w.y;  res.z.w = m.w.z;  res.w.w = m.w.w;

    return res;
}
// Inverse of a matrix
mat4 inverse(mat4 m){
    mat4 min = minor(m);
	mat4 cof = cofactor(min);
	mat4 trans = mat_transpose(cof);
	float det = determinant(m, cof);
	mat4 res = scale_mat_mul((1/det),trans);
	return res;
}
// Matrix-vector multiplication
vec4 mat_vec_mul(mat4 m, vec4 v){
    vec4 res;

    res.x = m.x.x * v.x + m.y.x * v.y + m.z.x * v.z + m.w.x * v.w; 
    res.y = m.x.y * v.x + m.y.y * v.y + m.z.y * v.z + m.w.y * v.w; 
    res.z = m.x.z * v.x + m.y.z * v.y + m.z.z * v.z + m.w.z * v.w; 
    res.w = m.x.w * v.x + m.y.w * v.y + m.z.w * v.z + m.w.w * v.w; 

    return res;
}
mat4 identity() {
    mat4 m;
    m.x = (vec4){1,0,0,0};  
    m.y = (vec4){0,1,0,0};  
    m.z = (vec4){0,0,1,0};  
    m.w = (vec4){0,0,0,1};  
    return m;
}


mat4 translate(float ax, float ay, float az) {
    mat4 m = identity();
    m.w.x = ax;  
    m.w.y = ay;
    m.w.z = az;
    return m;
}
mat4 scale_m(float bx, float by, float bz) {
    mat4 m;
    m.x = (vec4){bx,0, 0, 0};
    m.y = (vec4){0, by,0, 0};
    m.z = (vec4){0, 0, bz,0};
    m.w = (vec4){0, 0, 0, 1};
    return m;
}

mat4 rotate_x_rad(float t) {
    float c = cosf(t), s = sinf(t);
    mat4 m;
    m.x = (vec4){1, 0, 0, 0};
    m.y = (vec4){0, c, s, 0};   
    m.z = (vec4){0,-s, c, 0};
    m.w = (vec4){0, 0, 0, 1};
    return m;
}

mat4 rotate_y_rad(float t) {
    float c = cosf(t), s = sinf(t);
    mat4 m;
    m.x = (vec4){ c, 0,-s, 0};
    m.y = (vec4){ 0, 1, 0, 0};
    m.z = (vec4){ s, 0, c, 0};
    m.w = (vec4){ 0, 0, 0, 1};
    return m;
}

mat4 rotate_z_rad(float t) {
    float c = cosf(t), s = sinf(t);
    mat4 m;
    m.x = (vec4){ c, s, 0, 0};
    m.y = (vec4){-s, c, 0, 0};
    m.z = (vec4){ 0, 0, 1, 0};
    m.w = (vec4){ 0, 0, 0, 1};
    return m;
}


