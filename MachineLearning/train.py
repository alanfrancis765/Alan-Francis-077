import tensorflow as tf

# ✅ Use the correct dataset paths
train_dir = r"C:\Users\alanf\Downloads\Fruit-Images-Dataset-master\apple-vs-mandarin\train"
test_dir = r"C:\Users\alanf\Downloads\Fruit-Images-Dataset-master\apple-vs-mandarin\test"

# ✅ Load dataset
train_ds = tf.keras.preprocessing.image_dataset_from_directory(
    train_dir,
    image_size=(64, 64),
    batch_size=32,
    label_mode="binary"
)

test_ds = tf.keras.preprocessing.image_dataset_from_directory(
    test_dir,
    image_size=(64, 64),
    batch_size=32,
    label_mode="binary"
)

# ✅ Model definition
model = tf.keras.Sequential([
    tf.keras.layers.Rescaling(1./255, input_shape=(64, 64, 3)),
    tf.keras.layers.Conv2D(32, 3, activation='relu'),
    tf.keras.layers.MaxPooling2D(),
    tf.keras.layers.Conv2D(64, 3, activation='relu'),
    tf.keras.layers.MaxPooling2D(),
    tf.keras.layers.Flatten(),
    tf.keras.layers.Dense(64, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid')  # Binary classification
])

# ✅ Compile
model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])

# ✅ Train
model.fit(train_ds, validation_data=test_ds, epochs=5)
model.save("apple_vs_mandarine_model.keras")
