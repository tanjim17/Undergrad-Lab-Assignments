from train import LeNet, load_images, resize_images, nCLASSES
import numpy as np
import pandas as pd
import sys
from sklearn.metrics import log_loss, accuracy_score, f1_score, confusion_matrix

lr = 0.001

def main():
    testdir = sys.argv[1]
    testdir = resize_images(testdir)

    # Load csv
    test_val_df = pd.read_csv('test.csv')

    # extract filenames and labels
    filenames = test_val_df['filename'].values
    y_test = test_val_df['digit'].values
 
    # load data
    x_test = load_images(filenames, dirname=testdir)

    # Load the model
    model = LeNet(lr=lr)
    model.set_weights('params.pkl')

    # Predict
    y_true = np.zeros((y_test.shape[0], nCLASSES))
    for i in range(y_true.shape[0]):
        y_true[i, y_test[i]] = 1  # generating one-hot encoding of y_test
    y_predicted = model.predict(x_test)
    y_predicted_labels = np.argmax(y_predicted, axis=1)

    # Save predictions in csv file
    df = pd.DataFrame({'FileName': filenames, 'Digit': y_predicted_labels})
    df.to_csv('predictions.csv', index=False)

    # calculate performance scores
    loss = log_loss(y_true, y_predicted)
    cm = confusion_matrix(y_test, y_predicted_labels)
    accuracy = accuracy_score(y_test, y_predicted_labels)
    macro_f1_score = f1_score(y_test, y_predicted_labels, average='macro')
        
    print(f'\nLoss: {loss}, Accuracy: {accuracy}, Macro F1 Score: {macro_f1_score}')
    print(f'Confusion Matrix:\n{cm}\n')

    # write performance scores to file
    with open(f'test_performance.txt', 'w') as f:
        f.write(f'Loss: {loss}, Accuracy: {accuracy}, Macro F1 Score: {macro_f1_score}\n')
        f.write(f'Confusion Matrix:\n{cm}\n\n')

if __name__ == '__main__':
    main()