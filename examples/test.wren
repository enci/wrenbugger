class System {
    foreign static setPixel(x, y, on)
    foreign static clearDisplay()
    foreign static input()
    foreign static print(text)
}

class Main {
    static run() {
        System.print("Wrenbugger demo - WASD to move, Q to quit")
        
        var x = 3
        var y = 3
        
        while (true) {
            System.clearDisplay()
            System.setPixel(x, y, true)
            
            var key = System.input()
            
            if (key == "w" && y > 0) y = y - 1
            if (key == "s" && y < 7) y = y + 1
            if (key == "a" && x > 0) x = x - 1
            if (key == "d" && x < 7) x = x + 1
            if (key == "q") break
            
            System.print("Position: %(x), %(y)")
        }
        
        System.print("Goodbye!")
    }
}

Main.run()
