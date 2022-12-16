import numpy as np

class LogisticRegression:
    def __init__(self, params):
        """
        figure out necessary params to take as input
        :param params:
        """
        self.params = params

    def sigmoid(self, z):
        return 1.0 / (1 + np.exp(-z))

    def gradient(self, X, y, y_pred):
        m = X.shape[0]
        dw = (1 / m) * np.dot(X.T, (y_pred - y))
        db = (1 / m) * np.sum((y_pred - y))
        return dw, db

    def fit(self, X, y):
        """
        :param X:
        :param y:
        :return: self
        """
        assert X.shape[0] == y.shape[0]
        assert len(X.shape) == 2
        assert len(y.shape) == 1

        m, n = X.shape
        y = y.reshape(m, 1)
        w = np.zeros((n,1))
        b = 0
    
        for itr in range(self.params['n_iter']):
            for i in range((m - 1) // self.params['batch_size'] + 1):
                start_i = i * self.params['batch_size']
                end_i = start_i + self.params['batch_size']
                Xb = X[start_i : end_i]
                yb = y[start_i : end_i]
                
                y_pred = self.sigmoid(np.dot(Xb, w) + b)
                dw, db = self.gradient(Xb, yb, y_pred)
                w -= self.params['learning_rate'] * dw
                b -= self.params['learning_rate'] * db
            
        self.w = w
        self.b = b

    def predict(self, X):
        """
        function for predicting labels of for all datapoint in X
        :param X:
        :return:
        """
        assert len(X.shape) == 2
        m = X.shape[0]

        y_pred = np.zeros((m, 1))
        for i in range(m):
            y_pred[i] = 1 if self.sigmoid(np.dot(X[i], self.w) + self.b) > 0.5 else 0
        return y_pred
