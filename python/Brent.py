
from MathUtils import MathUtils
import numpy as np

class Brent:

    def __init__(self, kinematics):
        
        self.kin = kinematics

        self.Q = None
        self.phi_target = None
        self.angles_out = None
        self.angles_old = None
        self.angles_ik = np.zeros(4, np.float32)

        self.Max = 100
        self.eps = 10.0**(-6.0)
        self.invphi = (1.0 + np.sqrt(5.0)) / 2.0 - 1.0
        self.ratio = (3.0 - np.sqrt(5.0)) / 2.0
        
    def prepare(self, Q, phi_target, angles_out, angles_old=None):

        self.Q = Q
        self.phi_target = phi_target
        self.angles_out = angles_out
        self.angles_old = angles_old

    def f(self, x):
        result = self.kin.ik(self.Q, x, self.angles_ik)
        if(result):
            return self.kin.loss(self.phi_target, phi_actual=x, angles_old=self.angles_old, angles_new=self.angles_ik)
        return np.Infinity
    
    """
    Recursive loop of Brent's Minimization Method
    @param a left interval
    @param b right interval
    @param v previous iterate value
    @param w previous iterate value
    @param x previous iterate value
    @param dold last delta step
    @param eold last golden interval size
    @param i iteration counter
    @return minimum
    """
    def brent(self, a, b, v, w, x, dold, eold, i):

        fv = self.f(v)
        fw = self.f(w)
        fx = self.f(x)
        newi = i + 1.0
        m = 0.5 * (a + b)

        if b - a <= self.eps or i > self.Max:

            return m
        
        else:
            
            r = (x - w) * (fx - fv)
            tq = (x - v) * (fx - fw)
            tp = (x - v) * tq - (x - w) * r
            tq2 = 2.0 * (tq - r)
            p = -tp if tq2 > 0.0  else tp
            q = tq2 if tq2 > 0.0 else -tq2
            safe = q != 0.0
            deltax = p / q if safe else 0.0
            parabolic = safe and a < x + deltax and x + deltax < b and abs(deltax) < 0.5 * abs(eold)

            if parabolic:
                e = dold
            elif x < m:
                e = b - x
            else: 
                e = a - x

            if parabolic:
                d = deltax
            else:
                d = self.ratio * e

            u = x + d
            fu = self.f(u)
            
            if fu <= fx:
                newa = a if u < x else x
                newb = x if u < x else b
                return self.brent(newa, newb, w, x, u, d, e, newi)
            else:
                newa = u if u < x else a
                newb = b if u < x else u
                if fu <= fw or w == x:
                    return self.brent(newa, newb, w, u, x, d, e, newi)
                elif fu <= fv or v == x or v == w:
                    return self.brent(newa, newb, u, w, x, d, e, newi)
                else:
                    return self.brent(newa, newb, v, w, x, d, e, newi)


    def brentMethod(self, a, b):
        x = b + self.invphi * (a - b)
        return self.brent(a, b, x, x, x, 0.0, 0.0, 0)