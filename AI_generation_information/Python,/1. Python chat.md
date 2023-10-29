# Python implementation

## USER
Can you write a python bare metal implementation of this CNN:
```python
model = models.Sequential()
model.add(layers.Conv2D(4, (3, 3), activation='relu', input_shape=(28, 28, 1)))
model.add(layers.MaxPooling2D((4, 4)))
model.add(layers.Conv2D(8, (3, 3), activation='relu'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Flatten())
model.add(layers.Dense(10, dtype='float16'))  # Ensure that the final layer has float32 dtype
```

## ASSISTANT
A "bare metal" implementation typically refers to a very low-level coding approach, often in a language like C or assembly and with direct control over hardware resources. However, developing a deep learning model like a CNN from scratch on such a low level is complex, time-consuming, and generally unnecessary given the range of high-level libraries available that are optimized for such tasks.

However, if you're interested in understanding the details of a convolutional neural network, you can see a basic implementation in Python with NumPy. This won't include all the features of the Keras model, but it will show the basic principles.

For simplicity's sake, I'll show you how to implement a forward pass of a single convolutional layer:

```python
import numpy as np

def conv2d(X, W):
    '''
    A simple implementation of 2D convolution.

    Parameters:
    X: Input tensor of shape (height, width, channels)
    W: Filter tensor of shape (filter_height, filter_width, channels)

    Returns:
    conv: Convolved tensor
    '''
    h, w = X.shape[0] - W.shape[0] + 1, X.shape[1] - W.shape[1] + 1  # calculate dimensions of the output tensor
    conv = np.zeros((h, w))
    
    for i in range(h):
        for j in range(w):
            conv[i, j] = np.sum(X[i:i+W.shape[0], j:j+W.shape[1]] * W)
    
    return conv

# Testing our Conv2D function
X = np.random.rand(5, 5, 1)  # a 5x5 grayscale image
W = np.random.rand(3, 3, 1)  # a 3x3 filter

out = conv2d(X, W)
```

This is a very basic implementation and does not include strides, padding, multiple filters, or multiple input channels, but it's a start.

You might also notice there's no activation function applied. Most convolutional layers in practice apply an activation function to the output of the convolution. In the case of your Keras model, that would be a ReLU activation function. This can be implemented in NumPy as follows:
```python
def relu(X):
    return np.maximum(0, X)  # ReLU function

out = relu(conv2d(X, W))  # Apply ReLU activation after convolution
```

## USER
Can you write a bare metal implementation of the CNN, just the forward function, assuming that the CNN was trained previously?

## ASSISTANT
Sure, I can show you a simple Python implementation of this specific model using NumPy for the computations. Note, this is a "bare-bones" implementation, with the understanding that all layers use valid padding and that max-pooling layers use non-overlapping 2x2 patches.

This implementation also assumes you've somehow previously trained this model and now have the kernel weights and biases for each layer. Here's how you might do the forward pass:
```python
import numpy as np

# define activation functions
def relu(x):
    return np.maximum(0, x)

def softmax(x):
    exps = np.exp(x - np.max(x, axis=-1, keepdims=True))
    return exps / np.sum(exps, axis=-1, keepdims=True)

# define forward pass functions
def conv2d_forward(X, W, b):
    # X - input, W - weights, b - bias
    h_filter, w_filter, d_filter, n_filters = W.shape
    h, w, d = X.shape
    out = np.zeros((h - h_filter + 1, w - w_filter + 1, n_filters))
    for i in range(h - h_filter + 1):
        for j in range(w - w_filter + 1):
            for k in range(n_filters):
                out[i, j, k] = np.sum(X[i:i+h_filter, j:j+w_filter] * W[:, :, :, k]) + b[k]
    return out

def maxpool2d_forward(X, pool_size):
    h, w, d = X.shape
    h_out = h // pool_size
    w_out = w // pool_size
    out = np.zeros((h_out, w_out, d))
    for i in range(h_out):
        for j in range(w_out):
            for k in range(d):
                out[i, j, k] = np.max(X[i*pool_size:(i+1)*pool_size, j*pool_size:(j+1)*pool_size, k])
    return out

def flatten(X):
    return X.flatten()

def dense_forward(X, W, b):
    return np.dot(W.T, X) + b

# assuming weights and biases have been trained and are provided
W_conv1, b_conv1, W_conv2, b_conv2, W_conv3, b_conv3, W_dense1, b_dense1, W_dense2, b_dense2 = None # Replace None with your actual trained weights and biases

# the forward pass for the specific model
def forward(X):
    out = conv2d_forward(X, W_conv1, b_conv1)
    out = relu(out)
    out = maxpool2d_forward(out, 4)
    out = conv2d_forward(out, W_conv2, b_conv2)
    out = relu(out)
    out = maxpool2d_forward(out, 2)
    out = relu(out)
    out = dense_forward(out, W_dense2, b_dense2)
    out = softmax(out)
    return out
```
The forward function now performs the forward pass for your specific model. Note that in practice, you'd often apply batch normalization and dropout after the convolutional layers, before the activation function, which we've omitted here for simplicity.

