from data_handler import bagging_sampler
import numpy as np
import copy

class BaggingClassifier:
    def __init__(self, base_estimator, n_estimator):
        """
        :param base_estimator:
        :param n_estimator:
        :return:
        """
        self.base_estimator = base_estimator
        self.n_estimator = n_estimator

    def fit(self, X, y):
        """
        :param X:
        :param y:
        :return: self
        """
        assert X.shape[0] == y.shape[0]
        assert len(X.shape) == 2
        assert len(y.shape) == 1

        estimators = []
        for i in range(self.n_estimator):
            print("Training estimator", i, "...")
            Xb, yb = bagging_sampler(X, y)
            self.base_estimator.fit(Xb, yb)
            estimators.append(copy.deepcopy(self.base_estimator))
        self.estimators = estimators

    def predict(self, X):
        """
        function for predicting labels of for all datapoint in X
        apply majority voting
        :param X:
        :return:
        """
        assert len(X.shape) == 2
        
        for i in range(self.n_estimator):
            # for i in range(self.n_estimator):
            #     print(self.estimators[i].w)
            y_pred = self.estimators[i].predict(X)
            if i == 0:
                y_pred_sum = y_pred
            else:
                y_pred_sum += y_pred

        y_pred = np.where(y_pred_sum > self.n_estimator / 2, 1, 0)
        return y_pred
