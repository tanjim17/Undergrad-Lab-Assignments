import sys
import random
from tabulate import tabulate

if __name__ == '__main__':
    n = int(sys.argv[1]) # height of nuclear fission tree
    nFissions_max = int(sys.argv[2]) # max number of fissions
    max_itr = int(sys.argv[3])

    # calculate fission probabilities
    p = [0] * (nFissions_max + 1)
    p[0] = 0.4825
    for i in range(1, nFissions_max + 1):
        p[i] = 0.2126 * (0.5893 ** (i -1))

    # adjust probabilities so that they sum to 1
    if sum(p) != 1:
        p[0] += 1 - sum(p)

    # find cumulative probability
    for i in range(1, nFissions_max + 1):
        p[i] += p[i - 1]

    # frequency of node count in nth level
    nNodes_freqs = [0] * (nFissions_max + 2)

    # simulate nuclear fission
    for _ in range(max_itr):
        nNodes_prev = 1
        for i in range(n):
            nNodes = 0
            for j in range(nNodes_prev):
                u = random.random()
                for k in range(nFissions_max + 1):
                    if u < p[k]:
                        nNodes += k
                        break
            nNodes_prev = nNodes
        if nNodes_prev <= nFissions_max + 1:
            nNodes_freqs[nNodes] += 1
            
    # print results
    data = []
    for i in range(nFissions_max + 2):
        data.append([i, nNodes_freqs[i] / max_itr])
    print('Tree height =', n)
    print('Max number of fissions =', nFissions_max)
    print('Number of iterations =', max_itr, '\n\n')
    print(f'In the last level:\n')
    print (tabulate(data, headers=['Number of Fissions', 'Probability'], numalign='center'))
