#ifndef MARGIT_GAME_HPP
#define MARGIT_GAME_HPP

#include <margit/point-mass.hpp>
#include <margit/spring.hpp>

#include <OpenGL/OpenGL.h>

#include <vector>

namespace margit {
  class Game {
    static const int n = 10;
    
  public:
    Game () : m_punt_time (0) {
      m_stop_watch.start ();
      
      float stiffness = 200;
      float damping = 2;
      float width = 10;
      float height = 20;
      float diagonal = ::sqrt ((width * width) / 4 + height * height);
      
      PointMass* pa = new PointMass (Vector2D (0, 0), 1);
      PointMass* pb = new PointMass (Vector2D (width * n, 0), 1);
      
      pa->set_fixedness (true);
      pb->set_fixedness (true);
      
      for (int i = 1; i <= n - 1; i++) {
        PointMass* p = new PointMass (Vector2D (width * i + width / 2,
                                                height * 2), 1);
        m_top_vs.push_back (p);
        m_all_vs.push_back (p);
      }
      
      m_bottom_vs.push_back (pa);
      m_all_vs.push_back (pa);
      for (int i = 2; i <= n - 1; i++) {
        PointMass* p = new PointMass (Vector2D (width * i, height), 2);
        m_bottom_vs.push_back (p);
        m_all_vs.push_back (p);
      }
      m_bottom_vs.push_back (pb);
      m_all_vs.push_back (pb);
      
      for (int i = 1; i <= n - 1; i++)
        m_bottom_ss.push_back (new Spring (m_bottom_vs[i - 1], m_bottom_vs[i],
                                           damping, stiffness, width));
      for (int i = 1; i <= n - 2; i++)
        m_top_ss.push_back (new Spring (m_top_vs[i - 1], m_top_vs[i],
                                        damping, stiffness, width));
      for (int i = 1; i <= n - 1; i++)
        m_rl_ss.push_back (new Spring (m_bottom_vs[i - 1], m_top_vs[i - 1],
                                       damping, stiffness, diagonal));
      for (int i = 1; i <= n - 1; i++)
        m_ll_ss.push_back (new Spring (m_bottom_vs[i], m_top_vs[i - 1],
                                       damping, stiffness, diagonal));
      
      for (int i = 0; i < n - 1; i++) {
        m_all_ss.push_back (m_rl_ss[i]);
        m_all_ss.push_back (m_ll_ss[i]);
        m_all_ss.push_back (m_bottom_ss[i]);
        if (i < n - 2)
          m_all_ss.push_back (m_top_ss[i]);
      }
    }
    
    void render () {
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho (-50, 150, -100, 100, 0, 1);
      glMatrixMode (GL_MODELVIEW);
      
      glClearColor (.6, .2, .8, 0);
      glClear (GL_COLOR_BUFFER_BIT);
      glPointSize (4);
      glDisable (GL_DEPTH_TEST);
      
      glEnable (GL_LINE_SMOOTH);
      glEnable (GL_POINT_SMOOTH);
      glEnable (GL_POLYGON_SMOOTH);
      
      glBegin (GL_POINTS);
      glColor3f (1, 1, 1);
      for (int i = 0; i < n - 1; i++)
        glVertex2f (m_top_vs[i]->position ().x, m_top_vs[i]->position ().y);
      for (int i = 0; i < n; i++)
        glVertex2f (m_bottom_vs[i]->position ().x, m_bottom_vs[i]->position ().y);
      glEnd ();
      
      glBegin (GL_LINES);
      for (int i = 0; i < m_all_ss.size (); i++) {
        if (!m_all_ss[i]->is_snapped ()) {
          Vector2D a = m_all_ss[i]->point_a ().position ();
          Vector2D b = m_all_ss[i]->point_b ().position ();
          glVertex2f (a.x, a.y);
          glVertex2f (b.x, b.y);
        }
      }
      glEnd ();
    }
    
    void punt () {
      m_punt_time = 0.2;
      m_punt_force = Vector2D::random_unit () * 400;
    }
    
    void snap () {
      int i = random () % m_all_ss.size ();
      m_all_ss[i]->snap ();
    }
    
    void unsnap () {
      for (int i = 0; i < m_all_ss.size (); i++)
        m_all_ss[i]->unsnap ();
    }
    
    void step () {
      scalar_t dt = m_stop_watch.lap ();
      for (int i = 0; i < n - 1; i++) {
        m_top_vs[i]->reset_force ();
        m_top_vs[i]->add_gravity (9.82);
      }
      for (int i = 0; i < n; i++) {
        m_bottom_vs[i]->reset_force ();
        m_bottom_vs[i]->add_gravity (9.82);
      }
      
      if (m_punt_time > 0.0) {
        m_punt_time = max (m_punt_time - dt, 0.0f);
        for (int i = 0; i < n - 1; i++)
          m_top_vs[i]->add_force (m_punt_force);
        for (int i = 0; i < n; i++)
          m_bottom_vs[i]->add_force (m_punt_force);
      }
      
      for (int i = 0; i < n - 2; i++)
        m_top_ss[i]->add_forces_to_point_masses ();
      for (int i = 0; i < n - 1; i++)
        m_bottom_ss[i]->add_forces_to_point_masses ();
      for (int i = 0; i < n - 1; i++)
        m_rl_ss[i]->add_forces_to_point_masses ();
      for (int i = 0; i < n - 1; i++)
        m_ll_ss[i]->add_forces_to_point_masses ();

      for (int i = 0; i < n - 1; i++)
        m_top_vs[i]->step (dt);
      for (int i = 0; i < n; i++)
        m_bottom_vs[i]->step (dt);

      printf ("step %f\n", dt);
    }
    
  private:
    std::vector<PointMass*> m_top_vs;
    std::vector<PointMass*> m_bottom_vs;
    std::vector<PointMass*> m_all_vs;
    std::vector<Spring*> m_top_ss;
    std::vector<Spring*> m_rl_ss;
    std::vector<Spring*> m_ll_ss;
    std::vector<Spring*> m_bottom_ss;
    std::vector<Spring*> m_all_ss;
    StopWatch m_stop_watch;
    
    scalar_t m_punt_time;
    Vector2D m_punt_force;
  };
}

#endif