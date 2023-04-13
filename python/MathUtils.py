import numpy as np



class MathUtils:

    @staticmethod
    def angles_360_to_180(angles):
        for i in range(angles.shape[0]):
            if(angles[i] < -np.pi):
                angles[i] += np.pi*2.0
            elif(angles[i] > np.pi): 
                angles[i] -= np.pi*2.0

    @staticmethod
    def angle_360_to_180(angle):
        if(angle < -np.pi):
            return angle + np.pi*2.0
        elif(angle > np.pi): 
            return angle - np.pi*2.0
        return angle

    @staticmethod
    def vec4(x=0.0, y=0.0, z=0.0, w=1.0):
        return np.array([x,y,z,w], dtype=np.float64)

    @staticmethod
    def vec3(x=0.0, y=0.0, z=0.0):
        return np.array([x,y,z], dtype=np.float64)

    @staticmethod
    def matrix4x4():
        return np.identity(n=4, dtype=np.float64)

    @staticmethod
    def matrix_translation(out, translation):
        out[:,:] = 0.0
        out[0,0] = 1.0
        out[1,1] = 1.0
        out[2,2] = 1.0
        out[3,3] = 1.0
        out[0,3] = translation[0]
        out[1,3] = translation[1]
        out[2,3] = translation[2]

    @staticmethod
    def matrix_rotation_x(out, angle):
        out[:,:] = 0.0
        out[0,0] = 1.0
        out[1,1] = +np.cos(angle)
        out[1,2] = -np.sin(angle)
        out[2,1] = +np.sin(angle)
        out[2,2] = +np.cos(angle)
        out[3,3] = 1.0

    @staticmethod
    def matrix_rotation_y(out, angle):
        out[:,:] = 0.0
        out[0,0] = +np.cos(angle)
        out[0,2] = +np.sin(angle)
        out[1,1] = 1.0
        out[2,0] = -np.sin(angle)
        out[2,2] = +np.cos(angle)
        out[3,3] = 1.0

    @staticmethod
    def matrix_rotation_z(out, angle):
        out[:,:] = 0.0
        out[0,0] = +np.cos(angle)
        out[0,1] = -np.sin(angle)
        out[1,0] = +np.sin(angle)
        out[1,1] = +np.cos(angle)
        out[2,2] = 1.0
        out[3,3] = 1.0

