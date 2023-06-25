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
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing.image import ImageDataGenerator



batch_size = 2
image_size = (224, 224)

model_names = ['RC5-RSA_1rb','RC5-RSA_2rb','RC5-RSA_3rb','RC5-RSA-EXTRA_4rb','RC5-RSA-EXTRA_5rb','RC5-RSA_6rb','RC5-RSA_7rb']
target_sizes = {
    'RC5-RSA_1rb': (224, 224),
    'RC5-RSA_2rb': (224, 224),
    'RC5-RSA_3rb': (224, 224),
    'RC5-RSA-EXTRA_4rb': (224, 224),
    'RC5-RSA-EXTRA_5rb': (224, 224),
    'RC5-RSA_6rb': (224, 224),
    'RC5-RSA_7rb': (224, 224)
}
accuracies = []

# Загрузка моделей и тестирование по одной модели
for model_name in model_names:
    model_path = f'/Users/Alex/Desktop/pr/{model_name}.h5'
    model = tf.keras.models.load_model(model_path)

    test_data_directory = f'/Users/Alex/Desktop/pr/7dataset/rc5_datasets/{model_name}/test'
    target_size = target_sizes[model_name]
    test_data_generator = tf.keras.preprocessing.image_dataset_from_directory(
        test_data_directory,
        image_size=image_size,
        batch_size=batch_size,
        shuffle=False
    )

    # Оценка точности модели на тестовых данных
    loss, accuracy = model.evaluate(test_data_generator)
    print(f"Accuracy for {model_name}: {accuracy}")

    accuracies.append(accuracy)

# Создание графика точности моделей
plt.figure(figsize=(10, 6))
plt.bar(np.arange(len(model_names))+1, accuracies)
plt.xlabel('Model')
plt.ylabel('Accuracy')
plt.title('Model Accuracy')
plt.ylim(0, 1)
plt.grid(True)  # Добавление сетки на график
plt.xticks(np.arange(len(model_names))+1, range(1, len(model_names)+1))  # Изменение названий моделей на числа
plt.savefig('accuracy_bar.png')

# Создание гистограммы точности моделей
plt.figure(figsize=(10, 6))
plt.hist(accuracies, bins=10)
plt.xlabel('Accuracy')
plt.ylabel('Frequency')
plt.title('Accuracy Distribution')
plt.savefig('accuracy_histogram.png')

plt.show()

# models1 = ['Speck_1rb.h5', 'Speck_2rb.h5', 'Speck_3rb.h5', 
#           'Speck_4rb.h5', 'Speck_5rb.h5', 'Speck_6rb.h5', 
#           'Speck_7rb.h5', 'Speck_8rb.h5',
#           'Speck_9rb.h5']



# test_datasets = []
# for i in range(len(models1)):
#     # Загрузка тестовых данных из папки для соответствующей модели
#     test_data = tf.keras.preprocessing.image_dataset_from_directory(
#         os.path.join("/Users/Alex/Desktop/pr/7dataset/speck_datasets/Speck/", f'test{i+1}'),
#         batch_size=batch_size,
#         image_size=(224, 224))   
#     test_datasets.append(test_data)

# # Создание списка процентов тестовой выборки
# test_percents = []
# for i, model in enumerate(models1):
#     # Загрузка модели
#     loaded_model = tf.keras.models.load_model(model)
#     # Получение процента тестовой выборки
#     test_loss, test_accuracy = loaded_model.evaluate(test_datasets[i])
#     test_percents.append(test_accuracy)

# print("Part 2")



# # Создание графика

# plt.grid(color='gray', linestyle='--', linewidth=0.5, alpha=0.5)
# plt.bar(models1, test_percents)
# plt.xticks(rotation=90)
# plt.yticks([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])
# plt.ylabel('Точность на тестовой выборке')
# plt.legend()
# plt.show()




print("END")