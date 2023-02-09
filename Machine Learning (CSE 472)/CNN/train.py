import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.metrics import log_loss, accuracy_score, f1_score, confusion_matrix
from tqdm import tqdm
import math
import cv2

IMG_SIZE = 32
nCLASSES = 10

batch_size = 32
nEpochs = 10
lr = 0.001

def load_images(filenames, dirname):
    print('\nLoading images...')
    images = np.zeros((len(filenames), 3, IMG_SIZE, IMG_SIZE))
    for i in tqdm(range(len(filenames))):
        image = cv2.imread(dirname + '/' + filenames[i])
        image = np.moveaxis(image, -1, 0)
        images[i] = image
    images = images / 255 # normalize
    return images

def getWindows(input, output_size, kernel_size, padding=0, stride=1, dilate=0):
    working_input = input
    working_pad = padding
    # dilate the input if necessary
    if dilate != 0:
        working_input = np.insert(working_input, range(1, input.shape[2]), 0, axis=2)
        working_input = np.insert(working_input, range(1, input.shape[3]), 0, axis=3)

    # pad the input if necessary
    if working_pad != 0:
        working_input = np.pad(working_input, pad_width=((0,), (0,), (working_pad,), (working_pad,)), mode='constant', constant_values=(0.,))

    _, _, out_h, out_w = output_size
    out_b, out_c, _, _ = input.shape

    batch_str, channel_str, kern_h_str, kern_w_str = working_input.strides

    return np.lib.stride_tricks.as_strided(
        working_input,
        (out_b, out_c, out_h, out_w, kernel_size, kernel_size),
        (batch_str, channel_str, stride * kern_h_str, stride * kern_w_str, kern_h_str, kern_w_str)
    )

class ConvolutionLayer:
    def __init__(self, in_channels, out_channels, kernel_size=3, stride=1, padding=0):
        self.in_channels = in_channels
        self.out_channels = out_channels
        self.kernel_size = kernel_size
        self.stride = stride
        self.padding = padding
        self.cache = None
        self.weights = np.random.randn(self.out_channels, self.in_channels, self.kernel_size, self.kernel_size) * math.sqrt(2 / (self.in_channels * self.kernel_size * self.kernel_size))
        self.biases = np.zeros(self.out_channels)

    def forward(self, x):
        n, c, h, w = x.shape
        out_h = (h - self.kernel_size + 2 * self.padding) // self.stride + 1
        out_w = (w - self.kernel_size + 2 * self.padding) // self.stride + 1

        windows = getWindows(x, (n, c, out_h, out_w), self.kernel_size, self.padding, self.stride)

        out = np.einsum('bihwkl,oikl->bohw', windows, self.weights)
        # add biases to kernels
        out += self.biases[None, :, None, None]

        self.cache = x, windows
        return out

    def backward(self, grad_output):
        x, windows = self.cache

        padding = self.kernel_size - 1 if self.padding == 0 else self.padding

        grad_output_windows = getWindows(grad_output, x.shape, self.kernel_size, padding=padding, stride=1, dilate=self.stride - 1)
        rot_kern = np.rot90(self.weights, 2, axes=(2, 3))

        grad_biases = np.sum(grad_output, axis=(0, 2, 3))
        grad_weights = np.einsum('bihwkl,bohw->oikl', windows, grad_output)
        grad_input = np.einsum('bohwkl,oikl->bihw', grad_output_windows, rot_kern)

        self.weights -= lr * grad_weights
        self.biases -= lr * grad_biases

        return grad_input

class MaxPoolingLayer:
    def __init__(self, pool_size, stride=2):
        self.pool_size = pool_size
        self.stride = stride

    def forward(self, input):
        self.input = input
        (batch_size, in_channels, height, width) = input.shape
        output_height = int((height - self.pool_size) / self.stride + 1)
        output_width = int((width - self.pool_size) / self.stride + 1)
        output = np.zeros((batch_size, in_channels, output_height, output_width))
        for i in range(batch_size):
            for j in range(in_channels):
                for k in range(output_height):
                    for l in range(output_width):
                        x_start = k * self.stride
                        y_start = l * self.stride
                        x_end = x_start + self.pool_size
                        y_end = y_start + self.pool_size
                        output[i, j, k, l] = np.max(input[i, j, x_start:x_end, y_start:y_end])
        return output

    def backward(self, grad_output):
        (batch_size, in_channels, height, width) = grad_output.shape
        grad_input = np.zeros_like(self.input)
        for i in range(batch_size):
            for j in range(in_channels):
                for k in range(height):
                    for l in range(width):
                        x_start = k * self.stride
                        y_start = l * self.stride
                        x_end = x_start + self.pool_size
                        y_end = y_start + self.pool_size
                        pool = self.input[i, j, x_start:x_end, y_start:y_end]
                        max_index = np.argmax(pool)
                        max_index = np.unravel_index(max_index, pool.shape)
                        grad_input[i, j, x_start:x_end, y_start:y_end][max_index] = grad_output[i, j, k, l]
        return grad_input

