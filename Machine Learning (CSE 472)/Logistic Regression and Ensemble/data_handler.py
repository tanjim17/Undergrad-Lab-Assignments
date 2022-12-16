import numpy as np
import pandas as pd

def load_dataset():
    """
    function for reading data from csv
    and processing to return a 2D feature matrix and a vector of class
    :return:
    """
    data = pd.read_csv('data_banknote_authentication.csv')
    X = data.iloc[:, :-1].values
    y = data.iloc[:, -1].values
    return X, y

def split_dataset(X, y, test_size, shuffle):
    """
    function for spliting dataset into train and test
    :param X:
    :param y:
    :param float test_size: the proportion of the dataset to include in the test split
    :param bool shuffle: whether to shuffle the data before splitting
    :return:
    """
    assert X.shape[0] == y.shape[0]

    if shuffle:
        idx = np.random.permutation(X.shape[0])
        X, y = X[idx], y[idx]

    n_train = int(X.shape[0] * (1 - test_size))
    X_train, y_train = X[:n_train], y[:n_train]
    X_test, y_test = X[n_train:], y[n_train:]
    return X_train, y_train, X_test, y_test


def bagging_sampler(X, y):
    """
    Randomly sample with replacement
    Size of sample will be same as input data
    :param X:
    :param y:
    :return:
    """
    assert X.shape[0] == y.shape[0]
    m = X.shape[0]

    X_sample , y_sample = np.empty((0, X.shape[1])), np.empty((0,))

    for i in range(m):
        i = np.random.randint(0, m)
        X_sample = np.append(X_sample, [X[i]], axis=0)
        y_sample = np.append(y_sample, y[i])

    return X_sample, y_sample