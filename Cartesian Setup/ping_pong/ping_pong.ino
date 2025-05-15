#include <AccelStepper.h> //accelstepper library

const byte limitSwitch_1 = 5; //pin for the microswitch using attachInterrupt()
const byte limitSwitch_2 = 18; //pin for the microswitch using attachInterrupt()

bool switchFlipped = false; //stores the status for flipping
bool previousFlip = true; //stores the previous state for flipping - needed for the direction change

// direction Digital 9 (CCW), pulses Digital 8 (CLK)
AccelStepper stepper(1, 15, 2);

void setup()
{
  //Limit Switches
  pinMode(limitSwitch_1, INPUT_PULLUP); // internal pullup resistor (debouncing)
  pinMode(limitSwitch_2, INPUT_PULLUP); // internal pullup resistor (debouncing)
  
  attachInterrupt(digitalPinToInterrupt(limitSwitch_1), FlipDirection, FALLING);   //do not change it to 'CHANGE'
  attachInterrupt(digitalPinToInterrupt(limitSwitch_2), FlipDirection, FALLING); 
  //---------------------------------------------------------------------------

  //Serial Communication
  Serial.begin(9600); //defining some baud rate
  Serial.println("Testing Accelstepper"); //print a message
  //---------------------------------------------------------------------------

  //Stepper parameters
  //setting up some default values for maximum speed and maximum acceleration
  stepper.setMaxSpeed(13000); //SPEED = Steps / second  
  stepper.setAcceleration(1500); //ACCELERATION = Steps /(second)^2    
  stepper.setSpeed(10000);
  delay(500);
  //---------------------------------------------------------------------------

}

void loop()
{
  
  stepper.runSpeed(); //step the motor (this will step the motor by 1 step at each loop indefinitely)
  flipCheck();   //checking the flip in each loop
}

void flipCheck()
{
  if(switchFlipped == true)
  {    
     //Serial.println(previousFlip); //This was just a control flag for debugging
    
     if(previousFlip == true) //If the previous flip is 1, we have positive direction
     {    
        stepper.setSpeed(10000);       
     }
     if(previousFlip == false) //If the previous flip is 0, we have negative direction
     {  
       stepper.setSpeed(-10000);
     }
     switchFlipped = false; 
	//We have to reset this, so in the next iteration of the loop, the code will not enter this part, only when there was a click again
  }
 
}

void FlipDirection()
{    
  switchFlipped = true; //we change the status to true, so the code will enter the flipCheck() function 
  previousFlip = !previousFlip; //change the state to different from the previous - this controls the direction
 
}
