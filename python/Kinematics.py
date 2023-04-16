
from MathUtils import MathUtils
import numpy as np

class Kinematics:

    def __init__(self):
        
        # hips offsets
        diagonal = 0.155*0.5
        dx = diagonal / np.sqrt(2)
        dy = diagonal / np.sqrt(2)

        # hips translation vectors
        self.hips_translation = np.array([
            [+dx, +dy, 0.0, 1.0],
            [-dx, +dy, 0.0, 1.0],
            [-dx, -dy, 0.0, 1.0],
            [+dx, -dy, 0.0, 1.0],
        ], dtype=np.float64)

        # hips rotations around y
        self.hips_rotation_z = np.deg2rad(np.array([45.0, 135.0, -135.0, -45.0], dtype=np.float64))

        # hips transformation matrices
        self.hips_matrix = np.zeros(shape=(self.hips_translation.shape[0],4,4), dtype=np.float64)
        self.hips_matrix_inv = np.zeros(shape=(self.hips_translation.shape[0],4,4), dtype=np.float64)
        for i in range(self.hips_translation.shape[0]):
            Ti = MathUtils.matrix4x4()
            Ri = MathUtils.matrix4x4()
            MathUtils.matrix_translation(Ti, self.hips_translation[i])
            MathUtils.matrix_rotation_z(Ri, self.hips_rotation_z[i])
            self.hips_matrix[i] = Ti@Ri
            self.hips_matrix_inv[i] = np.linalg.inv(self.hips_matrix[i])

        # phi min and max angles
        self.phi_min = np.deg2rad(-50.0)
        self.phi_max = np.deg2rad(50.0)

        # limb lengths
        self.lengths = np.array([0.07, 0.07, 0.07, 0.096], dtype=np.float64)

        # joint angle limits
        self.limits_min = np.deg2rad([-90.0, -90.0, -90.0, -90.0], dtype=np.float64)
        self.limits_max = np.deg2rad([+90.0, +90.0, +90.0, +90.0], dtype=np.float64)


    def ik(self, Q, phi, angles_out):

        L0 = self.lengths[0]
        L1 = self.lengths[1]
        L2 = self.lengths[2]
        L3 = self.lengths[3]

        xy = np.sqrt(Q[0] * Q[0] + Q[1] * Q[1]) - L0 - L3 * np.sin(phi)
        z = Q[2] + L3 * np.cos(phi)

        v = (xy * xy + z * z - L1 * L1 - L2 * L2) / (2.0 * L1 * L2)

        if(v < -1.0 or v > 1.0):
            return False, False
        
        limit = False
        
        a2 = -np.arccos(v)
        a2 = MathUtils.angle_360_to_180(a2)
        if(a2 < self.limits_min[2] or a2 > self.limits_max[2]):
            limit = True

        a1 = np.arctan2(z, xy) - np.arctan2(L2 * np.sin(a2), L1 + L2 * np.cos(a2))
        a1 = MathUtils.angle_360_to_180(a1)
        if(a1 < self.limits_min[1] or a1 > self.limits_max[1]):
            limit = True

        a3 = phi - a1 - a2 - np.pi * 0.5
        a3 = MathUtils.angle_360_to_180(a3)
        if(a3 < self.limits_min[3] or a3 > self.limits_max[3]):
            limit = True

        a0 = np.arctan2(Q[1], Q[0])
        a0 = MathUtils.angle_360_to_180(a0)
        if(a0 < self.limits_min[0] or a0 > self.limits_max[0]):
            limit = True
        
        angles_out[0] = a0
        angles_out[1] = a1
        angles_out[2] = a2
        angles_out[3] = a3
        
        return True, limit


    def fk(self, angles, pos_out):

        cos_a0 = np.cos(angles[0])
        sin_a0 = np.sin(angles[0])

        pos_out[0][0] = self.lengths[0] * cos_a0
        pos_out[0][1] = self.lengths[0] * sin_a0
        pos_out[0][2] = 0.0

        angle_cum = 0.0
        for i in range(1,4):
            angle_cum += angles[i]
            cos_cum = np.cos(angle_cum) * self.lengths[i]
            pos_out[i][0] = pos_out[i-1][0] + cos_cum * cos_a0
            pos_out[i][1] = pos_out[i-1][1] + cos_cum * sin_a0
            pos_out[i][2] = pos_out[i-1][2] - np.sin(angle_cum) * self.lengths[i]

    def loss(self, phi_target, phi_actual, phi_old=None, phi_weight=1.0, angles_old=None, angles_new=None, angles_weight=0.5):

        phi_error_weight = phi_weight
        phi_delta_weight = 1.0

        loss = 0.0

        if(phi_target is not None):
            phi_error = phi_target - phi_actual
            # loss += (np.square(phi_delta) + np.abs(phi_delta)) * phi_weight
            loss += np.square(phi_error) * phi_error_weight
        
        if(phi_old is not None):
            phi_delta = phi_old - phi_actual
            loss += np.square(phi_delta) * phi_delta_weight


        if(angles_old is not None):
            angles_delta = angles_old-angles_new
            # loss += np.amax(np.square(angles_delta) + np.abs(angles_delta)) * angles_weight
            loss += np.sum(np.square(angles_delta)) * angles_weight

        return loss

    def search(self, Q, phi_target, angles_out, angles_old=None, num_phi_vals=255, w_phi=1.0, w_angles=0.5, phi_old=None):

        phi_vals = np.linspace(self.phi_min, self.phi_max, num_phi_vals)
        best_loss = np.inf
        best_loss_idx = -1
        angles_ik = np.zeros(4, dtype=np.float64)

        for idx, phi in enumerate(phi_vals):

            result = self.ik(Q, phi, angles_ik)

            if(result):
                loss_val = self.loss(
                    phi_target=phi_target, 
                    phi_actual=phi, 
                    phi_old=phi_old,
                    angles_old=angles_old, 
                    angles_new=angles_ik, 
                    phi_weight=w_phi, 
                    angles_weight=w_angles,
                )
                if(loss_val < best_loss):
                    best_loss_idx = idx
                    best_loss = loss_val

        print(best_loss)

        if(best_loss_idx == -1):
            has_result = False
            best_phi = np.nan

        else:
            has_result = True
            best_phi = phi_vals[best_loss_idx]
            self.ik(Q, best_phi, angles_out)

        return has_result, best_phi
    
    def search_brent(self, Q, phi_target, angles_out, angles_old=None):

        a = self.phi_min
        b = self.phi_max
        tol = 1.0e-6
        max_iter=100

        angles_ik = np.zeros(4, dtype=np.float64)
        golden_ratio = (3.0 - np.sqrt(5.0)) / 2.0

        x = a + golden_ratio * (b - a)
        w = x
        v = x

        result = self.ik(Q, x, angles_ik)
        if(result):
            fx = self.loss(phi_target, phi_actual=x, angles_old=angles_old, angles_new=angles_ik)
        else:
            fx = 1.0e6

        fw = fx
        fv = fx

        u = 0
        p = 0
        q = 0

        for _ in range(max_iter):

            print(f"x={np.rad2deg(x):.3f}, u={np.rad2deg(u):.3f}, a={np.rad2deg(a):.3f}, b={np.rad2deg(b):.3f} p={np.rad2deg(p):.3f} q={np.rad2deg(q):.3f}")

            midpoint = (a + b) / 2
            tol1 = tol * abs(x) + tol / 10
            tol2 = 2 * tol1

            if abs(x - midpoint) <= (tol2 - (b - a) / 2):
                return True, x

            p = 0
            q = 0
            r = 0

            if x != w and x != v and w != v:
                r = (x - w) * (fx - fv)
                q = (x - v) * (fx - fw)
                p = (x - v) * q - (x - w) * r
                q = 2 * (q - r)

                if q > 0:
                    p = -p
                else:
                    q = -q

            if abs(p) < abs(q * tol1) and p > q * (a - x) and p < q * (b - x):
                u = x + p / q
            else:
                u = x + tol1 if x < midpoint else x - tol1
                u = max(min(u, b), a)


            result = self.ik(Q, u, angles_ik)
            if(result):
                fu = self.loss(phi_target, phi_actual=u, angles_old=angles_old, angles_new=angles_ik)
            else:
                fu = 1.0e6

            if fu <= fx:
                if u >= x:
                    a = x
                else:
                    b = x

                v = w
                w = x
                x = u
                fv = fw
                fw = fx
                fx = fu
            else:
                if u < x:
                    a = u
                else:
                    b = u

                if fu <= fw or w == x:
                    v = w
                    w = u
                    fv = fw
                    fw = fu
                elif fu <= fv or v == x or v == w:
                    v = u
                    fv = fu

        return False, np.nan
    