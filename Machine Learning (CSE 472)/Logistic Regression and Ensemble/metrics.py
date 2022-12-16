"""
Refer to: https://en.wikipedia.org/wiki/Precision_and_recall#Definition_(classification_context)
"""


def accuracy(y_true, y_pred):
    """

    :param y_true:
    :param y_pred:
    :return:
    """
    count = 0
    for i in range(len(y_true)):
        if y_true[i] == y_pred[i]:
            count += 1
    return count / len(y_true)
    

def precision_score(y_true, y_pred):
    """

    :param y_true:
    :param y_pred:
    :return:
    """
    tp = 0
    fp = 0
    for i in range(len(y_true)):
        if y_true[i] == 1 and y_pred[i] == 1:
            tp += 1
        elif y_true[i] == 0 and y_pred[i] == 1:
            fp += 1
    return tp / (tp + fp)

def recall_score(y_true, y_pred):
    """

    :param y_true:
    :param y_pred:
    :return:
    """
    tp = 0
    fn = 0
    for i in range(len(y_true)):
        if y_true[i] == 1 and y_pred[i] == 1:
            tp += 1
        elif y_true[i] == 1 and y_pred[i] == 0:
            fn += 1
    return tp / (tp + fn)


def f1_score(y_true, y_pred):
    """

    :param y_true:
    :param y_pred:
    :return:
    """
    precision = precision_score(y_true, y_pred)
    recall = recall_score(y_true, y_pred)
    return 2 * (precision * recall) / (precision + recall)
