class Car:
# constructor
    def __init__(self, make, model, color):
        self.make = make
        self.model = model
        self.color = color
        self.no_of_owners = 0  

    def car_info(self):
        print("Make:", self.make)
        print("Model:", self.model)
        print("Color:", self.color)
        print("Number of Owners:", self.no_of_owners)

    def sell(self):
        self.no_of_owners += 1  

my_car = Car("BMW", "M4", "Blue")

my_car.car_info()
my_car.sell()
my_car.car_info()
