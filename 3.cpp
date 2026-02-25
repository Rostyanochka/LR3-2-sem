#include <cmath>
#include <iostream>
#include <locale.h>
#include <string>
using namespace std;

long long gcd(long long a, long long b) { return b == 0 ? a : gcd(b, a % b); }

string decimalToFraction(double value, double epsilon = 1.0e-6) {
  bool negative = value < 0;
  if (negative)
    value = -value;

  long long integer_part = static_cast<long long>(value); // целая
  value -= integer_part;                                  // дроб

  // гранич знач для цепной дроби
  long long lower_n = 0, lower_d = 1;
  long long upper_n = 1, upper_d = 1;

  while (true) {                            // средние дроби среди границ
    long long middle_n = lower_n + upper_n; // числ
    long long middle_d = lower_d + upper_d; // знамен

    if (middle_d * (value + epsilon) < middle_n) {
      // ср дробь большая - уменьшаем верхнюю границу
      upper_n = middle_n;
      upper_d = middle_d;
    } else if (middle_n < (value - epsilon) * middle_d) {
      // ср дробь маленькая - увеличиваем нижнюю грницу
      lower_n = middle_n;
      lower_d = middle_d;
    } else { // найдена дробь
      long long numerator = integer_part * middle_d + middle_n;
      long long denominator = middle_d;

      long long common_divisor = gcd(numerator, denominator);
      numerator /= common_divisor;
      denominator /= common_divisor;

      if (negative)
        numerator = -numerator;

      if (denominator == 1) {
        return to_string(numerator);
      } else {
        return to_string(numerator) + "/" + to_string(denominator);
      }
    }
  }
}

double summ_rows(int a, double b, int iterations = 1000) {
  if (b <= 1)
    return NAN; // расходится

  double sum = 0.0;
  for (int n = 1; n <= iterations; n++) {
    sum += pow(n, a) / pow(b, n); // вычисляем каждый член ряда
  }
  return sum;
}

string format_result(int a, double b, double sum) {
  if (isnan(sum)) {
    return "infinity";
  }

  return decimalToFraction(sum);
}

int main() {
  setlocale(LC_ALL, "Russian");
  int a;
  double b;

  cout << "Введите значение a: ";
  cin >> a;
  cout << "Введите значение b: ";
  cin >> b;

  double sum = summ_rows(a, b);
  cout << format_result(a, b, sum) << endl;

  return 0;
}