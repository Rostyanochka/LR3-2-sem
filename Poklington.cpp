#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <locale.h>
#include <random>
#include <vector>

using namespace std;

vector<int> primes; // Глобальный вектор для хранения простых чисел

// Функция для определения количества бит в числе
int bitsize(long long n) { return n == 0 ? 0 : 64 - __builtin_clzll(n); }

// Решето Эратосфена для нахождения простых чисел до 500
void sieveOfEratosthenes() {
  const int n = 500;
  vector<bool> is_prime(n + 1, true);
  is_prime[0] = is_prime[1] = false;

  // Просеивание: отмечаем составные числа
  for (int p = 2; p * p <= n; ++p) {
    if (is_prime[p]) {
      for (int i = p * p; i <= n; i += p) {
        is_prime[i] = false;
      }
    }
  }

  // Заполняем вектор primes простыми числами
  primes.clear();
  for (int i = 2; i <= n; ++i) {
    if (is_prime[i]) {
      primes.push_back(i);
    }
  }
}

// Быстрое возведение в степень по модулю (a^b mod m)
long long mod_pow(long long a, long long b, long long m) {
  long long result = 1;
  a = a % m;
  while (b > 0) {
    if (b % 2 == 1) {
      result = (result * a) % m;
    }
    a = (a * a) % m;
    b = b / 2;
  }
  return result;
}

// Тест Миллера-Рабина на простоту
bool millerRabinTest(long long n, int k = 5) {
  if (n <= 1)
    return false;
  if (n == 2 || n == 3)
    return true;
  if (n % 2 == 0)
    return false;

  // Разложение n-1 = d * 2^s
  long long d = n - 1;
  int s = 0;
  while (d % 2 == 0) {
    d /= 2;
    s++;
  }

  // Генератор случайных чисел
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<long long> dis(2, n - 2);

  // Проводим k тестов
  for (int i = 0; i < k; i++) {
    long long a = dis(gen);
    long long x = mod_pow(a, d, n);

    if (x == 1 || x == n - 1)
      continue;

    for (int j = 0; j < s - 1; j++) {
      x = mod_pow(x, 2, n);
      if (x == n - 1)
        break;
    }

    if (x != n - 1)
      return false;
  }
  return true;
}

// Тест Поклингтона на простоту
bool pocklingtonTest(long long n, const vector<int> &primes) {
  if (n <= 1)
    return false;
  if (n == 2 || n == 3)
    return true;
  if (n % 2 == 0)
    return false;

  // Факторизация n-1
  vector<long long> factors;
  long long n_minus_1 = n - 1;
  long long temp = n_minus_1;

  // Ищем простые делители n-1
  for (int p : primes) {
    if (p * p > temp)
      break;
    if (temp % p == 0) {
      factors.push_back(p);
      while (temp % p == 0) {
        temp /= p;
      }
    }
  }
  if (temp > 1) {
    factors.push_back(temp);
  }

  // Вычисляем F (произведение простых делителей)
  long long F = 1;
  for (long long q : factors) {
    F *= q;
  }

  // Проверка условия F > √n
  if (F <= sqrt(n)) {
    return false;
  }

  // Генератор случайных чисел
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<long long> dis(2, n - 2);

  // Проводим 10 тестов
  for (int i = 0; i < 10; ++i) {
    long long a = dis(gen);
    if (mod_pow(a, n - 1, n) != 1) {
      return false;
    }

    // Проверяем дополнительные условия
    bool all_conditions = true;
    for (long long q : factors) {
      if (mod_pow(a, (n - 1) / q, n) == 1) {
        all_conditions = false;
        break;
      }
    }

    if (all_conditions) {
      return true;
    }
  }

  return false;
}

// Генерация простых чисел по методу Поклингтона
vector<int> generatePocklingtonPrimes(int count) {
  random_device rd;
  mt19937 gen(rd());

  vector<int> result;
  const int target_prime_bits = 16; // Целевой размер простого числа в битах
  const int half_bits = target_prime_bits / 2;

  // Границы для F (F должен быть больше √n)
  const int F_min_bits = half_bits + 1;
  const int F_max_bits = half_bits + 2;

  while (result.size() < count) {
    vector<int> F_factors;
    long long F = 1;
    int attempts = 0;
    const int max_attempts = 100;

    // Подбираем множители для F
    while (bitsize(F) < F_min_bits && attempts++ < max_attempts) {
      uniform_int_distribution<size_t> prime_dis(0, primes.size() - 1);
      int p = primes[prime_dis(gen)];

      if (bitsize(F * p) > F_max_bits)
        continue;

      if (find(F_factors.begin(), F_factors.end(), p) == F_factors.end()) {
        F_factors.push_back(p);
        F *= p;
      }
    }

    int F_bits = bitsize(F);
    if (F_bits < F_min_bits || F_bits > F_max_bits)
      continue;

    // Вычисляем размер R
    const int R_bits = target_prime_bits - F_bits;
    if (R_bits < 1)
      continue;

    // Генерируем четное R
    long long R_min = 1LL << (R_bits - 1);
    long long R_max = (1LL << R_bits) - 1;

    if (R_min % 2 != 0)
      R_min++;
    if (R_max % 2 != 0)
      R_max--;

    if (R_max <= R_min)
      continue;

    uniform_int_distribution<long long> R_dis(R_min, R_max);
    long long R = R_dis(gen);
    if (R % 2 != 0)
      R--;

    // Проверка на переполнение
    if (R > (numeric_limits<long long>::max() - 1) / F)
      continue;

    // Вычисляем кандидата n = R*F + 1
    long long n = R * F + 1;

    if (bitsize(n) != target_prime_bits)
      continue;

    // Проверяем кандидата тестом Поклингтона
    if (pocklingtonTest(n, primes)) {
      if (find(result.begin(), result.end(), n) == result.end()) {
        result.push_back(static_cast<int>(n));
      }
    }
  }
  return result;
}

// Вывод результатов тестирования в виде таблицы
void printResultsTable(const vector<int> &numbers) {
  cout << "| № | Число | Поклингтон | Миллера-Рабина |\n";
  cout << "-----------------------------------------------------\n";

  int rejected_miller = 0;

  for (size_t i = 0; i < numbers.size(); ++i) {
    bool pocklington_result = pocklingtonTest(numbers[i], primes);
    bool miller_result = millerRabinTest(numbers[i]);

    if (!miller_result)
      rejected_miller++;

    cout << "| " << i + 1 << " | " << numbers[i] << " | "
         << (pocklington_result ? "+" : "-") << " | "
         << (miller_result ? "+" : "-") << " |\n";
  }

  cout << "-----------------------------------------------------\n";
  cout << "k = " << rejected_miller << "\n";
}

int main() {
  setlocale(LC_ALL, "Russian");
  sieveOfEratosthenes(); // Генерируем простые числа до 500

  // Генерируем 10 простых чисел методом Поклингтона
  vector<int> pocklington_primes = generatePocklingtonPrimes(10);

  // Выводим результаты тестирования
  printResultsTable(pocklington_primes);

  return 0;
}