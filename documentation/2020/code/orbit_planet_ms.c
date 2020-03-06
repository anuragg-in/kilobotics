#include <kilolib.h>

//defining states
#define MESSAGE_CHECK 0
#define DISTANCE_CHECK 1
#define TURN_LEFT 2
#define TURN_RIGHT 3
#define INITIALIZE 4
#define LEFT 100
#define RIGHT 150

#define THRESHOLD 50                 //orbit radius
#define MOTOR_ON_DURATION 500        //time delay between message receive and next message check
#define TOTAL_NUM_COMMUNICATION 4    //number of times we have to check for message from star

int state = INITIALIZE, distance, message_rx_status = 0, count = 0, temp_distance;

void message_rx(message_t *m, distance_measurement_t *d)
{
	message_rx_status = 1;                   //initializing the algo once you receive the message
	temp_distance = estimate_distance(d);    //estimating the distance between planet and star
}
	

void move(int direction)
{
	switch(direction)                            //defining which motion of motors
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
			if(message_rx_status == 1)
			{
				message_rx_status = 0;	           //to identify the next message from any of the star
				count++;
				if(temp_distance < distance)          //to find which star is nearer to the planet
				{
					distance = temp_distance;
				}
				if(count == TOTAL_NUM_COMMUNICATION)   //count for no. of messages received 
				{
					count = 0;			
					state = DISTANCE_CHECK;
				}
			}
			
		break;

		case DISTANCE_CHECK:
			if(distance>THRESHOLD)                          //to check which motor should turn ON
			{
				state = TURN_RIGHT;

			}
			else
			{
				state = TURN_LEFT;
			}
			
		break;

		case TURN_LEFT:                             //right motor turns ON and runs for specified time delay (MOTOR_ON_DURATION)
			move(LEFT);
			state = INITIALIZE;			
		break;

		case TURN_RIGHT:                           //left motor turns ON and runs for specified time delay (MOTOR_ON_DURATION)
			move(RIGHT);
			state = INITIALIZE;			
		break;

		case INITIALIZE:                           
			distance = 1000;
			state = MESSAGE_CHECK;
		break;

		default:
		break;
	}

}



int main()
{
	kilo_init();                             //initializing the Kilobot(planet)
	kilo_message_rx = message_rx;            //register message_rx call back function
	kilo_start(setup,loop);

	return 0;
}
