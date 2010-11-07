#lang racket/gui

(define (^2 x) (* x x))

(define (distance u v)
  (sqrt (+ (^2 (- (real-part u) (real-part v)))
           (^2 (- (imag-part u) (imag-part v))))))

(define wind
  (let [(alpha 0.999)
        (x 0)]
    (lambda ()
      (let [(y (* 2 (- 0.5 (random))))]
        (set! x (+ (* x alpha) (* y (- 1 alpha))))
        x))))

(define (rk4 f h y0)
  (let* [(hh (/ h 2))
         (k1 (* h (f 0 y0)))
         (k2 (* h (f hh (+ y0 (/ k1 2)))))
         (k3 (* h (f hh (+ y0 (/ k2 2)))))
         (k4 (* h (f h (+ y0 k3))))]
    (+ y0 (* 1/6 (+ k1 (* 2 k2) (* 2 k3) k4)))))

(define (euler f h y0)
  (+ y0 (* h (f 0 y0))))

(define integrate rk4)

(define (update-velocity v0 a dt)
  (integrate (lambda (t y)
               (+ y (* a t))) dt v0))

(define (update-position p0 a v dt)
  (let [(one-dimension
         (lambda (px ax vx)
           (integrate (lambda (t y)
                        (+ vx (* ax t))) dt px)))]
    (make-rectangular (one-dimension (real-part p0)
                                     (real-part a)
                                     (real-part v))
                      (one-dimension (imag-part p0)
                                     (imag-part a)
                                     (imag-part v)))))
         
(define +water-line+ 500)

(define vertex%
  (class object%
    (init-field position mass)
    
    (define velocity 0)
    (define frame-force 0)
    
    (super-new)
    
    (define/public (get-position) position)
    (define/public (get-velocity) velocity)
    
    (define/private (get-acceleration)
      (/ frame-force mass))
    
    (define/public (reset-force)
      (set! frame-force 0))
    
    (define/public (add-gravity g)
      (add-force (make-rectangular 0 (* mass g))))
    
    (define/public (add-force f)
      (set! frame-force (+ frame-force f)))
    
    (define/public (step dt)
;;      (when (< (magnitude velocity) 0.1)
      (add-force (* (wind) 200))
      (set! position (update-position position (get-acceleration) velocity dt))
;      (set! velocity (update-velocity velocity (get-acceleration) dt)))))
;      (set! position (+ position
 ;                       (* velocity dt)
  ;                      (* 0.5 (get-acceleration) (^2 dt))))
      (set! velocity (+ velocity (* (get-acceleration) dt))))))

(define fixed-vertex%
  (class vertex%
    (super-new)
    
    (define/override (add-force f)
      (void))))

