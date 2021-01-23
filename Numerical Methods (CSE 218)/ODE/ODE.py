import numpy as np
import matplotlib.pyplot as plt

def f(x, y):
    return (x + 20*y) * np.sin(x*y)

def euler_method(f, h, x0, y0, x_end):
    
    x = np.arange(x0, x_end + h, h)
    y = np.empty(x.size)
    y[0] = y0
    
    for i in range(x.size - 1):      
        y[i+1] = y[i] + f(x[i], y[i]) * h
        
    return x, y

def RK_method_2nd_order(f, h, x0, y0, x_end, a2):

    x = np.arange(x0, x_end + h, h)
    y = np.empty(x.size)
    y[0] = y0
    
    a1 = 1 - a2
    p1 = 1 / 2 / a2
    q11 = 1 / 2 / a2
    
    for i in range(x.size - 1):      
        k1 = f(x[i], y[i])
        k2 = f(x[i] + p1*h, y[i] + q11*k1*h)
        
        y[i+1] = y[i] + (a1*k1 + a2*k2) * h
        
    return x, y

def RK_method_4th_order(f, h, x0, y0, x_end):

    x = np.arange(x0, x_end + h, h)
    y = np.empty(x.size)
    y[0] = y0
    
    for i in range(x.size - 1):      
        k1 = f(x[i], y[i])
        k2 = f(x[i] + 0.5*h, y[i] + 0.5*k1*h)
        k3 = f(x[i] + 0.5*h, y[i] + 0.5*k2*h)
        k4 = f(x[i] + h, y[i] + k3*h)
        
        y[i+1] = y[i] + (k1 + 2*k2 + 2*k3 + k4) * h * (1/6)
        
    return x, y

no_of_methods = 5
no_of_steps = 4
x0 = 0
x_end = 10
y0 = 4
h = np.array([0.01, 0.05, 0.1, 0.5])

fig1, ax1 = plt.subplots(no_of_methods, figsize = (15, 70))
fig2, ax2 = plt.subplots(no_of_steps, figsize = (15, 60))

step_color = np.array(['blue', 'green', 'red', 'orange'])
step_label = np.array(['0.01', '0.05', '0.1', '0.5'])
method_color = np.array(['blue', 'green', 'red', 'orange', 'purple'])
method_label = np.array(['Euler', 'Heun', 'Midpoint', 'Ralston', 'RK_4th'])

for i in range(no_of_methods):
    ax1[i].axhline(color = 'black')
    ax1[i].axvline(color = 'black')
    ax1[i].set_title(method_label[i], fontsize = 20)
    
for i in range(no_of_steps):
    ax2[i].axhline(color = 'black')
    ax2[i].axvline(color = 'black')
    ax2[i].set_title(step_label[i], fontsize = 20)

X = np.empty(no_of_methods, np.ndarray)
Y = np.empty(no_of_methods, np.ndarray)

for i in range(no_of_steps):
    
    X[0], Y[0] = euler_method(f, h[i], x0, y0, x_end)
    X[1], Y[1] = RK_method_2nd_order(f, h[i], x0, y0, x_end, 0.5) #Heun
    X[2], Y[2] = RK_method_2nd_order(f, h[i], x0, y0, x_end, 1)  #Midpoint
    X[3], Y[3] = RK_method_2nd_order(f, h[i], x0, y0, x_end, 2/3) #Ralston
    X[4], Y[4] = RK_method_4th_order(f, h[i], x0, y0, x_end)
        
    for j in range(no_of_methods):
        ax1[j].plot(X[j], Y[j], color = step_color[i], label = step_label[i])  #same method
        ax2[i].plot(X[j], Y[j], color = method_color[j], label = method_label[j])  #same step
        
for i in range(no_of_methods):
    ax1[i].legend(fontsize = 15)
    
for i in range(no_of_steps):
    ax2[i].legend(fontsize = 15)

        
    
    
