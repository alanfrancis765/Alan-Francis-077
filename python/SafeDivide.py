class SafeDivide:
    def safe_divide(self):
        try:
            a = int(input("Enter a: "))  
            b = int(input("Enter b: ")) 
            c = a / b  
            print("Result:", c)
        except ZeroDivisionError:
            print("Error: Cannot divide by zero!")
        except ValueError:
            print("Error: Invalid input! Please enter numbers only.")
        except Exception as e:
            print("Unexpected error:", e)



obj = SafeDivide()
obj.safe_divide()