(define spring%
  (class object% 
    (init-field damping stiffness rest-length vertex-a vertex-b)
    
    (super-new)
    
    (define max-length-factor 1.5)
    (define frequency (* (random) 20))
    (define current-rest-length rest-length)
    (define phase 0)
    
    (define broken? #f)
    
    (define/public (is-broken?) broken?)
    
    (define/public (get-vertex-positions)
      (values (send vertex-a get-position)
              (send vertex-b get-position)))
    
    (define/public (get-length-factor)
      (/ (distance (send vertex-a get-position)
                   (send vertex-b get-position))
         current-rest-length))
    
    (define/public (step dt)
      (void))
    
;    (define/public (step dt)
 ;     (set! phase (+ phase (* dt frequency)))
  ;    (set! current-rest-length (* (+ 0.9 (* 0.3 (/ (+ 1 (sin phase)) 2)))
   ;                                rest-length)))
    
    (define/public (add-forces)
      (when (> (get-length-factor) max-length-factor)
        (set! broken? #t))
      (unless broken?
        (let [(p1 (send vertex-a get-position))
              (p2 (send vertex-b get-position))]
          (let* [(magnitude (* (- stiffness)
                               (- (distance (send vertex-a get-position)
                                            (send vertex-b get-position))
                                  current-rest-length)))
                 (f (make-polar magnitude (angle (- p1 p2))))]
            (send vertex-a add-force 
                  (+ f (* (- damping)
                          (- (send vertex-a get-velocity)
                             (send vertex-b get-velocity)))))
            (send vertex-b add-force 
                  (+ (- f) (* (- damping)
                              (- (send vertex-b get-velocity)
                                 (send vertex-a get-velocity)))))))))))
  
(define (call-forever thunk)
  (thunk)
  (call-forever thunk))

(define-syntax-rule (forever expr ...)
  (call-forever (lambda ()
                  expr ...)))

(define (spring-color spring)
  (let [(anger (send spring get-length-factor))]
    (if (< anger 1)
        (make-object color% 0 (inexact->exact (round (* (- 1 anger) 255))) 0)
        (make-object color% (min 255 (inexact->exact (round (* (- anger 1) 2550))))
          0 0))))

(define world-weird
  (new (class object%
         (super-new)
         
         (define n 20)
         
         (define my-stiffness 40)
         (define my-damping 0.9)
         (define width 100)
         (define height 150)
         (define diagonal (sqrt (+ (sqr (/ width 2)) (sqr height))))
         
         (define vertex-a (new fixed-vertex% [position width] [mass 1]))
         (define vertex-b (new fixed-vertex% [position (make-rectangular (+ width (* (- n 1) width)) 0)] [mass 1]))
         
         (define top-vertices
           (for/list ([i (in-range 1 n)])
             (new vertex% [position (make-rectangular 
                                     (+ width (- (/ width 2)) (* i width))
                                     (- height))]
                  [mass 1])))
         
         (define bottom-vertices
           (append (list vertex-a)
                   (for/list ([i (in-range 1 (- n 1))])
                     (new vertex% [position (make-rectangular 
                                             (+ width (* i width))
                                             0)]
                          [mass 1]))
                   (list vertex-b)))
         
         (define vertices (append top-vertices bottom-vertices))
         
         (define bottom-springs
           (for/list ([i (in-range 1 n)])
             (new spring%
                  [vertex-a (list-ref bottom-vertices (- i 1))]
                  [vertex-b (list-ref bottom-vertices i)]
                  [damping my-damping]
                  [stiffness my-stiffness]
                  [rest-length width])))
         
         (define top-springs
           (for/list ([i (in-range 1 (- n 1))])
             (new spring%
                  [vertex-a (list-ref top-vertices (- i 1))]
                  [vertex-b (list-ref top-vertices i)]
                  [damping my-damping]
                  [stiffness my-stiffness]
                  [rest-length (* 0.7 width)])))
         
         (define forward-springs
           (for/list ([i (in-range 1 n)])
             (new spring%
                  [vertex-a (list-ref bottom-vertices (- i 1))]
                  [vertex-b (list-ref top-vertices (- i 1))]
                  [damping my-damping]
                  [stiffness my-stiffness]
                  [rest-length diagonal])))
         
         (define backward-springs
           (for/list ([i (in-range 1 n)])
             (new spring%
                  [vertex-a (list-ref top-vertices (- i 1))]
                  [vertex-b (list-ref bottom-vertices i)]
                  [damping my-damping]
                  [stiffness my-stiffness]
                  [rest-length diagonal])))
         
         (define springs (append bottom-springs
                                 top-springs
                                 forward-springs
                                 backward-springs
                                 ))

         (define/public (step dt)
           (for-each (lambda (vertex)
                       (send vertex reset-force)
                       (send vertex add-gravity 9.82)
                       ) vertices)
           (for-each (lambda (spring)
                       (send spring step dt)
                       (send spring add-forces)) springs)
           (for-each (lambda (vertex)
                        (send vertex step dt)) vertices))
         
         (define/private (draw-vertices dc)
           (send dc set-pen (make-object color% 0 0 0) 1 'solid)
           (for-each (lambda (vertex)
                       (let [(p (send vertex get-position))]
                         (send dc draw-ellipse 
                               (- (real-part p) 5)
                               (- (imag-part p) 5) 10 10)))
                     vertices))
         
         (define/private (draw-springs dc)
           (for-each (lambda (spring)
                       (unless (send spring is-broken?)
                         (let-values ([(p1 p2) (send spring get-vertex-positions)])
                           (send dc set-pen (spring-color spring) 2 'solid)
                           (send dc draw-line
                                 (real-part p1) (imag-part p1)
                                 (real-part p2) (imag-part p2)))))
                     springs))
         
         (define/public (draw dc)
           (draw-springs dc)
           (draw-vertices dc)))))

(define world-simple
  (new (class object%
         (super-new)
         
         (define n 4)
         
         (define vertex-a (new fixed-vertex% [position 100+100i] [mass 1]))
         (define vertex-b (new vertex% [position 100+50i] [mass 1]))
         (define vertex-c (new fixed-vertex% [position 200+100i] [mass 1]))
 
         (define vertices (list vertex-a vertex-b vertex-c))
         
         (define springs
           (list
             (new spring%
                  [vertex-a vertex-a]
                  [vertex-b vertex-b]
                  [damping 0.8]
                  [stiffness 20]
                  [rest-length 50])))

         (define/public (step dt)
           (for-each (lambda (vertex)
                       (send vertex reset-force)
;                       (send vertex add-gravity 9.82)
                       ) vertices)
;           (for-each (lambda (spring)
;                       (send spring add-forces)) springs)
           (for-each (lambda (vertex)
                        (send vertex step dt)) vertices))
         
         (define/private (draw-vertices dc)
           (send dc set-pen (make-object color% 0 0 0) 1 'solid)
           (for-each (lambda (vertex)
                       (let [(p (send vertex get-position))]
                         (send dc draw-ellipse 
                               (- (real-part p) 5)
                               (- (imag-part p) 5) 10 10)))
                     vertices))
         
         (define/private (draw-springs dc)
           (for-each (lambda (spring)
                       (unless (send spring is-broken?)
                         (let-values ([(p1 p2) (send spring get-vertex-positions)])
                           (send dc set-pen (spring-color spring) 2 'solid)
                           (send dc draw-line
                                 (real-part p1) (imag-part p1)
                                 (real-part p2) (imag-part p2)))))
                     springs))
         
         (define/public (draw dc)
           (draw-springs dc)
           (draw-vertices dc)))))

(define frame
  (new frame% [label "Vertexarnas krig"]
       [width 1100] [height 800]))

(define buffer
  (make-object bitmap% 1100 800))

(define buffer-dc
  (let [(dc (new bitmap-dc% [bitmap buffer]))]
    (send dc set-smoothing 'smoothed)
    (send dc set-origin 200 200)
    (send dc set-scale 0.3 0.3)
    (send dc set-background (make-object color% "DeepSkyBlue"))
    dc))

(define current-world world-weird)

(new (class canvas%
       (super-new)
       (define/override (on-char event)
         (let [(t0 (current-milliseconds))]
           (forever
             (let [(t (current-milliseconds))]
               (send current-world step (/ (- t t0) 1000))
               (set! t0 t)
               (send this on-paint))))))

     [parent frame]
     [paint-callback (lambda (canvas dc)
                       (send buffer-dc clear)
                       (send current-world draw buffer-dc)
                       (send dc draw-bitmap buffer 0 0))])

(send frame show #t)