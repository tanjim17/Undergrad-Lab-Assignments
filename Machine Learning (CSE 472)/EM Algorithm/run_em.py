import sys
import numpy as np
import matplotlib.pyplot as plt
from gmm_em import GMM_EM

K_MIN = 1
K_MAX = 10

if __name__ == '__main__':
    argc = len(sys.argv)
    if argc != 3 and argc != 4:
        sys.exit("Argument count mismatch!")
    filepath = sys.argv[1]
    itr_count = int(sys.argv[2])

    data = np.genfromtxt(filepath, delimiter=' ')
    # print(data)
    # print(data.shape)

    if argc == 3:
        log_likelihoods = []
        for k in range(K_MIN, K_MAX + 1):
            print(f"Training for {k} cluster{'s' if k > 1 else ''}...")
            gmm = GMM_EM(data, K=k, itr_count=itr_count)
            gmm.train()
            log_likelihoods.append(gmm.get_log_likelihood())

        # plot log likelihood vs no. of clusters
        plt.plot([*range(K_MIN, K_MAX + 1)], log_likelihoods)
        plt.xlabel('No. of clusters')
        plt.ylabel('Log likelihood')
        plt.title(f'Log likelihood vs No. of clusters (Dimension = {data.shape[1]})')
        plt.grid()
        plt.show()
    else:
        k = int(sys.argv[3])
        gmm = GMM_EM(data, K=k, itr_count=itr_count)
        gmm.train(isPlot=True)