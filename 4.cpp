#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  int n, m;
  cin >> n >> m;

  vector<int> a(n); // вектор с числами из тетради
  for (int i = 0; i < n; ++i) {
    cin >> a[i];
  }

  vector<int> pref(n + 1, 0); // префиксные суммы
  for (int i = 0; i < n; ++i) {
    pref[i + 1] = pref[i] + a[i];
  }

  vector<int> dp(
      n + 1,
      0); // хранения максимальной разницы между суммами текущего и след игрока

  for (int i = n - 1; i >= 0; --i) { // заполнение с конца dp - выгода
    int max_diff = -1e9;
    for (int k = 1; k <= m && i + k <= n; ++k) {
      int current_sum = pref[i + k] - pref[i];
      int next_diff = dp[i + k];
      max_diff = max(max_diff, current_sum - next_diff);
    }
    dp[i] = max_diff;
  }

  if (dp[0] > 0) {
    cout << 1 << endl; // выиграл паша
  } else {
    cout << 0 << endl; // выиграла вика
  }

  return 0;
}