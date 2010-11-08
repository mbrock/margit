#include <margit/math.hpp>

namespace margit {
  Vector2D operator * (scalar_t k, const Vector2D& v)
  { return v * k; }
  
  scalar_t random_scalar ()
  { return 2 * (::random () / (scalar_t) RAND_MAX) - 1; }
}
