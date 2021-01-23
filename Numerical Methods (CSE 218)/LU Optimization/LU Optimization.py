import numpy as np

file = open('in1.txt')

n = int(file.readline())

A = np.empty((n, n))
B = np.empty((n))
L = np.identity(n) 

for i in range(n):
    A[i] = list(map(float, file.readline().split()))
    
U = np.copy(A)
   
for i in range(n):
    B[i] = float(file.readline())
    
file.close()
 
for i in range(n):
        
    for j in range(i+1, n):  #gaussian elimination
        factor = U[j][i] / U[i][i]
        
        U[j] -= U[i] * factor
        #B[j] -= B[i] * factor
        
        L[j][i] = factor

file = open('out1.txt', 'w')
 
print('L Matrix:\n', L, '\n')
np.savetxt(file, L, fmt = '%.4f')
file.write('\n')
print('U Matrix:\n', U, '\n')
np.savetxt(file, U, fmt = '%.4f')
file.write('\n')


for i in range(n):
    for j in range(n):
        if U[i][j] != 0:
            break
    if j == n:
        print('No unique solution\n')
        file.write('No unique solution\n')
        exit(1)
        
X = np.empty((n))
Y = np.empty((n))         

for i in range(n):  #forward substitution to find Y, LY = B
    y = B[i]
    
    for j in range(i):
        y -= L[i][j] * Y[j]  
    
    y /= L[i][i]
    
    Y[i] = y
    
for i in range(n-1, -1, -1):  #backward substitution to find X, UX = Y
    x = Y[i]
    
    for j in range(i+1, n):
        x -= U[i][j] * X[j]
        
    x /= U[i][i]
    
    X[i] = x
 
print('Y Matrix:\n', Y, '\n')
np.savetxt(file, Y, fmt = '%.4f')
file.write('\n')
print('X Matrix:\n', X, '\n')
np.savetxt(file, X, fmt = '%.4f')
file.write('\n')
            
file.close()        
            
            
            
            
            
            
            
            
            
            
            
            
            
            

        
 
    
