import os

# Step 1: Define the file name
filename = "my_file.txt"

# Step 2: Check if the file exists
if not os.path.exists(filename):
    # Step 3: Create and write to the file
    with open(filename, "w") as f:
        f.write("Hello, this is a newly created file!\n")
        f.write("It contains some sample text.\n")
    print(f"{filename} has been created.")
else:
    print(f"{filename} already exists.")

# Step 4: Read and print the contents
with open(filename, "r") as f:
    print("\nFile contents:")
    for line in f:
        print(line.strip())
#with open(filename, "r") as f:
#    stuff = f.readlines()
#    print(f"Contents of the file:{stuff}") 
