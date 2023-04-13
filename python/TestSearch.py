from Kinematics import Kinematics
import numpy as np
from MathUtils import MathUtils
import time
from Brent import Brent


if __name__ == "__main__":


    kin = Kinematics()
    brent = Brent(kin)


    angles_out = np.zeros(4, dtype=np.float64)
    angles_old = np.zeros(4, dtype=np.float64)
    
    Q = np.array([0.2, 0.0, 0.0], dtype=np.float64) 
    phi = np.deg2rad(0.0)

    t_start = time.time()    
    has_result, best_phi = kin.search(Q, phi, angles_out=angles_out, angles_old=angles_old)
    t_total = time.time()-t_start
    loss = kin.loss(phi_target=phi, phi_actual=best_phi, angles_old=angles_old, angles_new=angles_out)
    print(f"search: time={t_total*1000:.2f}ms")
    print(f"has_result={has_result}, best_phi={np.rad2deg(best_phi)}, loss={loss}, angles_out={np.rad2deg(angles_out)}")

    # t_start = time.time()    
    # has_result, best_phi = ik.kin.search_brent(Q, phi, angles_out=angles_out, angles_old=angles_old)
    # t_total = time.time()-t_start
    # loss = ik.kin.loss(phi_target=phi, phi_actual=best_phi, angles_old=angles_old, angles_new=angles_out)
    # print(f"search_brent: time={t_total*1000:.2f}ms")
    # print(f"has_result={has_result}, best_phi={np.rad2deg(best_phi)}, loss={loss}, angles_out={np.rad2deg(angles_out)}")

    t_start = time.time()    
    brent.Max = 100
    brent.prepare(Q, phi, angles_out=angles_out, angles_old=angles_old)
    best_phi = brent.brentMethod(kin.phi_min, kin.phi_max)
    print(best_phi)
    t_total = time.time()-t_start
    has_result = kin.ik(Q, best_phi, angles_out)
    loss = kin.loss(phi_target=phi, phi_actual=best_phi, angles_old=angles_old, angles_new=angles_out)
    print(f"search_brent: time={t_total*1000:.2f}ms")
    print(f"has_result={has_result}, best_phi={np.rad2deg(best_phi)}, loss={loss}, angles_out={np.rad2deg(angles_out)}")