class FlatteningLayer:
    def forward(self, input):
        self.input_shape = input.shape
        return input.reshape((input.shape[0], -1))

    def backward(self, grad_output):
        return grad_output.reshape(self.input_shape)

class FullyConnectedLayer:
    def __init__(self, in_features, out_features):
        self.weights = np.random.randn(in_features, out_features) * math.sqrt(2 / in_features)
        self.biases = np.zeros((1, out_features))

    def forward(self, input):
        self.input = input
        return np.dot(input, self.weights) + self.biases

    def backward(self, grad_output):
        grad_input = np.dot(grad_output, self.weights.T)
        grad_weights = np.dot(self.input.T, grad_output)
        grad_biases = np.sum(grad_output, axis=0, keepdims=True)
        
        self.weights -= lr * grad_weights
        self.biases -= lr * grad_biases
        
        return grad_input

class ReLULayer:
    def forward(self, input):
        self.input = input
        return np.maximum(0, input)

    def backward(self, grad_output):
        grad_input = grad_output.copy()
        grad_input[self.input < 0] = 0
        return grad_input

class SoftmaxLayer:
    def forward(self, u):
        v = np.exp(u)
        v = v / np.sum(v, axis=1, keepdims=True)
        return v

    def backward(self, del_v):
        del_u = np.copy(del_v)
        return del_u

class LeNet:
    def __init__(self):
        self.model_components = []

        self.model_components.append(ConvolutionLayer(in_channels=3, out_channels=6, kernel_size=5, stride=1, padding=0))
        self.model_components.append(ReLULayer())
        self.model_components.append(MaxPoolingLayer(pool_size=2, stride=2))

        self.model_components.append(ConvolutionLayer(in_channels=6, out_channels=16, kernel_size=5, stride=1, padding=0))
        self.model_components.append(ReLULayer())
        self.model_components.append(MaxPoolingLayer(pool_size=2, stride=2))

        self.model_components.append(FlatteningLayer())
        self.model_components.append(FullyConnectedLayer(in_features=16 * 5 * 5, out_features=120))
        self.model_components.append(ReLULayer())
        self.model_components.append(FullyConnectedLayer(in_features=120, out_features=84))
        self.model_components.append(ReLULayer())
        self.model_components.append(FullyConnectedLayer(in_features=84, out_features=10))
        self.model_components.append(SoftmaxLayer())
    
    def train(self, u, y_true):
        for i in range(len(self.model_components)):
            u = self.model_components[i].forward(u)
            # print(i, u.shape)
        
        del_v = u - y_true
        
        for i in range(len(self.model_components) - 1, -1, -1):
            del_v = self.model_components[i].backward(del_v)
            # print(i, del_v.shape)

    
    def predict(self, u):
        for i in range(len(self.model_components)):
            u = self.model_components[i].forward(u)
        return u
    
    def save_model(self):
        pass
        # todo

def main():
    # Load csv
    train_val_df = pd.read_csv('temp1.csv')
    # print(train_val_data.head())

    # extract filenames and labels
    filenames = train_val_df['filename'].values
    labels = train_val_df['digit'].values

    # split data into train and validation sets
    train_filenames, val_filenames, y_train, y_val = train_test_split(filenames, labels, test_size=0.2)
    
    #load train and validation data
    x_train = load_images(train_filenames, dirname='train_val')
    x_val = load_images(val_filenames, dirname='train_val')
    # print(x_train.shape, x_val.shape)

    # initialize model
    model = LeNet()

    nBatches = math.ceil(y_train.shape[0] / batch_size)
    min_macro_f1_score = math.inf

    print('\nTraining...')
    for epoch in tqdm(range(nEpochs)):
        # training
        for batch in tqdm(range(nBatches)):
            curr_batch_size = y_train.shape[0] - batch * batch_size if (batch + 1) * batch_size > y_train.shape[0] else batch_size
            y_true = np.zeros((curr_batch_size, nCLASSES))
            for i in range(y_true.shape[0]):
                y_true[i, y_train[batch * batch_size + i]] = 1  # generating one-hot encoding of y_train
            model.train(x_train[batch * batch_size: batch * batch_size + curr_batch_size], y_true)
        
        # validation
        y_true = np.zeros((y_val.shape[0], nCLASSES))
        for i in range(y_true.shape[0]):
            y_true[i, y_val[i]] = 1  # generating one-hot encoding of y_validation
        y_predicted = model.predict(x_val)
        
        # calculate performance scores
        val_loss = log_loss(y_true, y_predicted)
        y_predicted_labels = np.argmax(y_predicted, axis=1)
        cm = confusion_matrix(y_val, y_predicted_labels)
        accuracy = accuracy_score(y_val, y_predicted_labels)
        macro_f1_score = f1_score(y_val, y_predicted_labels, average='macro')
        
        if macro_f1_score < min_macro_f1_score:
            min_macro_f1_score = macro_f1_score
        
        print(f'Epoch: {epoch + 1}, Validation Loss: {val_loss}, Accuracy: {accuracy}, Macro F1 Score: {macro_f1_score}')
        print(f'Confusion Matrix:\n{cm}')

if __name__ == '__main__':
    main()