from Kinematics import Kinematics
import numpy as np
from MathUtils import MathUtils
import time

class JetsonIK:

    def __init__(self):
        self.kin = Kinematics()

    def test_ik_fk(self):

        angles_out = np.zeros(4, dtype=np.float64)
        angles_old = np.zeros(4, dtype=np.float64)
        
        Q = np.array([0.2, 0.0, 0.0], dtype=np.float64) 
        phi = np.deg2rad(0.0)

        has_result, best_phi = self.kin.search(Q, phi, angles_out=angles_out, angles_old=angles_old)
        
        print(has_result, best_phi)
        print(angles_out)

        pos_out = [np.zeros(3, dtype=np.float64) for i in range(4)]
        self.kin.fk(angles_out, pos_out)

        print(f"Q={Q}")
        print("forward kinematics:")
        for i in range(4):
            print(pos_out[i])

    def surf(self):

        hip_angle = np.deg2rad(0.0)
        foot_xy = 0.15
        foot_z = -0.15
        foot_phi = np.deg2rad(0.0)

        hip_angles = [0.0 for i in range(4)]
        foot_phis = [0.0 for i in range(4)]
        foot_pos_relative = [MathUtils.vec4() for i in range(4)]
        foot_pos_transformed = [MathUtils.vec4() for i in range(4)]

        surf_translation = MathUtils.vec4()
        surf_rotation = MathUtils.vec3()

        T_surf = MathUtils.matrix4x4()
        Rx_surf = MathUtils.matrix4x4()
        Ry_surf = MathUtils.matrix4x4()
        Rz_surf = MathUtils.matrix4x4()

        angles_out = np.array([np.zeros(shape=(4), dtype=np.float64) for i in range(4)], dtype=np.float64)
        angles_old = np.array([np.zeros(shape=(4), dtype=np.float64) for i in range(4)], dtype=np.float64)

        while(True):

            t_start = time.time()

            # compute hip angles
            hip_angles[0] = +hip_angle
            hip_angles[1] = -hip_angle
            hip_angles[2] = +hip_angle
            hip_angles[3] = -hip_angle

            # compute foot angles
            for i in range(4):
                foot_phis[i] = foot_phi

            # compute relative foot positions
            for i in range(4):
                foot_pos_relative[i][0] = foot_xy * np.cos(hip_angles[i])
                foot_pos_relative[i][1] = foot_xy * np.sin(hip_angles[i])
                foot_pos_relative[i][2] = foot_z

            # update surf translation 
            animation_speed = 1.0
            # surf_translation[0] = np.cos(t_start * np.pi * 2.0 * 0.25 * animation_speed) * 0.05
            # surf_translation[1] = np.sin(t_start * np.pi * 2.0 * 0.25 * animation_speed) * 0.05
            # surf_translation[2] = np.sin(t_start * np.pi * 2.0 * 0.25 * animation_speed) * 0.05

            # update surf rotation
            # surf_rotation[0] = np.sin(t_start * np.pi * 2.0 * 0.25 * animation_speed) * np.deg2rad(15.0)
            # surf_rotation[1] = np.sin(t_start * np.pi * 2.0 * 0.25 * animation_speed) * np.deg2rad(15.0)
            surf_rotation[2] = np.sin(t_start * np.pi * 2.0 * 0.25 * animation_speed) * np.deg2rad(30.0)

            # compute surf transformation matrix
            MathUtils.matrix_translation(T_surf, surf_translation)
            MathUtils.matrix_rotation_x(Rx_surf, surf_rotation[0])
            MathUtils.matrix_rotation_y(Ry_surf, surf_rotation[1])
            MathUtils.matrix_rotation_z(Rz_surf, surf_rotation[2])
            M_surf = T_surf @ Rz_surf @ Rx_surf @ Ry_surf
            M_surf_inv = np.linalg.inv(M_surf)

            # compute transformed foot positions
            for i in range(4):
                foot_pos_transformed[i] = self.kin.hips_matrix_inv[i] @ M_surf_inv @ self.kin.hips_matrix[i] @ foot_pos_relative[i]

            # compute inverse kinematics for all feet
            no_results = []
            for i in range(4):
                has_result, best_phi = self.kin.search(foot_pos_transformed[i], foot_phis[i], angles_out[i], angles_old[i], num_phi_vals=201)
                if(not has_result):
                    no_results.append(i)

            # do something with computed ik results
            if(len(no_results) > 0):

                # ik failed, do nothing
                print(f"no ik results for some legs: {no_results}")

            else:

                # move joint angles to new position
                print(f"surf result success")
                for i in range(4):
                    print(f"leg-{i} angles: {np.rad2deg(angles_out[i])}")
                    np.copyto(angles_old, angles_out)            

            # print time
            t_total = time.time() - t_start
            print(f"t_total={t_total*1000.0:.2f}ms")
            print("")

            # wait 
            # time.sleep(1)


if __name__ == "__main__":


    ik = JetsonIK()

    # ik.test_ik_fk()

    ik.surf()


