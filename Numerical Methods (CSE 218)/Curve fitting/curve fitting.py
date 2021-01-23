import numpy as np
import matplotlib.pyplot as plt

def gauss_elim(a, b):
    n = b.size
    x = np.empty((n))
    
    for j in range(n - 1):
        
        # partial pivoting
        max_row = j
        for i in range(j + 1, n):
            if np.abs(a[i][j]) > np.abs(a[max_row][j]):
                max_row = i
        
        if max_row != j:
            a[[j, max_row]] = a[[max_row, j]]
            b[[j, max_row]] = b[[max_row, j]]
        
        #forward elimination
        for i in range(j + 1, n):
            factor = a[i][j] / a[j][j]
            
            a[i] -= a[j] * factor
            b[i] -= b[j] * factor
            
    #backward substitution
    for i in range(n - 1, -1, -1):
        x[i] = b[i]
        for j in range(n - 1, i, -1):
            x[i] -= a[i][j] * x[j]
        x[i] /= a[i][i]

    return x

file = open('data.txt')

x = np.array([])
y = np.array([])

for line in file:
    row = line.split()
    x = np.append(x, float(row[0]))
    y = np.append(y, float(row[1]))
    
file.close()

n = x.size

plt.figure(figsize = (20, 12))
plt.scatter(x, y, s = 5, color = 'black')

sum_x = np.sum(x)
sum_x2 = np.sum(x**2)
sum_x3 = np.sum(x**3)
sum_x4 = np.sum(x**4)
sum_x5 = np.sum(x**5)
sum_x6 = np.sum(x**6)

sum_y = np.sum(y)
sum_yx = np.sum(y * x)
sum_yx2 = np.sum(y * x**2)
sum_yx3 = np.sum(y * x**3)

y_avg = sum_y / n

#first order
a = np.array([[n, sum_x],
              [sum_x, sum_x2] ])
b = np.array([sum_y, sum_yx])

c0, c1 = gauss_elim(a, b)

print('first order curve:\n', 'a0 = %.2f' %c0, 'a1 = %.2f' %c1)

y1 = c0 + c1 * x
St = np.sum((y - y_avg) ** 2)
Sr = np.sum((y - y1) ** 2)

print(' r: %.2f' %np.sqrt((St - Sr) / St))

plt.plot(x, y1, label = '1st order')

#second order
a = np.array([[n, sum_x, sum_x2],
              [sum_x, sum_x2, sum_x3],
              [sum_x2, sum_x3, sum_x4]])
b = np.array([sum_y, sum_yx, sum_yx2])

c0, c1, c2 = gauss_elim(a, b)

print('second order curve:\n', 'a0 = %.2f' %c0, 'a1 = %.2f' %c1, 'a2 = %.2f' %c2)

y1 = c0 + c1 * x + c2 * (x**2)
St = np.sum((y - y_avg)**2)
Sr = np.sum((y - y1) ** 2)

print(' r: %.2f' %np.sqrt((St - Sr) / St))

plt.plot(x, y1, label = '2nd order')

#third order
a = np.array([[n, sum_x, sum_x2, sum_x3],
              [sum_x, sum_x2, sum_x3, sum_x4],
              [sum_x2, sum_x3, sum_x4, sum_x5],
              [sum_x3, sum_x4, sum_x5, sum_x6]])
b = np.array([sum_y, sum_yx, sum_yx2, sum_yx3])

c0, c1, c2, c3 = gauss_elim(a, b)

print('third order curve:\n', 'a0 = %.2f' %c0, 'a1 = %.2f' %c1, 'a2 = %.2f' %c2, 'a3 = %.2f' %c3)

y1 = c0 + c1 * x + c2 * (x**2) + c3 * (x**3)
St = np.sum((y - y_avg)**2)
Sr = np.sum((y - y1) ** 2)

print(' r: %.2f' %np.sqrt((St - Sr) / St))

plt.plot(x, y1, label = '3rd order')

plt.title('Regression Curve', fontsize = 20)
plt.legend(fontsize = 15)
plt.show()