## USER
How can I get the Bias and weights based on the CNN implementation from a numpy file:

## ASSISTANT
To load the weights and biases from these .npy files, you can use NumPy's load() function:
```python
# Load weights and biases from .npy files
weights_conv1 = np.load('weights_conv1.npy')
biases_conv1 = np.load('biases_conv1.npy')
weights_conv2 = np.load('weights_conv2.npy')
biases_conv2 = np.load('biases_conv2.npy')
weights_conv3 = np.load('weights_conv3.npy')
biases_conv3 = np.load('biases_conv3.npy')
weights_dense1 = np.load('weights_dense1.npy')
biases_dense1 = np.load('biases_dense1.npy')
weights_dense2 = np.load('weights_dense2.npy')
biases_dense2 = np.load('biases_dense2.npy')
```
## USER
And how can I call the function forward based on the previous weights and biases?

## ASSISTANT
To use the previously mentioned forward function with the weights and biases obtained from your trained CNN model, you first need to load these parameters and pass them to the function. The forward function needs to be slightly modified to take these parameters as arguments. Here's how you might do this:
```python
# Assuming weights and biases have been loaded as mentioned before
# Modify the forward function to take these parameters as arguments

def forward(X, W_conv1, b_conv1, W_conv2, b_conv2, W_dense2, b_dense2):
    out = conv2d_forward(X, W_conv1, b_conv1)
    out = relu(out)
    out = maxpool2d_forward(out, 4)
    out = conv2d_forward(out, W_conv2, b_conv2)
    out = relu(out)
    out = maxpool2d_forward(out, 2)
    out = flatten(out)
    out = dense_forward(out, W_dense2, b_dense2)
    out = softmax(out)
    return out

# Call the function with a single image from the test dataset and the weights and biases
single_image = test_images[0]  # assuming test_images is your test dataset

# Ensure that the single image has the right shape (28, 28, 1)
single_image = np.reshape(single_image, (28, 28, 1))

output = forward(single_image, weights_conv1, biases_conv1, weights_conv2, biases_conv2, weights_conv3, biases_conv3, weights_dense1, biases_dense1, weights_dense2, biases_dense2)

# output now contains the predicted probabilities for each class

```
Remember that this code assumes that your input image is already preprocessed (rescaled and reshaped as necessary) to match the input that the CNN model expects.

## USER
How can i get the information of the CNN?

## ASSISTANT
If you want to understand the details of the layers, their shapes, and parameters of your model, you can use the summary() function provided by Keras. It prints a summary representation of your model.

Here's how you can use it:
```python
model.summary()
```
Running this will give you output similar to the following (for your specific model):
```text
Model: "sequential"
_________________________________________________________________
 Layer (type)                Output Shape              Param #   
=================================================================
 conv2d (Conv2D)             (None, 26, 26, 4)         40        
                                                                 
 max_pooling2d (MaxPooling2D  (None, 6, 6, 4)          0         
 )                                                               
                                                                 
 conv2d_1 (Conv2D)           (None, 4, 4, 8)           296       
                                                                 
 max_pooling2d_1 (MaxPooling  (None, 2, 2, 8)          0         
 2D)                                                             
                                                                 
 flatten (Flatten)           (None, 32)                0         
                                                                 
 dense (Dense)               (None, 10)                330       
                                                                 
=================================================================
Total params: 666
Trainable params: 666
Non-trainable params: 0
```
This summary includes the type of each layer, output shape of each layer, and the number of parameters (weights and biases) that each layer has.

If you want to inspect the model's architecture in more detail, you can access the individual layers with model.layers and examine their configurations with the get_config() method:
```python
for layer in model.layers:
    print(layer.get_config())
```
This will print out a dictionary of each layer's configuration, including its name, activation function, whether it uses bias, and other details.

# Functionality tests
**Functionality tests were performed with the following code:**
```python
correct_predictions = 0
#(x_train, y_train), (x_test, y_test) = mnist.load_data() #%%This was doing before
# iterate over the test set
#for i in range(len(x_test)):

for i in range(10000):
    # prepare the input
    x = test_images[i].astype(np.float16)

    # Ensure that the single image has the right shape (28, 28, 1)
    single_image = np.reshape(x, (28, 28, 1))

    #output = forward(single_image, weights_conv1, biases_conv1, weights_conv2, biases_conv2, weights_conv3, biases_conv3, weights_dense1, biases_dense1, weights_dense2, biases_dense2)
    output = forward(single_image, weights_conv1, biases_conv1, weights_conv2, biases_conv2, weights_dense2, biases_dense2)

    # Find the index of the maximum value
    prediction = np.argmax(output)

    # compare the prediction with the actual label
    actual_label = np.argmax(test_labels[i])
    #actual_label = test_labels[i]

    if prediction == actual_label:
        correct_predictions += 1

# calculate the accuracy
accuracy = correct_predictions / 10000
print("The accuracy of the forward_pass function is:", accuracy)
```
```
