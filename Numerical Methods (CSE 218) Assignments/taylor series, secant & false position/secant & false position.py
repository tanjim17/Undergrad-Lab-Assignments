import numpy as np
from matplotlib import pyplot as plt

MAX_i = 1000
Es = 0.5

def f(x):
     return x / (1-x) * np.sqrt(6 / (2 + x)) - 0.05
   
def secant(f, x0, x1, es, max_i):
    
    #1st iteration
    x2 = x1 - f(x1) * (x1 - x0) / (f(x1) - f(x0))
    
    x0 = x1
    x1 = x2
    
    ea = es + 1
    
    #loop starts from 2nd iteration
    for i in range(2, max_i):
        x2 = x1 - f(x1) * (x1 - x0) / (f(x1) - f(x0))
        
        if x2 != 0:
            ea = np.abs((x2 - x1) / x2) * 100
        
        if ea < es:
            return x2, i
        
        else:
            x0 = x1
            x1 = x2
    
    return x2, i

def false_position(f, xl, xu, es, max_i):
    
    # 1st iteration
    xr = xu - f(xu) * (xl - xu) / (f(xl) - f(xu))
    
    if f(xl) * f(xr) < 0:
        xu = xr
        xro = xr
            
    elif f(xl) * f(xr) > 0:
        xl = xr
        xro = xr
    
    else:
        return xr, 1
    
    ea = es + 1
    
    #loop starts from 2nd iteration
    for i in range(2, max_i):
        xr = xu - f(xu) * (xl - xu) / (f(xl) - f(xu))
        
        if xr != 0:
            ea = np.abs((xr - xro) / xr) * 100
         
        if ea < es:
            return xr, i
        
        if f(xl) * f(xr) < 0:
            xu = xr
            xro = xr
            
        elif f(xl) * f(xr) > 0:
            xl = xr
            xro = xr
            
        else:
            return xr, i   # ea becomes zero
    
    return xr, i
    
plt.figure(figsize = (20,12))
plt.axhline(0, color = 'black')
plt.axvline(0, color = 'black')
plt.grid()

x = np.arange(-0.5, 0.5, 0.001)
plt.plot(x, f(x))
plt.show()

while True:
    xl = float(input('Enter lower bound: '))
    xu = float(input('Enter upper bound: '))
    
    if f(xl) * f(xu) < 0:
        break
    else:
        print('f(xl)*f(xu) should be less than 0. try different inputs')

x0 = float(input('Enter first initial guess: '))
x1 = float(input('Enter second initial guess: '))
    
result = false_position(f, xl, xu, Es, MAX_i)

print('using false position:') 
print('x = ', result[0], ', number of iteration = ', result[1])

result = secant(f, x0, x1, Es, MAX_i)

print('using secant method:') 
print('x = ', result[0], ', number of iteration = ', result[1])


























