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
    std::cout << "x: " << pp.GetXPos()  << std::endl;
    std::cout << "y: " << pp.GetYPos()  << std::endl;
    std::cout << "a: " << pp.GetYaw()  << std::endl;


    // Print out what the bumpers tell us:
    std::cout << "Left  bumper: " << bp[0] << std::endl;
    std::cout << "Right bumper: " << bp[1] << std::endl;
      
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
		//Should be fine since origin remains the same
		//screwed if not, would have to modify.
		if(xPos > 11 || yPos >11){ //too far from origin we need to turn back.
			turnrate=dtor(10);//turn left
			//code to go forward or else we turn in circles forever
		}else if(xPos < -1 || yPos <-){ //off course need to adjust.
			turnrate=dtor(-10);//turn right
			//code to go forward or else we turn forever
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
