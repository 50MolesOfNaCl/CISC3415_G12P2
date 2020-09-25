#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>

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
  
  double xPos;
  double yPos;
  double yaw;
  
  // Control loop
  while(true){    
	double turnrate, speed;
	// Read from the proxies.
	robot.Read();
	//odometry
	xPos = pp.GetXPos();
	yPos = pp.GetYPos();
	yaw = pp.GetYaw();
	
	// What does odometry tell us? In other words, how far do we
    // think we have gone?
    std::cout << "x: " << xPos  << std::endl;
    std::cout << "y: " << yPos  << std::endl;
    std::cout << "a: " << yaw  << std::endl;


    // Print out what the bumpers tell us:
    std::cout << "Left  bumper: " << bp[0] << std::endl;
    std::cout << "Right bumper: " << bp[1] << std::endl;
    speed = 0;//reset to avoid breaking stuff
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
		if((xPos < 11 && yPos < -1) && yaw > (dtor(90) || yaw > 0) ){ //Out of bound South!
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
		//Does not work yet
		} else if((xPos < 0 && yPos > -1) && (yaw > 0 || yaw < dtor(-90)) ){ //Out of bound West!
			std::cout <<"Out of bound West! Adjusting yaw to something between 0 and -90 azimuth"<< std::endl;
			turnrate=dtor(10);
			speed=0;
		} else {
			turnrate=0;
			speed=0.5;//good to go, keep going forward!
		}
	}

    // What did we decide to do?
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Turn rate: " << turnrate << std::endl << std::endl;

    // Send the motion commands that we decided on to the robot.
    pp.SetSpeed(speed, turnrate);  
    }//EOL
  
}
