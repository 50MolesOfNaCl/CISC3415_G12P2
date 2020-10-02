/*
 * bumper.cc
 *
 * Group 12: Jennie Kang, Edmund Lam, Jamila Toaha
 *
 * Project 2 - World 2: Uses bumper proxy and odometry information to make circuit around obstacle.
 *  - Uses odemetry to create a tighter bounding box that robot travels around
 *  - Uses bumper proxy to adjust robot's direction
 *  - Checks state in the isStuck function and gets the robot out of situations where it becomes stuck.
 *
 * Date: 9/27/2020
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
int isStuckCount = 0;	 		// Give time for program to register if robot is stuck

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
      yawState[0] == yawState[1] && yawState[1] == yawState[2] && isStuckCount++ > 1 )
	return true;
 
  isStuckCount = 0;
	return false;
}



int main(int argc, char *argv[])
{  
  using namespace PlayerCc;  

  // Set up proxies. Proxies are the datastructures that Player uses to
  // talk to the simulator and the real robot.

  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0);       // The bumper proxy reads from the bumpers.
  Position2dProxy pp(&robot,0);       // The 2D proxy reads odometry from the robot and sends motion commands.

  // Allow the program to take charge of the motors (take care now)
  pp.SetMotorEnable(true);
  
  double xPos, yPos, yaw;
  double timer = 0;
  int randomTurnRate; 			//This is just a placeholder for random variable btw 0 and 1
  int randomSpeed;                      //This is just a placeholder for random variable btw 0 and 1	

  // Control loop
  while(true){    
	double turnrate, speed;
	// Read from the proxies.
	robot.Read();
	//odometry
	xPos = pp.GetXPos();
	yPos = pp.GetYPos();
	yaw = pp.GetYaw();
	
	bool stuckResult = isStuck(xPos, yPos, yaw); 
	
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
        // If either bumper is pressed
	if(bp[0] || bp[1]){
		speed=-0.5; //Go backwards

		// Left bumper in contact, right bumper not in contact. Turn
		// to the right.
		if (bp[0] && !bp[1]) {  
			turnrate=dtor(-10);  
		}
	
		// Left bumper not in contact, right bumper in contact. Turn
		// to the left
		if (!bp[0] && bp[1]) {
			turnrate=dtor(10);
		}
	
		// Both left and right bumpers in contact.
		if (bp[0] && bp[1]) {
			//Make a random choice about which way to turn.
			if(rand()%2 > 0){
				turnrate=dtor(-10);
			} else {
				turnrate=dtor(10);
			}
		}
	}
	else { //Bumpers are clear
		//For some reason this ignores 3.1415 and will go left if you turn that way. temp fix by turning right
		if((xPos < 11 && yPos < -1) && ((yaw < 0 ) || yaw < dtor(45)) ){ //Out of bound South!
			std::cout <<"Out of bound South!"<< std::endl;
			turnrate=dtor(10);
			speed=0;
			//code to go forward or else we turn in circles forever
		}else if((xPos > 11 && yPos < 11) && (yaw < dtor(110))){ //Out of bound East!
			std::cout <<"Out of bound East! Adjusting yaw to "<< dtor(110)<< std::endl;
			turnrate=dtor(10);
			speed=0;
			//code to go forward or else we turn forever
		} else if((xPos > 0 && yPos > 11) && (yaw > dtor(-110))){//Out of bound North!
			//Anything between 180 and 260 azimuth is acceptable
			std::cout <<"Out of bound North! Adjusting yaw to something between 180 and 260 azimuth"<< std::endl;
			turnrate=dtor(10);
			speed=0;
		} else if((xPos < 0 && yPos > -1) && (yaw > 0 || yaw < dtor(-90)) ){ //Out of bound West!
			std::cout <<"Out of bound West! Adjusting yaw to something between 0 and -90 azimuth"<< std::endl;
			turnrate=dtor(10);
			speed=0;
		} else {
			turnrate=0;
			speed=0.5;//good to go, keep going forward!
		}
	}

    // What does odometry tell us? In other words, how far do we
    // think we have gone?
    std::cout << "x: " << xPos  << std::endl;
    std::cout << "y: " << yPos  << std::endl;
    std::cout << "a: " << yaw  << std::endl;
    std::cout << "isStuck? " << stuckResult  << std::endl;

    // Print out what the bumpers tell us:
    std::cout << "Left  bumper: " << bp[0] << std::endl;
    std::cout << "Right bumper: " << bp[1] << std::endl;
    //speed = 0;//reset to avoid breaking stuff

    // What did we decide to do?
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Turn rate: " << turnrate << std::endl << std::endl;
    std::cout << "xState " << xState[0] << ", " << xState[1] << ", " << xState[2] << std::endl << std::endl;
    std::cout << "yState " << yState[0] << ", " << yState[1] << ", " << yState[2] << std::endl << std::endl;
    std::cout << "yawState " << yawState[0] << ", "<< yawState[1] << ", " << yawState[2] << std::endl << std::endl;

    // Send the motion commands that we decided on to the robot.
    pp.SetSpeed(speed, turnrate);  
    } // EOL
  
}


