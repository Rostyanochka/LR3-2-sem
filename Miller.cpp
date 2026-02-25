#include <algorithm>
#include <iostream>
#include <locale.h>
#include <random>
#include <vector>

using namespace std;

vector<int> primes;

void sieveOfEratosthenes() {
  const int n = 500;
  vector<bool> is_prime(n + 1, true); // Массив флагов простоты чисел
  is_prime[0] = is_prime[1] = false;  // 0 и 1 - не простые

  // Просеивание: отмечаем составные числа
  for (int p = 2; p * p <= n; ++p) {
    if (is_prime[p]) {
      // Все кратные p числа - составные
      for (int i = p * p; i <= n; i += p) {
        is_prime[i] = false;
      }
    }
  }

  // Заполняем вектор primes найденными простыми числами
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
  a = a % m; // Начальное приведение по модулю

  while (b > 0) {
    // Если степень нечетная, умножаем результат на a
    if (b % 2 == 1) {
      result = (result * a) % m;
    }
    // Возводим a в квадрат и уменьшаем степень вдвое
    a = (a * a) % m;
    b = b / 2;
  }
  return result;
}

// Факторизация числа n-1 (разложение на простые множители)
vector<long long> factorize(long long n) {
  vector<long long> factors;
  long long d = n - 1;

  // Выделяем все множители 2
  while (d % 2 == 0) {
    factors.push_back(2);
    d /= 2;
  }

  // Проверяем делимость на простые числа из решета
  for (int p : primes) {
    if (p * p > d)
      break; // Проверяем до корня из d
    if (d % p == 0) {
      factors.push_back(p);
      while (d % p == 0) {
        d /= p;
      }
    }
  }

  // Если остался простой делитель больше 1
  if (d > 1) {
    factors.push_back(d);
  }
  return factors;
}

// Тест Миллера-Рабина на простоту
bool millerTest(long long n, int t = 5) {
  // Крайние случаи
  if (n <= 1)
    return false;
  if (n == 2 || n == 3)
    return true;
  if (n % 2 == 0)
    return false; // Четные числа > 2 - составные

  // Факторизуем n-1
  vector<long long> factors = factorize(n - 1);

  // Генератор случайных чисел
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<long long> dis(2, n - 2);

  // Проверяем условия для каждого простого делителя q из factors
  for (long long q : factors) {
    bool all_conditions = false;

    // Повторяем тест t раз для надежности
    for (int i = 0; i < t; ++i) {
      long long a = dis(gen); // Случайное основание

      // Первое условие Ферма
      if (mod_pow(a, n - 1, n) != 1) {
        return false;
      }

      // Второе условие теста
      if (mod_pow(a, (n - 1) / q, n) != 1) {
        all_conditions = true;
        break;
      }
    }

    // Если ни один тест не прошел второе условие
    if (!all_conditions) {
      return false;
    }
  }

  return true; // Число вероятно простое
}

// Генерация числа m как произведения k случайных простых чисел
int generateM(int k) {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<size_t> dis(0, primes.size() - 1);

  while (true) {
    int m = 1;
    for (int i = 0; i < k; ++i) {
      size_t index = dis(gen); // Случайный индекс простого числа
      m *= primes[index];      // Умножаем на случайное простое

      // Проверка на превышение лимита
      if (m >= 49999) {
        m = -1;
        break;
      }
    }

    if (m != -1) {
      return m; // Возвращаем подходящее m
    }
  }
}

// Генерация count чисел, проходящих тест Миллера
vector<int> generateMillerPrimes(int count) {
  vector<int> result;
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> k_dis(1, 3); // k от 1 до 3

  while (result.size() < count) {
    int k = k_dis(gen);   // Случайное k
    int m = generateM(k); // Генерируем m
    int n = 2 * m + 1;    // Вычисляем кандидата n

    // Проверяем n тестом Миллера и на диапазон
    if (n > 1 && n < 100000 && millerTest(n)) {
      // Проверяем на уникальность
      if (find(result.begin(), result.end(), n) == result.end()) {
        result.push_back(n);
      }
    }
  }
  return result;
}

// Вывод результатов в виде таблицы
void printResultsTable(const vector<int> &numbers, const string &test_name) {
  cout << "Результат теста " << test_name << ":\n";
  cout << "| № | Число | Результат |\n";
  cout << "-----------------------------------------------------\n";

  int rejected = 0; // Счетчик отвергнутых чисел
  for (size_t i = 0; i < numbers.size(); ++i) {
    bool result = millerTest(numbers[i]);
    if (!result)
      rejected++;
    cout << "| " << i + 1 << " | " << numbers[i] << " | "
         << (result ? "+" : "-") << " |\n";
  }

  cout << "-----------------------------------------------------\n";
  cout << "k = " << rejected << "\n"; // Вывод количества отвергнутых чисел
}

int main() {
  setlocale(LC_ALL, "Russian"); // Установка русской локали

  sieveOfEratosthenes(); // Генерация простых чисел решетом

  // Генерация 10 чисел, проходящих тест Миллера
  vector<int> miller_primes = generateMillerPrimes(10);

  // Вывод результатов тестирования
  printResultsTable(miller_primes, "Миллера");

  return 0;
}