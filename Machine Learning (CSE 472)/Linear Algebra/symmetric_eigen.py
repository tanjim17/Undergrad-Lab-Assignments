import numpy as np

MIN = 0
MAX = 100

print("Enter matrix dimension:")
n = int(input())

# Create a random integer invertible symmetric matrix
A = np.random.randint(MIN, MAX, (n, n))
A = A + A.T
while np.linalg.det(A) == 0:
    A = np.random.randint(MIN, MAX, (n, n))
    A = A + A.T
print(A)

# Eigen decomposition
eigenvalues, eigenvectors = np.linalg.eig(A)
print("Eigenvalues: ", eigenvalues)
print("Eigenvectors: ", eigenvectors)

# Reconstruct the matrix
B = eigenvectors.dot(np.diag(eigenvalues)).dot(np.linalg.inv(eigenvectors))
print(B)

# Check if the two matrices are equal
print(np.allclose(A, B))