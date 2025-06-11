#ifndef VECTOR_H
#define VECTOR_H
// All the stuff required for vectors

// Floating Vector2
typedef struct {
    float x; 
    float y; 
} fv2; 

// Floating Vector 3
typedef struct {
    float x;
    float y;
    float z;
} fv3;

typedef struct {
    fv3 ihat;
    fv3 jhat;
    fv3 khat;
} BasisVectors;

fv3 multfv3(fv3 a, fv3 b); // Multiply two vectors together
fv3 scalefv3(fv3 a, float scal);   // Multiply a vector by a scalar
fv3 addfv3(fv3 a, fv3 b); // Add two vectors together

#endif // !VECTOR_H
