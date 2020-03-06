#include <kilolib.h>

#define MESSAGE_CHECK_EF 0
#define ID_CHECK 1
#define ORBIT 2
#define TURN_LEFT 3
#define TURN_RIGHT 4
#define DISTANCE_CHECK 5
#define INITIALIZE 6
#define INFINITE 7
#define INITIALIZE_GRADIENT 8
#define MESSAGE_CHECK_GRADIENT 9
#define DISTANCE_THRESHOLD_GRADIENT 10

#define THRESHOLD_VALUE_GRADIENT 70
#define THRESHOLD 50
#define LEFT 100
#define RIGHT 150
#define MOTOR_ON_DURATION 500
#define TOTAL_NUM_COMMUNICATION 15
#define TOTAL_NUM_COMMUNICATION_ORBIT 3
#define TOTAL_NUM_COMMUNICATION_GRADIENT 20

int received_msg, distance_gradient, message_sent = 0, temp_unique_id, neighbor_id, state = INITIALIZE_GRADIENT, temp_distance, distance, message_rx_status = 0, count = 0, self_unique_id = 251;
message_t message;

void message_rx(message_t *m, distance_measurement_t *d)
{
    received_msg = (*m).data[0];
	message_rx_status = 1;
	temp_distance = estimate_distance(d);
	distance_gradient = estimate_distance(d);
	/*set_color(RGB(1,0,1));
	delay(500);*/
}

void LED_color()
{
    switch (self_unique_id) 
  {
    case 1: set_color(RGB(1,0,0));
    break;
    case 2: set_color(RGB(0,1,0));
    break;
    case 3: set_color(RGB(0,0,1));
    break;
    case 4: set_color(RGB(1,1,0));
    break;
    case 5: set_color(RGB(1,0,1));
    break;
    case 6: set_color(RGB(0,1,1));
    break;
    case 7: set_color(RGB(1,1,1));
    break;
    default: set_color(RGB(0,0,0));
    break;
  }
  delay(1000);
}

message_t *message_tx()
{
    return &message;
}


void message_tx_success()
{
    message_sent = 1;
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
	    case INITIALIZE_GRADIENT:
        message_rx_status = 0;
        //state = MESSAGE_CHECK_GRADIENT;
        if(count <= TOTAL_NUM_COMMUNICATION_GRADIENT)
        {
            state = MESSAGE_CHECK_GRADIENT;
        }
        else
        {
            count = 0;
            state = MESSAGE_CHECK_EF;
            //state = INFINITE;
        }
        break;

		case MESSAGE_CHECK_GRADIENT:
		if(message_rx_status == 1)
		{
		   state = DISTANCE_THRESHOLD_GRADIENT;
		   //state = INFINITE;
		   count++;
		}
		else
		{
		    state = INITIALIZE_GRADIENT;
		}
		break;

        case DISTANCE_THRESHOLD_GRADIENT:
        //state = INFINITE;
        if(distance_gradient < THRESHOLD_VALUE_GRADIENT)
        {   
            temp_unique_id = received_msg + 1;
            if(temp_unique_id < self_unique_id)
            {
                //set_color(RGB(0,0,1));
                self_unique_id = temp_unique_id;
                message.data[0] = self_unique_id;
                message.crc = message_crc(&message);
                LED_color();
            }
        }
        state = INITIALIZE_GRADIENT;
        break;
        
        case INFINITE:
        set_color(RGB(1,1,1));
        break;
        
		case MESSAGE_CHECK_EF:
			if(message_rx_status == 1)
			{
				message_rx_status = 0;	
				count++;
				if(received_msg > neighbor_id)
				{
					neighbor_id = received_msg;
				}
				if(count == TOTAL_NUM_COMMUNICATION)
				{
					count = 0;			
					state = ID_CHECK;
				}
			}
			
		break; 

		case ID_CHECK:
			if(self_unique_id > neighbor_id)
			{
				//set_color(RGB(1,0,0));
				state = ORBIT;
			
				//state = INFINITE;
			}
			else
			{
			    neighbor_id = 0;
				state = MESSAGE_CHECK_EF;
			}
			
		break;
		
		case ORBIT:
			if(message_rx_status == 1)
			{
				message_rx_status = 0;	           //to identify the next message from any of the star
				count++;
				if(temp_distance < distance)          //to find which star is nearer to the planet
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
