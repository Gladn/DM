import tensorflow as tf
import tensorflow_datasets as tfds
from tensorflow import keras
from keras import layers
from matplotlib import pyplot
import os
import numpy as np
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Dense, Flatten, Dropout, GlobalAveragePooling2D
from tensorflow.keras import utils
from tensorflow.keras.preprocessing import image
from tensorflow.keras.preprocessing import image_dataset_from_directory
from tensorflow.keras.models import Model
from tensorflow.keras.applications.resnet import ResNet50
import matplotlib.pyplot as plt
from tensorflow.keras.layers import Input
from keras import regularizers

print("Num GPUs Available: ", len(tf.config.list_physical_devices('GPU')))
tf.config.list_physical_devices('GPU')
TF_GPU_ALLOCATOR='cuda_malloc_async' 

batch_size=16
image_size=(100, 100)

train_dataset = image_dataset_from_directory('/t/rc5_datasets/RC5-RSA_3rb/train',
                                             subset='training',
                                             seed=42,
                                             validation_split=0.1,
                                             batch_size=batch_size,
                                             image_size=image_size)


validation_dataset = image_dataset_from_directory('/rc5_datasets/RC5-RSA_3rb/train',
                                             subset='validation',
                                             seed=42,
                                             validation_split=0.1,
                                             batch_size=batch_size,
                                             image_size=image_size)


print(train_dataset.class_names)
print(validation_dataset.class_names)
num_classes = len(train_dataset.class_names)
print(num_classes)

test_dataset = image_dataset_from_directory('/rc5_datasets/RC5-RSA_3rb/test',
                                             batch_size=batch_size,
                                             image_size=image_size)


AUTOTUNE = tf.data.experimental.AUTOTUNE
train_dataset = train_dataset.prefetch(buffer_size=AUTOTUNE)
validation_dataset = validation_dataset.prefetch(buffer_size=AUTOTUNE)
test_dataset = test_dataset.prefetch(buffer_size=AUTOTUNE)

#####
inputs = layers.Input(shape=(100, 100, 3))
x = layers.Conv2D(64, kernel_size=(3, 3), activation='relu', padding='same')(inputs)
x = layers.Conv2D(128, kernel_size=(3, 3), activation='relu', padding='same')(inputs)
x = layers.Flatten()(x)
outputs = layers.Dense(1, activation='sigmoid')(x)
model = Model(inputs=inputs, outputs=outputs)
model.compile(optimizer = 'adam', loss='binary_crossentropy', metrics = ['accuracy'])

weights = model.get_weights()
print(weights)

history = model.fit(train_dataset, 
                    validation_data=validation_dataset,
                    epochs=11,
                    verbose=2)

scores = model.evaluate(test_dataset, verbose=1)

print("Доля верных ответов на тестовых данных, в процентах:", round(scores[1] * 100, 4))

###
plt.plot(history.history['accuracy'], 
         label='Доля верных ответов на обучающем наборе')
plt.plot(history.history['val_accuracy'], 
         label='Доля верных ответов на проверочном наборе')
plt.xlabel('Эпоха обучения')
plt.ylabel('Доля верных ответов')
plt.legend()
plt.grid()
plt.show()


plt.plot(history.history['loss'], 
         label='Ошибка на обучающем наборе')
plt.plot(history.history['val_loss'], 
         label='Ошибка на проверочном наборе')
plt.xlabel('Эпоха обучения')
plt.ylabel('Ошибка')
plt.legend()
plt.grid()
plt.show()




model.save("/.../model2.h5")
print("END")