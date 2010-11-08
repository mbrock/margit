#ifndef MARGIT_MATH_HPP
#define MARGIT_MATH_HPP

#include <cmath>
#include <cstdlib>

#include <CoreServices/CoreServices.h>

#include <mach/mach.h>
#include <mach/mach_time.h>

namespace margit {
  typedef float coord_t;
  typedef float scalar_t;

  scalar_t random_scalar ();
  
  template <typename T>
  T max (T a, T b)
  { return (a > b) ? a : b; }
  
  struct Vector2D {
    coord_t x, y;

    Vector2D () : x (0), y (0) { }
    Vector2D (coord_t x, coord_t y) : x (x), y (y) { }

    static Vector2D random_unit () 
    { return Vector2D (random_scalar (), random_scalar ()).normal (); }
    
    Vector2D operator + (const Vector2D& v) const
    { return Vector2D (x + v.x, y + v.y); }

    Vector2D& operator += (const Vector2D& v)
    {
      x += v.x;
      y += v.y;
      return *this;
    }

    Vector2D operator - (const Vector2D& v) const
    { return Vector2D (x - v.x, y - v.y); }

    Vector2D operator - () const
    { return Vector2D (-x, -y); }

    Vector2D operator * (scalar_t k) const
    { return Vector2D (x * k, y * k); }

    Vector2D operator / (scalar_t k) const
    { return Vector2D (x / k, y / k); }

    scalar_t magnitude () const
    { return ::sqrt (x * x + y * y); }

    Vector2D normal () const
    { return *this / magnitude (); }

    scalar_t distance_to (const Vector2D& v) const
    { return (*this - v).magnitude (); }

    void reset ()
    { x = y = 0; }
  };

  Vector2D operator * (scalar_t k, const Vector2D& v);

#define MARGIT_USE_RUNGE_KUTTA 1
  
  // Approximates the integral of f from 0 to h given the
  // starting value y0 at f(0).
  template <typename F, typename T>
  T integrate (const F& f, scalar_t h, T y0)
  {
#if MARGIT_USE_RUNGE_KUTTA
    // Fourth-order Runge-Kutta integration.

    const scalar_t half_h = h / 2;
    const T k1 = h * f.value_at (0, y0);
    const T k2 = h * f.value_at (half_h, y0 + k1 / 2);
    const T k3 = h * f.value_at (half_h, y0 + k2 / 2);
    const T k4 = h * f.value_at (h, y0 + k3);

    return y0 + (k1 + (2 * k2) + (2 * k3) + k4) / 6;
#else
    return y0 + h * f.value_at (0, y0); // Euler integration.
#endif
  }
  
  class StopWatch {
  public:
    void start () {
      m_t0 = mach_absolute_time ();
    }
    
    scalar_t lap () {
      const uint64_t t (mach_absolute_time ());
      const uint64_t elapsed (t - m_t0);
      Nanoseconds elapsed_nano
        (AbsoluteToNanoseconds (*(AbsoluteTime*) &elapsed));
      
      m_t0 = t;
      
      return *((uint64_t*) &elapsed_nano) / 1000000000.0;
    }
    
  private:
    uint64_t m_t0;
  };
}

#endif
