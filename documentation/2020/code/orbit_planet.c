#include <kilolib.h>

//defining states 
#define MESSAGE_CHECK 0 
#define DISTANCE_CHECK 1
#define TURN_LEFT 2
#define TURN_RIGHT 3
#define LEFT 100
#define RIGHT 150

//radius of the orbit 
#define THRESHOLD 50

//time delay between message receive and next message check
#define MOTOR_ON_DURATION 500

int state = MESSAGE_CHECK, distance, message_rx_status = 0;

void message_rx(message_t *m, distance_measurement_t *d)
{
	message_rx_status = 1;                   //initializing the algo once you receive the message
	distance = estimate_distance(d);         //estimating the distance between planet and star
}
	

void move(int direction)
{
	switch(direction)                         //defining which motion of motors
	{
		case LEFT:
			spinup_motors();
			set_motors(kilo_straight_left, 0);
			delay(MOTOR_ON_DURATION);
			set_motors(0, 0);
		break;
		
		case RIGHT:
			spinup_motors();
			set_motors(0, kilo_straight_right);
			delay(MOTOR_ON_DURATION);
			set_motors(0, 0);
		break;
		
		default:
		break;
	}
}

void setup()
{
	
}

void loop()
{
	switch(state)
	{
		case MESSAGE_CHECK:
			if(message_rx_status==1)
			{
				message_rx_status = 0;	      //to identify the next message from star			
				state = DISTANCE_CHECK;
			}
			
		break;

		case DISTANCE_CHECK:                          //to check which motor should turn ON
			if(distance>THRESHOLD)
			{
				state = TURN_RIGHT;

			}
			else
			{
				state = TURN_LEFT;
			}
		break;

		case TURN_LEFT:                              //right motor turns ON and runs for specified time delay (MOTOR_ON_DURATION)
			move(LEFT);
			state = MESSAGE_CHECK;
		break;

		case TURN_RIGHT:                             //left motor turns ON and runs for specified time delay (MOTOR_ON_DURATION)
			move(RIGHT);
			state = MESSAGE_CHECK;
		break;

		default:
		break;
	}

}



int main()
{
	kilo_init();                      //initializing the Kilobot(planet) 
	kilo_message_rx = message_rx;     //register message_rx call back function
	kilo_start(setup,loop);

	return 0;
}
