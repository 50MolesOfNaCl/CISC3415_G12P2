/*
 * bumper.cc
 *
 * Group 12: Jennie Kang, Edmund Lam, Jamila Toaha
 *
 * Project 2 - Part 2 - Maze
 *  Robot primarily uses odometry to make it's way through the maze. Starts at bottom left corner and stops 
 *  at top left corner. 
 * -isStuck function is added just in case
 *
 * Date: 10/6/2020
 * 
 * Original Documentation: 
 *  Sample code for a robot that has two front bumpers and uses them to
 *  avoid obstacles. Suitable for use with the Roomba and Create. 
 * 
 *  Works IRL. Doesn't work so well in Stage, probably because of the
 *  crude modeling of the bumpers, and can get stuck in convex shapes
 *  when it backs into an object while trying to turn away from it.
 *
 *  Based on an example provided by Monica Anderson and Jeff Forbes,
 *  via Carlos Jaramillo, and changed to (hopefully) make it easier to
 *  understand.
 *
 *  Modified:    Simon Parsons
 *  Date:        15th June 2009
 *  Last change: 19th September 2011
 *  
 */


#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>
#include <math.h>

using namespace std;


// Creates values for x and y values for isStuck function
double xState [3] = {};			// xState: {xPreviousPrevious, xPrevious, xCurrent}
double yState [3] = {};			// yState: {yPreviousPrevious, yPrevious, yCurrent}
double yawState [3] = {}; 		// yawState: {yawPreviousPrevious, yawPrevious, yawCurrent}
//int isStuckCount = 0;	 		// Give time for program to register if robot is stuck

// Function determines if robot is stuck by comparing the last 3 states the robot is in
// Goal: to compare x, y, and yaw values from 3 consecutive cycles
bool isStuck(double xCurrent, double yCurrent, double yawCurrent){

  // The follow updates State arrays, to contain the 3 most recent x and y coordinates and yaw orientations

  xState[0] = xState[1];
  xState[1] = xState[2];
  xState[2] = xCurrent;

  yState[0] = yState[1];
  yState[1] = yState[2];
  yState[2] = yCurrent;

  yawState[0] = yawState[1];
  yawState[1] = yawState[2];
  yawState[2] = yawCurrent;

  // If the values from the last 3 cycles are the same across the x States, the y States, and the yaw States for more than one second
  if( xState[0] == xState [1] && xState[1] == xState[2] &&
      yState[0] == yState [1] && yState[1] == yState[2] &&
      yawState[0] == yawState[1] && yawState[1] == yawState[2])
	return true;
 
	return false;
}






int main(int argc, char *argv[])
{  
  using namespace PlayerCc;  

  // Set up proxies. Proxies are the datastructures that Player uses to
  // talk to the simulator and the real robot.

  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0);       // The bumper proxy reads from the
                                      // bumpers.
  Position2dProxy pp(&robot,0);       // The 2D proxy reads odometry from 
                                      // the robot and sends motion commands.

  // Allow the program to take charge of the motors (take care now)
  pp.SetMotorEnable(true);

  double xPos, yPos, yaw;
  double timer = 0;
  int randomTurnRate; 			//Provided to help robot get unstuck
  int randomSpeed;                      //Provided to help robot get unstuck	

  // Control loop
  while(true) 
    {    
      double turnrate, speed;

      // Read from the proxies.
      robot.Read();

      // What does odometry tell us? In other words, how far do we
      // think we have gone?
      std::cout << "x: " << pp.GetXPos()  << std::endl;
      std::cout << "y: " << pp.GetYPos()  << std::endl;
      std::cout << "a: " << pp.GetYaw()  << std::endl;


      // Print out what the bumpers tell us:
      std::cout << "Left  bumper: " << bp[0] << std::endl;
      std::cout << "Right bumper: " << bp[1] << std::endl;

	xPos = pp.GetXPos();
	yPos = pp.GetYPos();
	yaw = pp.GetYaw();

bool stuckResult = isStuck(xPos, yPos, yaw); 
	
	//Checks if robot is stuck. If it is stuck will adjust turnrate and/or speed in random fashion so robot can become unstuck
	if(stuckResult) {

		//The is to allow the random turn rate and speed to stay consistent for a few cycles 
		//so that robot can make a consistent move/ progress in that direction
		if(timer == 0) {
		  randomTurnRate = rand () % 60 - 30; 		// Choose a turnrate btw 30 and -30 degrees


		  randomSpeed = rand() % 2;			// random variable to choose between 2 speeds
		  //Adjust speed every 1 second that the robot is stuck
		  speed = randomSpeed == 0 ? -.5 : .5; 		// Go forwards or backwords
		}
		
    		std::cout << "isStuck speed" << speed  << std::endl;
   		std::cout << "isStuck yaw" << yaw  << std::endl;
  		
		//adjust turn rate every cycle it's stuck
                turnrate =  dtor(randomTurnRate);
		
		//Give robot 1 second to move in random direction (forward or backwards) and/or random degree
		if (timer > 10) 
			timer = 0;
		else {
			timer++;
		}
		
	}
	else
      // If either bumper is pressed, go backwards. Depending on the
      // combination of bumpers that are pressed, turn some also,
      // trying to turn away from the point of contact. 
      //
      // Otherwise just go forwards
      if(bp[0] || bp[1]){

	speed=-0.5;

	// Left bumper in contact, right bumper not in contact. Turn
	// to the right.  
	//
	// dtor converts from degrees to radians.
	if (bp[0] && !bp[1]) {  
	  turnrate=dtor(-25);  
	}

	// Left bumper not in contact, right bumper in contact. Turn
	// to the left
	if (!bp[0] && bp[1]) {
	  turnrate=dtor(25);
	}

	// Both left and right bumpers in contact. Make a random
	// choice about which way to turn.
	if (bp[0] && bp[1]) {
	  if(rand()%2 > 0){
	    turnrate=dtor(-25);
	  }
	  else {
	    turnrate=dtor(25);
	  }
	}
      } 
	//bumpers are not pressed
      else {
	//turn left when you reach the first turn
	if(xPos > 10) {
		turnrate=dtor(15);
		speed = 0.5;
	}
	//turn left when you reach the second turn
	else if(xPos < 1 && (yPos > 2 && yPos < 9)) {
		turnrate=dtor(-14);
		speed = 0.5;
	}
	//turn right when you reach the third turn
	else if(xPos > 10 && yPos > 5){
		turnrate=dtor(15);
		speed = 0.5;
	}
	//stops the robot at the top left corner
	else if(xPos < 0 && yPos > 9) {
		turnrate = 0;
		speed = 0;
		break;
		
	}		
	//keep going forward
   	else {
	  turnrate = 0;
	  speed=0.5;
	}

	}     

      // What did we decide to do?
      std::cout << "Speed: " << speed << std::endl;      
      std::cout << "Turn rate: " << turnrate << std::endl << std::endl;

      // Send the motion commands that we decided on to the robot.
      pp.SetSpeed(speed, turnrate);  
    }
  
}



