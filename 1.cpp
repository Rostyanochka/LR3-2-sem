#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

double func(double x) {
    if (x >= -7 && x < -6) {
        return 1;
    }
    else if (x >= -6 && x < -4) {
        return -0.5 * (x + 6) + 1; // линейный участок
    }
    else if (x >= -4 && x < 0) {
        double val = 4 - pow(x + 2, 2);
        return (val >= 0) ? sqrt(val) : NAN; // полуокружность R=2
    }
    else if (x >= 0 && x < 2) {
        double val = 1 - pow(x - 1, 2);
        return (val >= 0) ? -sqrt(val) : NAN; // нижняя полуокружность R=1
    }
    else if (x >= 2 && x <= 3) {
        return (x - 2) - 1; // линейный участок
    }
    else {
        return NAN; // вне области определения
    }
}

int main() {
    double dx = 0.5;
    double xn = -7.0;
    double xk = 3.0;

    cout << "-----------------------------" << endl;
    cout << "         x   |          y" << endl;
    while (xn <= xk + 1e-6) { // защита от потери последней итерации из-за округления
        double y = func(xn);
        cout << "-----------------------------" << endl;
        if (!isnan(y)) {
            cout << setw(11) << fixed << setprecision(2) << xn << " |"
                 << setw(12) << fixed << setprecision(6) << y << endl;
        } else {
            cout << setw(11) << fixed << setprecision(2) << xn << " |"
                 << setw(12) << "undefined" << endl;
        }
        xn += dx;
    }

    return 0;
}

