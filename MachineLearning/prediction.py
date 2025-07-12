import tensorflow as tf
from tensorflow.keras.models import load_model
from tensorflow.keras.utils import load_img, img_to_array
import numpy as np
import matplotlib.pyplot as plt 


# Load your saved model
model = load_model("apple_vs_mandarine_model.h5")

# Load and preprocess the image
img_path = r"C:\Users\alanf\Downloads\test-pic.jpg"  # Update this to your image
img = load_img(img_path, target_size=(64, 64))
img_array = img_to_array(img)
img_array = tf.expand_dims(img_array, 0)  # Add batch dimension

# Predict
prediction = model.predict(img_array)
class_names = ['Apple', 'Mandarine']
predicted_class = class_names[int(prediction[0] > 0.5)]

# Function to explain prediction
def display_prediction(predicted_class):
    if predicted_class == 'Apple':
        print(f"Prediction: {predicted_class}")
        return "An apple is a round, edible fruit with a variety of colors (red, green, yellow) and flavors grown on the apple tree (Malus domestica)."
    else:
        print(f"Prediction: {predicted_class}")
        return "A mandarine is a small citrus fruit with a loose, easy-to-peel skin, typically orange in color and sweet in flavor."

# Call and display
explanation = display_prediction(predicted_class)
print(explanation)

plt.imshow(img)
plt.title(f"Prediction: {predicted_class}")
plt.axis('off')
plt.show()

