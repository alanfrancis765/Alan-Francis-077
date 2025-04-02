class Vehicles:
    def __init__(self, maxspeed, mileage, color ='white'):
        self.maxspeed = maxspeed
        self.mileage = mileage
        self.color = color
    def seats(self, max_seat):
        self.max_seat = max_seat
    def display(self):
        print("Maximum speed of the car:",car.maxspeed,"kph")
        print("Mileage per liter:",car.mileage,"kmpl")
        print("color:", car.color)
        print("Maximum seats:", car.max_seat)

print("Porsche 911 Carrera:")    

Carrera  = Vehicles(200, 50000)
Carrera .seats(5)
Carrera .display()

print("Volvo:")

Volvo = Vehicles(180, 75000)
Volvo.seats(5)
Volvo.display()
