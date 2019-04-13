#include <kilolib.h>

#define desired_distance 65
#define epsilon_margin 5
#define too_close 45
#define motor_delay 500
#define move_delay 0
#define threshold_rotate 12
#define number_communication 1

#define FORWARD 0
#define LEFT 1
#define RIGHT 2

#define MEASURE_DISTANCE 0
#define DISTANCE_CHECK_ORBIT 1
#define TOO_CLOSE 2
#define TOO_CLOSE_ESCAPE 4
#define GREATER_THAN_DESIRED 5
#define SMALLER_THAN_DESIRED 6
#define WAIT 7
#define MEASURE_AGAIN 8
#define WAIT_AGAIN 9
#define UPDATE_MINIMUM_DISTANCE 10
#define MEASURE_ESCAPE 11
#define WAIT_ESCAPE 12
#define FINISH 15

int state, last_state, distance, last_distance, message_rx_status, min_distance, temp;
message_t message;

void measure_distance()
{
    message_rx_status=0;
}

message_t *message_tx()
{
    return &message;
}

void move(int direction)
{
    switch(direction)
    {
        case FORWARD:
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(motor_delay);
            set_motors(0, 0);
            delay(move_delay);
            break;
        case LEFT:
            spinup_motors();
            set_motors(kilo_straight_left, 0);
            delay(motor_delay);
            set_motors(0, 0);
            delay(move_delay);
            break;
        case RIGHT:
            spinup_motors();
            set_motors(0, kilo_straight_right);
            delay(motor_delay);
            set_motors(0, 0);
            delay(move_delay);
            break;
    }
}

void setup() 
{
    message.type = NORMAL;
    message.data[0] = 0;
    message.crc = message_crc(&message);
    state = MEASURE_DISTANCE;
    set_color(RGB(0,0,1));
}

void loop() 
{
    switch(state)
    {
        case MEASURE_DISTANCE:
            //measure distance function
            message_rx_status = 0;
            state = WAIT;
            break;
        case WAIT:
            if(message_rx_status == number_communication)
            {
                state = DISTANCE_CHECK_ORBIT;
            }
            break;
        case DISTANCE_CHECK_ORBIT:
            if(distance < too_close)
            {
                state = TOO_CLOSE;
                min_distance = distance;
            }
            else
            {
                if(distance > desired_distance)
                {
                    state = GREATER_THAN_DESIRED;
                }
                else
                {
                    state = SMALLER_THAN_DESIRED;
                }
            }
            break;
        case TOO_CLOSE:
            set_color(RGB(1,0,0));
            move(LEFT);
            // measure distance
            state = MEASURE_AGAIN;
            break;
        case MEASURE_AGAIN:
            message_rx_status = 0;
            state = WAIT_AGAIN;
            break;
        case WAIT_AGAIN:
            if(message_rx_status == number_communication)
            {
                state = UPDATE_MINIMUM_DISTANCE;
            }
            break;
        case UPDATE_MINIMUM_DISTANCE:
            if(distance < min_distance)
            {
                min_distance = distance;
            }
            if(distance - min_distance > threshold_rotate)
            {
                state = TOO_CLOSE_ESCAPE;
            }
            else
            {
                state = TOO_CLOSE;
            }
            break;
        case FINISH:
            set_color(RGB(0,1,0));
            break;
        case TOO_CLOSE_ESCAPE:
            set_color(RGB(0,0,1));
            if(distance > desired_distance - epsilon_margin)
            {
                state = MEASURE_DISTANCE;
            }
            else
            {
                move(FORWARD);
                state = MEASURE_ESCAPE;
            }
            break;
        case MEASURE_ESCAPE:
            message_rx_status = 0;
            state = WAIT_ESCAPE;
            break;
        case WAIT_ESCAPE:
            if(message_rx_status == number_communication)
            {
                state = TOO_CLOSE_ESCAPE;
            }
            break;
        case GREATER_THAN_DESIRED:
            move(RIGHT);
            set_color(RGB(0,1,0));
            state = MEASURE_DISTANCE;
            break;
        case SMALLER_THAN_DESIRED:
            move(LEFT);
            set_color(RGB(1,0,0));
            state = MEASURE_DISTANCE;
            break;
        default:
            break;
    }
    last_state = state;
}


void message_rx(message_t *m, distance_measurement_t *d)
{
    if(message_rx_status == 0)
    {
        distance = 1000;
    }
    if(message_rx_status != number_communication)
    {
        temp = estimate_distance(d);
        if(temp < distance)
        {
            distance = temp;
        }
        message_rx_status++;
    }
}

int main() 
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);

    return 0;
}
