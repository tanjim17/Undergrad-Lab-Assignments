import numpy as np

MIN = 0
MAX = 100

n = int(input("Enter matrix row count: "))
m = int(input("Enter matrix column count: "))

# Create a random integer matrix
A = np.random.randint(MIN, MAX, (n, m))

# Singular Value Decomposition
U, D, V_T = np.linalg.svd(A)
print("U: ", U)
print("D: ", D)
print("V_T: ", V_T)

# Moore-Penrose Pseudoinverse using NumPy’s library function
A_pinv = np.linalg.pinv(A)

# Moore-Penrose Pseudoinverse using Eq. 2.47
D_inv = np.diag(1/D)
if n > m:
    D_plus = np.concatenate((D_inv, np.zeros((m, n - m))),axis = 1)
else:
    D_plus = np.concatenate((D_inv, np.zeros((m - n, n))),axis = 0)
A_pinv2 = V_T.T.dot(D_plus).dot(U.T)

#Check if the two matrices are equal
print(np.allclose(A_pinv, A_pinv2))