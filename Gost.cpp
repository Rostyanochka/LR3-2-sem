#include <ctime>
#include <iostream>
#include <locale.h>
#include <random>
#include <vector>

using namespace std;

// Количество простых чисел для генерации
const int NUMOFPRIMES = 10;
// Вектор для хранения простых чисел
vector<int> primes;

// Функция для определения размера числа в битах
int binSize(long long n) { return n == 0 ? 0 : 64 - __builtin_clzll(n); }

// округления в большую сторону при делении a на b
int bigRound(int a, int b) { return (a + b - 1) / b; }

// Функция для случайного выбора элемента из вектора
template <typename T> T randomChoice(const vector<T> &vec, mt19937_64 &gen) {
  uniform_int_distribution<size_t> dis(0, vec.size() - 1);
  return vec[dis(gen)];
}

// Решето Эратосфена для нахождения простых чисел до 500
void sieveOfEratosthenes() {
  const int n = 500;
  vector<bool> is_prime(n + 1, true);
  is_prime[0] = is_prime[1] = false;

  for (int p = 2; p * p <= n; ++p) {
    if (is_prime[p]) {
      for (int i = p * p; i <= n; i += p) {
        is_prime[i] = false;
      }
    }
  }

  primes.clear();
  for (int i = 2; i <= n; ++i) {
    if (is_prime[i]) {
      primes.push_back(i);
    }
  }
}

// Функция для возведения в степень по модулю (a^b mod m)
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

// Тест простоты по ГОСТ Р 34.10-94
bool gostTest(long long n, const vector<int> &primes) {
  if (n <= 1)
    return false;
  if (n == 2 || n == 3)
    return true;
  if (n % 2 == 0)
    return false;

  long long n_minus_1 = n - 1;
  long long temp = n_minus_1;
  long long q = 1;

  // Находим наибольший простой делитель q числа n-1
  for (int p : primes) {
    if (p * p > temp)
      break;
    if (temp % p == 0) {
      q = p;
      while (temp % p == 0) {
        temp /= p;
      }
    }
  }
  if (temp > 1) {
    q = temp;
  }

  if (mod_pow(2, n - 1, n) != 1) {
    return false;
  }

  // 2. 2^((n-1)/q) ≢ 1 mod n
  if (mod_pow(2, (n - 1) / q, n) == 1) {
    return false;
  }

  return true;
}

// Генерация простых чисел по ГОСТ Р 34.10-94
vector<int> generateGostPrimes(int t, const vector<int> &primes,
                               mt19937_64 &gen) {
  vector<int> result;
  uniform_real_distribution<double> dist(0.1, 1.0);

  const int pow2t1 = 1 << (t - 1); // 2^(t-1)
  const int pow2t = 1 << t;        // 2^t

  while (result.size() < NUMOFPRIMES) {
    // Выбираем простое число q размером не более t/2 бит
    int q = randomChoice(primes, gen);
    while (binSize(q) >= bigRound(t, 2)) {
      q = randomChoice(primes, gen);
    }

    bool isFound = false;

    while (!isFound) {
      // Генерируем случайное число ksi в диапазоне [0.1, 1.0]
      double ksi = dist(gen);
      // Вычисляем N
      int NN = bigRound(pow2t1, q) + bigRound(pow2t1 * ksi, q);
      // Обеспечиваем четность N
      int N = NN % 2 == 0 ? NN : NN + 1;
      int u = 0;

      while (!isFound) {
        // Вычисляем кандидата p = (N + u)*q + 1
        int p = (N + u) * q + 1;
        if (p >= pow2t)
          break;

        // Проверяем условия простоты:
        // 1. 2^(p-1) ≡ 1 mod p
        // 2. 2^(N+u) ≢ 1 mod p
        if (mod_pow(2, p - 1, p) == 1 && mod_pow(2, N + u, p) != 1) {
          result.push_back(p);
          isFound = true;
        }
        u += 2;
      }
    }
  }

  return result;
}

void printResultsTable(const vector<int> &numbers) {
  cout << "| № | Число | ГОСТ |\n";
  cout << "------------------------------------\n";

  int rejected = 0; // Счетчик отвергнутых чисел

  for (size_t i = 0; i < numbers.size(); ++i) {
    bool gost_result = gostTest(numbers[i], primes);

    if (!gost_result)
      rejected++;

    cout << "| " << i + 1 << " | " << numbers[i] << " | "
         << (gost_result ? "+" : "-") << " |\n";
  }

  cout << "k = " << rejected << "\n";
}

int main() {
  setlocale(LC_ALL, "Russian");
  mt19937_64 gen(time(0)); // Инициализируем генератор случайных чисел

  // Генерируем простые числа до 500 с помощью решета Эратосфена
  sieveOfEratosthenes();

  // Генерируем простые числа по ГОСТ Р 34.10-94 длиной 16 бит
  vector<int> gost_primes = generateGostPrimes(16, primes, gen);

  printResultsTable(gost_primes);

  return 0;
}