#ifndef MARGIT_SPRING_HPP
#define MARGIT_SPRING_HPP

#include <margit/point-mass.hpp>

namespace margit {
  class Spring {
  public:
    Spring (PointMass* a,
	    PointMass* b,
	    scalar_t damping,
	    scalar_t stiffness,
	    scalar_t rest_length)
      : m_a (a), m_b (b),
        m_damping (damping),
        m_stiffness (stiffness),
        m_rest_length (rest_length),
        m_is_snapped (false)
    { }

    bool is_snapped () const { return m_is_snapped; }
    void snap () { m_is_snapped = true; }
    void unsnap () { m_is_snapped = false; }
    
    const PointMass& point_a () const { return *m_a; }
    const PointMass& point_b () const { return *m_b; }
    
    void add_forces_to_point_masses () const
    {
      if (!m_is_snapped) {
        const Vector2D pa = m_a->position ();
        const Vector2D pb = m_b->position ();
        
        // Force magnitude is proportional to the displacement from rest length.
        const scalar_t magnitude =
          (-m_stiffness) * (pa.distance_to (pb) - m_rest_length);
        // Force direction is towards the other point.
        const Vector2D spring_force = magnitude * (pa - pb).normal ();
        // Damping force counteracts relative velocity.
        const Vector2D damping_force =
          (-m_damping) * (m_a->velocity () - m_b->velocity ());
        
        const Vector2D force = spring_force + damping_force;
        m_a->add_force (force);
        m_b->add_force (-force);
      }
    }

  private:
    PointMass* m_a;
    PointMass* m_b;

    scalar_t m_damping;
    scalar_t m_stiffness;
    scalar_t m_rest_length;
    
    bool m_is_snapped;
  };
}

#endif
