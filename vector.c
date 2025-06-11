
#include "vector.h"

fv3 multfv3(fv3 a, fv3 b) {
    fv3 c;
    c.x = a.x * b.x;
    c.y = a.y * b.y;
    c.z = a.z * b.z;
    return c;
}

fv3 scalefv3(fv3 a, float scal) {
    return (fv3) {scal * a.x, scal * a.y, scal * a.z};
}

fv3 addfv3(fv3 a, fv3 b) {
    return (fv3) {a.x + b.x, a.y + b.y, a.z + b.z};
}
