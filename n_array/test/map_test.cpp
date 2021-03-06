#include <satyr/n_array.h>
#include <cassert>
using namespace satyr;

int main() {
  n_array<double, 2, satyr::general_structure> array1(3, 5), array2(3,5);
  n_array<double, 3, satyr::general_structure> array3(3, 5, 6);
  [[maybe_unused]] auto expr1 = make_expression(array1);
  [[maybe_unused]] auto expr2 = make_expression(expr1);
  [[maybe_unused]] auto expr3 = make_expression(2.0);
  auto f = [](double x, double y) { return x + y; };
  [[maybe_unused]] auto eval = expr1.evaluator();
  [[maybe_unused]] auto expr4 = map(f, array1, array2);
  [[maybe_unused]] auto expr5 = map(f, array1, 5.0);

  // auto expr6 = map(f, array1, array3); // won't compile

  [[maybe_unused]] auto expr7 = array1 + array2;

  array1 = 59;
  array1 += 51;
  array1 += array1 + array2;

  sqrt(array1);

  array1 = map_indexes(array1.shape(),
                       [](index_t i, index_t j) { return i * 10 + j; });
  return 0;
}
