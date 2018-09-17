#include <iostream>
#include <cassert>
#include <climits>
#include <cmath>

#define INF_ROOTS INT_MAX

///...........................................
/// @def Solves quadratic equation
///
/// @param [in] a - coefficient
/// @param [in] b - coefficient
/// @param [in] c - coefficient
/// @param [out] x1 - first root pointer
/// @param [out] x2 - second root pointer
///
/// @return Number of roots
///...........................................

int SolveSquare(double a, double b, double c, double *x1, double *x2) {
    assert(x1 != nullptr);
    assert(x2 != nullptr);
    assert(x1 != x2);

    if (a == 0) {
        if (b == 0) {
            return c == 0 ? INF_ROOTS : 0;
        } else {
            *x1 = -c / b;
            return 1;
        }
    } else { // a != 0
        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {

            return 0;
        } else if (discriminant == 0) {
            *x1 = *x2 = -b / (2 * a);

            return 1;
        } else {
            double sqrt_discriminant = sqrt(discriminant);
            *x1 = (-b - sqrt_discriminant) / (2 * a);
            *x2 = (-b + sqrt_discriminant) / (2 * a);

            return 2;
        }
    }
}