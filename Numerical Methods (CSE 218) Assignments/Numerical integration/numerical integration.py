import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

def trapezoid(y, i, h):
    
    return h * ( y[i] + y[i + 1] ) / 2

def one_third(y, i, h):
    
    return h * ( y[i] + 4 * y[i + 1] + y[i + 2] ) / 3

def three_eighth(y, i, h):
    
    return 3 * h * ( y[i] + 3 * y[i + 1] + 3 * y[i + 2] + y[i + 3]) / 8

file = open('input.txt')

n = int(file.readline())

x = np.empty(n)
y = np.empty(n)

for i in range(n):
    data = file.readline().split()
    x[i] = data[0]
    y[i] = data[1]

file.close()

plt.figure(figsize = (15, 9))

blue_patch = mpatches.Patch(color='blue', alpha = 0.3, label='Trapezoid')
red_patch = mpatches.Patch(color='red', alpha = 0.3, label='1/3 rule')
green_patch = mpatches.Patch(color='green', alpha = 0.3, label='3/8 rule')
plt.legend(handles=[blue_patch, red_patch, green_patch], loc = 'upper left')

plt.axhline(color = 'black')
plt.axvline(color = 'black')
plt.scatter(x, y, color = 'black')

trapezoid_count = 0
one_third_count = 0
three_eighth_count = 0
integral = 0

i = 0
while i < n - 1:
    count = 1
    h = x[i + 1] - x[i]
    j = i + 1
    
    while count < 5 and j < n - 1:
        if np.abs((x[j + 1] - x[j]) - h) < 1e-9:
            count += 1
            j += 1
        else:
            break
    
    if count == 1:
        integral += trapezoid(y, i, h)
        trapezoid_count += count
        color = 'blue'
        
    elif count == 2:
        integral += one_third(y, i, h)
        one_third_count += count
        color = 'red'
    
    elif count == 3:
        integral += three_eighth(y, i, h)
        three_eighth_count += count
        color = 'green'
        
    elif count == 4:
        integral += one_third(y, i, h)
        integral += one_third(y, i + 2, h)
        one_third_count += count
        color = 'red'
        
    else:
        integral += three_eighth(y, i, h)
        three_eighth_count += count - 2
        j -= 2
        color = 'green'
        
    for k in range(i, j):
        plt.fill_between(x, y, where = [(a >= x[k]) and (a <= x[k + 1]) for a in x], color = color, alpha = 0.3)
        
    i = j

print("Trapezoid: ", trapezoid_count, " interval(s)")
print("1/3 rule: ", one_third_count, " interval(s)")
print("3/8 rule: ", three_eighth_count, " interval(s)\n")
print("Integral: ", integral)

#%matplotlib qt
    
        
    