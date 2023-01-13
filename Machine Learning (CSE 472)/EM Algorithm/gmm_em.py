import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import multivariate_normal

class GMM_EM:
    def __init__(self, X, K, itr_count):
        self.X = X
        self.K = K
        self.itr_count = itr_count
        self.N, self.D = X.shape

    def init_parameters(self):
        self.mu = np.random.rand(self.K, self.D)
        self.sigma = np.array([np.eye(self.D) for _ in range(self.K)])
        self.prior = np.ones(self.K) / self.K
        self.posterior = np.zeros((self.N, self.K))

    def e_step(self):
        for i in range(self.N):
            denominator = 0
            for k in range(self.K):
                self.posterior[i][k] = self.prior[k] * multivariate_normal.pdf(self.X[i], mean=self.mu[k], cov=self.sigma[k], allow_singular=True)
                denominator += self.posterior[i][k]
            self.posterior[i] /= denominator

    def m_step(self):
        for k in range(self.K):
            numerator = np.zeros(self.D)
            denominator = 0
            for i in range(self.N):
                numerator += self.posterior[i][k] * self.X[i]
                denominator += self.posterior[i][k]
            self.mu[k] = numerator / denominator

            numerator = np.zeros((self.D, self.D))
            for i in range(self.N):
                x_mu = self.X[i] - self.mu[k]
                numerator += self.posterior[i][k] * np.outer(x_mu, x_mu)
            self.sigma[k] = numerator / denominator

            self.prior[k] = denominator / self.N

    def plot(self, itr_count):
        plt.scatter(self.X[:, 0], self.X[:, 1], s=5)
        x, y = np.mgrid[min(self.X[:, 0]):max(self.X[:, 0]):.01, min(self.X[:, 1]):max(self.X[:, 1]):.01]
        grid = np.c_[x.ravel(), y.ravel()]
        for k in range(self.K):
            plt.contour(x, y, multivariate_normal.pdf(grid, mean=self.mu[k], cov=self.sigma[k], allow_singular=True).reshape(x.shape))
        plt.title(f"GMM EM (Iteration: {itr_count})")
        plt.show(block=False)
        plt.pause(0.5)
        if itr_count == self.itr_count - 1:
            plt.savefig("gmm.png")
        plt.clf()

    def train(self, isPlot=False):
        if isPlot and self.D != 2:
            sys.exit("Can plot 2D data only!")
        self.init_parameters()
        for itr in range(self.itr_count):
            self.e_step()
            self.m_step()
            if isPlot:
                self.plot(itr)
        # print("Mu:", self.mu)
        # print("Sigma:", self.sigma)
        # print("Prior:", self.prior)
        # print("Posterior:", self.posterior)

    def get_log_likelihood(self):
        log_likelihood = 0
        for i in range(self.N):
            temp = 0
            for k in range(self.K):
                temp += self.prior[k] * multivariate_normal.pdf(self.X[i], mean=self.mu[k], cov=self.sigma[k], allow_singular=True)
            log_likelihood += np.log(temp)
        return log_likelihood