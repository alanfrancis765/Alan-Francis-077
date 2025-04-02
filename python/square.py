import math

def perform_calculation():
    try:
        number1 = float(input("Enter the number: ")) 
        result = math.sqrt(number1)  
        print(f"Result: {result}")
    except ValueError as e: 
        print(f"Error: {e}")
perform_calculation()
