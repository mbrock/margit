#ifndef MARGIT_POINT_MASS_HPP
#define MARGIT_POINT_MASS_HPP

#include <margit/math.hpp>

namespace margit {
  // Represents the function f in velocity(t) = f(t, position(t)),
  // numerically integrated to move the point mass.
  struct PositionDerivative {
    Vector2D a, v;

    PositionDerivative (Vector2D a, Vector2D v) : a (a), v (v) { }

    Vector2D value_at (scalar_t x, Vector2D y) const
    { return v + a * x; }
  };

  class PointMass {
  public:
    PointMass (Vector2D position, scalar_t mass)
      : m_position (position),
        m_mass (mass),
        m_is_fixed (false)
    { }

    const Vector2D& position () const { return m_position; }
    const Vector2D& velocity () const { return m_velocity; }

    void set_fixedness (bool fixedness) { m_is_fixed = fixedness; }
    
    void reset_force ()
    { m_frame_force.reset (); }

    void add_force (const Vector2D& f)
    { m_frame_force += f; }

    void add_gravity (scalar_t g)
    { add_force (Vector2D (0, m_mass * -g)); }

    void step (scalar_t dt)
    {
      if (!m_is_fixed) {
        const Vector2D a = m_frame_force / m_mass;
        const PositionDerivative f (a, m_velocity);
        m_position = integrate<PositionDerivative, Vector2D> (f, dt, m_position);
        m_velocity += dt * a;
      }
    }

  private:
    Vector2D m_position;
    Vector2D m_velocity;
    scalar_t m_mass;
    
    bool m_is_fixed;
    
    Vector2D m_frame_force;
  };
}

#endif
