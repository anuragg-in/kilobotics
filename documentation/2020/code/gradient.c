#include <kilolib.h>

#define MESSAGE_CHECK_GRADIENT 0
#define DISTANCE_THRESHOLD_GRADIENT 1
#define TURN_LEFT 2
#define TURN_RIGHT 3
#define INITIALIZE 4 
#define THRESHOLD_GRADIENT 100
#define LEFT 100
#define RIGHT 150
#define MOTOR_ON_DURATION 800
#define TOTAL_NUM_COMMUNICATION 3

int received_msg, message_sent = 0, state = MESSAGE_CHECK_GRADIENT, distance, distance_gradient, message_rx_status = 0, count = 0, temp_unique_id, self_unique_id = 251, ref_unique_id;
message_t message;

void message_rx(message_t *m, distance_measurement_t *d)
{
    received_msg = (*m).data[0];
	message_rx_status = 1;
	distance_gradient = estimate_distance(d);
	/*set_color(RGB(1,0,1));
	delay(500);*/
}

void LED_color()
{
    set_color(RGB((self_unique_id % 2), ((self_unique_id / 2) % 2), ((self_unique_id / 4) % 2)));
    delay(1000);
}

message_t *message_tx()
{
    return &message;
}

void message_tx_success()
{
    message_sent = 1;
    set_color(RGB(1, 0, 1));
    delay(100);
    set_color(RGB(0, 0, 0));
    LED_color();
}



/*void move(int direction)
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
}*/

void setup()
{
            message.type = NORMAL;
            message.data[0] = self_unique_id;
            //message.data[0] = 1;
            message.crc = message_crc(&message);
            kilo_message_tx = message_tx;
            kilo_message_tx_success = message_tx_success;
            
}

void loop()
{
	switch(state)
	{
		/*case MESSAGE_CHECK:
			if(message_rx_status == 1)
			{
				message_rx_status = 0;	
				count++;
				if(temp_distance < distance)
				{
					distance = temp_distance;
				}
				if(count == TOTAL_NUM_COMMUNICATION)
				{
					count = 0;			
					state = DISTANCE_CHECK;
				}
			}
			
		break; 

		case DISTANCE_CHECK:
			if(distance>THRESHOLD)
			{
				state = TURN_RIGHT;

			}
			else
			{
				state = TURN_LEFT;
			}
			
		break;

		case TURN_LEFT:
			move(LEFT);
			state = INITIALIZE;			
		break;

		case TURN_RIGHT:
			move(RIGHT);
			state = INITIALIZE;			
		break;

		case INITIALIZE:
			distance = 1000;
			state = MESSAGE_CHECK;
		break; */
		
		case MESSAGE_CHECK_GRADIENT:
		/*set_color(RGB(0,0,1));
		delay(500);*/
		if(message_rx_status == 1)
		{
		    /*set_color(RGB(1,0,0));
		    delay(500);*/
		    message_rx_status = 0;
		    ref_unique_id = received_msg;
		    state = DISTANCE_THRESHOLD_GRADIENT;
		}
		else
		{
		    /*set_color(RGB(0,0,1));
		    delay(500);*/
		    state = MESSAGE_CHECK_GRADIENT;
		}
		break;

        case DISTANCE_THRESHOLD_GRADIENT:
        	//set_color(RGB(1,0,0));
        	//delay(500);
        if(distance_gradient < THRESHOLD_GRADIENT)
        {
            temp_unique_id = ref_unique_id + 1;
            if(temp_unique_id < self_unique_id)
            {
                set_color(RGB(0,0,1));
                self_unique_id = temp_unique_id;
                message.data[0] = self_unique_id;
                LED_color();
            }
        }
        /*else
        {
            set_color(RGB(1,0,1));
            delay(500);
        }*/
        state = MESSAGE_CHECK_GRADIENT;
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
