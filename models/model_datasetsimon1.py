import tensorflow as tf
import tensorflow_datasets as tfds
import h5py
from tensorflow import keras
from keras import layers
from matplotlib import pyplot
import os
import numpy as np
from tensorflow.keras.models import Sequential, Model
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Dense, Flatten, Dropout, GlobalAveragePooling2D
from tensorflow.keras import utils
from tensorflow.keras.preprocessing import image
from tensorflow.keras.preprocessing import image_dataset_from_directory
from tensorflow.keras.models import Model
from tensorflow.keras.applications.resnet import ResNet50
from tensorflow.keras.applications import ResNet101
from tensorflow.keras.applications.resnet_v2 import ResNet50V2
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import matplotlib.pyplot as plt
from tensorflow.keras.layers import Input
from tensorflow.keras.applications import EfficientNetB0
from tensorflow.keras.applications import InceptionResNetV2

print("Num GPUs Available: ", len(tf.config.list_physical_devices('GPU')))
tf.config.list_physical_devices('GPU')

batch_size=16
image_size=(100, 100)

#
for dataset_name in ['Simon1rb', 'Simon2rb']:
    train_dataset = image_dataset_from_directory(f'/Dataset/{dataset_name}/train',
                                                 subset='training',
                                                 seed=42,
                                                 validation_split=0.1,
                                                 batch_size=batch_size,
                                                 image_size=image_size)


    validation_dataset = image_dataset_from_directory(f'/Dataset/{dataset_name}/train',
                                             subset='validation',
                                             seed=42,
                                             validation_split=0.1,
                                             batch_size=batch_size,
                                             image_size=image_size)



    # Проверка меток
    print(train_dataset.class_names)
    print(validation_dataset.class_names)
    num_classes = len(train_dataset.class_names)
    print(num_classes)


    test_dataset = image_dataset_from_directory(f'/Dataset/{dataset_name}/test',
                                                batch_size=batch_size,
                                                image_size=image_size)



    AUTOTUNE = tf.data.experimental.AUTOTUNE
    train_dataset = train_dataset.prefetch(buffer_size=AUTOTUNE)
    validation_dataset = validation_dataset.prefetch(buffer_size=AUTOTUNE)
    test_dataset = test_dataset.prefetch(buffer_size=AUTOTUNE)

    #Предобученная модель EfficientNet, ResNet, Inception Resnetv2
    base_model = EfficientNetB0(weights='imagenet', include_top=False, input_shape=(100, 100, 3))
    x = base_model.output

    x = layers.Conv2D(512, kernel_size=(3, 3), activation='relu', padding='same')(x)
    #x = layers.MaxPooling2D(pool_size=(2, 2))(x)
    residual = x
    x = layers.Conv2D(256, kernel_size=(1, 1), activation='relu')(x)
    x = layers.Conv2D(256, kernel_size=(3, 3), activation='relu', padding='same')(x)
    x = layers.Conv2D(1024, kernel_size=(1, 1))(x)
    shortcut = layers.Conv2D(1024, kernel_size=(1, 1))(residual)
    x = layers.Add()([x, shortcut])
    x = layers.Activation('relu')(x)

    residual2 = x
    x = layers.Conv2D(512, kernel_size=(1, 1), activation='relu')(x)
    x = layers.Conv2D(512, kernel_size=(3, 3), activation='relu', padding='same')(x)
    x = layers.Conv2D(2048, kernel_size=(1, 1))(x)
    shortcut2 = layers.Conv2D(2048, kernel_size=(1, 1))(residual2)
    x = layers.Add()([x, shortcut2])
    x = layers.Activation('relu')(x)
    
        residual2 = x
    x = layers.Conv2D(512, kernel_size=(1, 1), activation='relu')(x)
    x = layers.Conv2D(512, kernel_size=(3, 3), activation='relu', padding='same')(x)
    x = layers.Conv2D(2048, kernel_size=(1, 1))(x)
    shortcut2 = layers.Conv2D(2048, kernel_size=(1, 1))(residual2)
    x = layers.Add()([x, shortcut2])
    x = layers.Activation('relu')(x)

    x = layers.Flatten()(x)
    outputs = layers.Dense(1, activation='sigmoid')(x)
    model = Model(inputs=base_model.input, outputs=outputs)

    model.compile(optimizer = 'adam', loss='binary_crossentropy', metrics = ['accuracy'])

    history = model.fit(train_dataset, 
                        validation_data=validation_dataset,
                        epochs=10,
                        verbose=2)

    scores = model.evaluate(test_dataset, verbose=1)

    print("Доля верных ответов на тестовых данных, в процентах:", round(scores[1] * 100, 4))


    #model.save(f"/Project/{dataset_name}.h5")
    model.save_weights(f"/Project/{dataset_name}_resnetv2_weights.h5")



print("END")