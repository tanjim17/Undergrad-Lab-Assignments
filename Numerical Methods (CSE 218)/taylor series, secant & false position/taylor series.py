import numpy as np
import matplotlib.pyplot as plt

def approx_ln_of_one_plus_x(x, n):
    
    sum = 0.0
    
    for i in range(1, n+1):
        sum += -(-1)**i * x**i / i
    
    return sum    

#plotting ln(1+x)
plt.figure(figsize = (20,12))
x = np.arange(-1, 1, 0.1)
y = np.log(1+x)
plt.plot(x, y, color = "red")

#plotting approx ln(1+x) for iteration = 1,3,5,20,50
y = approx_ln_of_one_plus_x(x, 1)
plt.plot(x, y, color = "blue")

y = approx_ln_of_one_plus_x(x, 3)
plt.plot(x, y, color = "green")

y = approx_ln_of_one_plus_x(x, 5)
plt.plot(x, y, color = "orange")

y = approx_ln_of_one_plus_x(x, 20)
plt.plot(x, y, color = "black")

y = approx_ln_of_one_plus_x(x, 50)
plt.plot(x, y, color = "violet")

#plotting error vs iteration
plt.figure(figsize = (20,12))
x = np.arange(2, 51)
y = np.array([])

sum = 0.5

for i in x:
    term = -(-1)**i * 0.5**i / i
    sum += term
    y = np.append(y, np.abs(term / sum * 100))
  
plt.plot(x, y)


