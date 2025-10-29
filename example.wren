// Sample Wren script for testing the debugger
System.print("Hello from Wren!")

var x = 10
var y = 20
var sum = x + y

System.print("The sum of %(x) and %(y) is %(sum)")

class Greeter {
  construct new(name) {
    _name = name
  }
  
  greet() {
    System.print("Hello, %(_name)!")
  }
}

var greeter = Greeter.new("World")
greeter.greet()

for (i in 1..5) {
  System.print("Count: %(i)")
}

System.print("Wren script completed!")
