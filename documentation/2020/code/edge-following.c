#include <kilolib.h>

//defining states
#define MESSAGE_CHECK_EF 0
#define ID_CHECK 1
#define ORBIT 2
#define TURN_LEFT 3
#define TURN_RIGHT 4
#define DISTANCE_CHECK 5
#define INITIALIZE 6
#define INFINITE 7

#define THRESHOLD 50            // distance between the moving outer edge bot and the remaining bots

//Kilobot motion parameters
#define LEFT 100
#define RIGHT 150
#define MOTOR_ON_DURATION 500

#define TOTAL_NUM_COMMUNICATION 5                        //total number of communication to identify its position
#define TOTAL_NUM_COMMUNICATION_ORBIT 3                  //total number of communication while moving towards the reference bot

int received_msg, message_sent = 0, neighbor_id = 0, state = MESSAGE_CHECK_EF, temp_distance, distance, message_rx_status = 0, count = 0, count_orbit = 0, self_unique_id = 251;
message_t message;

void message_rx(message_t *m, distance_measurement_t *d)
{
	received_msg = (*m).data[0];                             //receives unique_id of communicating bot
	message_rx_status = 1;                                   //indicates message decoded successfully
	temp_distance = estimate_distance(d);                    //distance estimation of communicating bot
	/*set_color(RGB(1,0,1));
	delay(500);*/
}

/*void LED_color()
{
    set_color(RGB((self_unique_id % 2), ((self_unique_id / 2) % 2), ((self_unique_id / 4) % 2)));
    delay(1000);
}*/

message_t *message_tx()
{
    return &message;
}


void message_tx_success()
{
    message_sent = 1;                                    //message(self unique_id) sent successfully
    /*set_color(RGB(1, 0, 1));
    delay(100);
    set_color(RGB(0, 0, 0));*/
   // LED_color();
}


void move(int direction)
{
	switch(direction)
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
            message.type = NORMAL;
            message.data[0] = kilo_uid;                                      //transmitting unique_id
            //message.data[0] = 1;
            message.crc = message_crc(&message);                             //parity check
            kilo_message_tx = message_tx;
            kilo_message_tx_success = message_tx_success;
            
}

void loop()
{
	switch(state)
	{
		case MESSAGE_CHECK_EF:                                       //message check for identifying the outer edge bot
			if(message_rx_status == 1)
			{
				message_rx_status = 0;	
				count++;
				if(received_msg > neighbor_id)
				{
					neighbor_id = received_msg;      //for each message received neighbor_id stores max(unique_id)
				}
				if(count == TOTAL_NUM_COMMUNICATION)
				{
					count = 0;			
					state = ID_CHECK;
				}
			}
			
		break; 

		case ID_CHECK:
			if(kilo_uid > neighbor_id)
			{
				//set_color(RGB(1,0,0));
				state = ORBIT;                           //if self id is more than neighbor id, starts moving towards ref. bot
				//state = INFINITE;
			}
			else
			{
			    neighbor_id = 0;
				state = MESSAGE_CHECK_EF;
			}
			
		break;
		
		case ORBIT:                                          //we've used the planet-multistar algo
			if(message_rx_status == 1)
			{
				message_rx_status = 0;	           //to identify the next message from any of the bot
				count++;
				if(temp_distance < distance)          //to find which bot is nearer to it
				{
					distance = temp_distance;
				}
				if(count == TOTAL_NUM_COMMUNICATION_ORBIT)   //count for no. of messages received 
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
			state = ORBIT;
		break;
		
		case INFINITE:
		break;

		
		default:
		break;
	}

}



int main()
{
	kilo_init();
	kilo_message_rx = message_rx;
	kilo_start(setup,loop);

	return 0;
}
