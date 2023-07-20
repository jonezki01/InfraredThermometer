// Call necessary libraries for sensors and LCD display functions.
#include <Adafruit_MLX90614.h>  // IR Sensor
#include <Adafruit_VL53L0X.h>   // Laser range sensor
#include <Adafruit_SH110X.h>    // LCD display
#include <Wire.h>               // I2C bus     

// Declare "mlx" and "lox" variables to be used to call functions from respective sensor libraries
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); 
Adafruit_VL53L0X lox = Adafruit_VL53L0X(); 

#define i2c_Address 0x3c // Define i2c address to memory location 0x3c
#define SCREEN_WIDTH 128 // Define OLED display width, in pixels
#define SCREEN_HEIGHT 64 // Define OLED display height, in pixels

// Create and class called "display" with specified parameters
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Declare button pin
int buttonPin = 2;
int buttonState = 0;
bool displayOn = true;  // Keep track of display on/off state
int lastButtonPressTime = 0;

// Initialize settings and configurations for the board and components that the sketch will use during its execution.
void setup() 
{
  Serial.begin(9600); // Sets the data rate in bits per second (baud) for serial data transmission.
  while (!Serial);

  pinMode(buttonPin, INPUT_PULLUP); // Sets pinmode function on button pullup

  // Serial print error for troubleshoot purposes / to make sure sensor works properly
  if (!mlx.begin())
  {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.print("Error connecting to MLX sensor. Check wiring.");
    display.display();
    while (1);
  }

  // Serial print error for troubleshoot purposes / to make sure sensor works properly
  if (!lox.begin()) 
  {
    Serial.println(F("Failed to boot VL53L0X"));
    display.setCursor(0, 40);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.print("Failed to boot VL53L0X");
    display.display();
    while(1);
  }
  // Startup range sensor
  lox.startRangeContinuous();

  // Startup and clear LCD display
  display.begin(0x3c, true); 
  display.clearDisplay();
  display.display();
  delay(500);
}

// All measurements and functions 
void loop() 
{
  // Read button state
  buttonState = digitalRead(buttonPin);

  // Check if button is pressed
  if (buttonState == LOW) 
  {
    // Declare variables for measured values  
    float ambient = mlx.readAmbientTempC();
    float object = mlx.readObjectTempC();
    int distance = -1;

    // Measure distance with laser range sensor
    if (lox.isRangeComplete()) 
    {
      distance = lox.readRange() / 10; // convert distance to cm
    }

    display.setContrast(255); // Set display contrast to maximum
    display.clearDisplay();   // Clear display before printing values

    // This part of the code sets up Infrared sensors measurements on LCD display

    display.setCursor(0, 0);              // Set printing location in 
    display.setTextSize(1);               // Set text size 
    display.setTextColor(SH110X_WHITE);   // Set text color
    display.print("Ambient: ");           // Print static text "Ambient:" on display
    display.print(ambient);               // Print measured ambient temperature on display
    display.println("C  ");               // Print static text "C" after measured temperature on display
    display.setCursor(0, 20);             // Set cursor below "Ambient"
    display.print("Object: ");            // Print static text "Object:" on display
    display.print(object);                // Print measured object temperature on display
    display.print("C  ");                 // Print static text "C" after measured temperature on display

    // Our cheap range sensor max range is about 130cm after which it starts to give 817cm
    // So here we have restricted it to work on distances 0-125cm with 1cm accuracy
    // And we set up the values and placement on the LCD display.
    if (distance > 125) 
    {
      distance = 125;
    }
    if (distance >= 125) 
    {
      display.setCursor(0, 40);
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.print("OUT OF RANGE!");
    } 
      else 
      {
        display.setCursor(0, 40);
        display.setTextSize(1);
        display.setTextColor(SH110X_WHITE);
        display.print("Distance: ");
        display.print(distance);
        display.print("cm");
      }

    display.display(); // This function is used to update the display with the contents that have been drawn on it.
    lastButtonPressTime = millis(); // Save the time of the button press
  } 
    else if (millis() - lastButtonPressTime > 10000) 
    { // If 10 seconds have elapsed since the last button press
    display.setContrast(0); // Set display contrast to zero (i.e. dim the screen)
    display.clearDisplay();
    display.display();
    } 
      else // Otherwise, the button has not been pressed and less than 10 seconds have elapsed since the last button press
      { 
    // Do nothing
      }
}