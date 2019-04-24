#include <kilolib.h>

//----- PARAMETERS FOR SHAPE FORMATION / COMMUNICATION ----- 
#define DESIRED_DISTANCE 65
#define EPSILON_MARGIN 5
#define TOO_CLOSE_DISTANCE 45
#define MOTOR_ON_DURATION 500
#define THRESHOLD_ROTATE 12
#define NUMBER_COMMUNICATION 3

//----- DEFINE MOTION ----- 
#define FORWARD 0
#define LEFT 1
#define RIGHT 2

//----- DEFINE STATE ----- 
#define MEASURE_DISTANCE 0
#define DISTANCE_CHECK_ORBIT 1
#define TOO_CLOSE 2
#define TOO_CLOSE_ESCAPE 3
#define GREATER_THAN_DESIRED 4
#define SMALLER_THAN_DESIRED 5
#define WAIT 6
#define MEASURE_AGAIN 7
#define WAIT_AGAIN 8
#define UPDATE_MINIMUM_DISTANCE 9
#define MEASURE_ESCAPE 10
#define WAIT_ESCAPE 11
#define FINISH 12

//----- VARIABLE DECLARATION -----
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

//----- ROUTINE FOR RECEPTION -----
void message_rx(message_t *m, distance_measurement_t *d)
{
    if(message_rx_status == 0)
    {
        distance = 1000;
    }
    if(message_rx_status != NUMBER_COMMUNICATION)
    {
        temp = estimate_distance(d);
        if(temp < distance)
        {
            distance = temp;
        }
        message_rx_status++;
    }
}

//----- ROUTINE FOR MOTION -----
void move(int direction)
{
    switch(direction)
    {
        case FORWARD:
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            delay(MOTOR_ON_DURATION);
            set_motors(0, 0);
            break;
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
    }
}

void setup() 
{
    //----- RESET FINITE STATE MACHINE -----
    state = MEASURE_DISTANCE;
    set_color(RGB(0,0,1));
}

void loop() 
{
    switch(state)
    {
        case MEASURE_DISTANCE:
            //----- INITIATE RECEPTION -----
            message_rx_status = 0;
            state = WAIT;
            break;
        case WAIT:
            if(message_rx_status == NUMBER_COMMUNICATION)
            {
                state = DISTANCE_CHECK_ORBIT;
            }
            break;
        case DISTANCE_CHECK_ORBIT:
            //----- CHECK THE REGION OF PLANET -----
            if(distance < TOO_CLOSE_DISTANCE)
            {
                state = TOO_CLOSE;
                min_distance = distance;
            }
            else
            {
                if(distance > DESIRED_DISTANCE)
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
            state = MEASURE_AGAIN;
            break;
        case MEASURE_AGAIN:
            message_rx_status = 0;
            state = WAIT_AGAIN;
            break;
        case WAIT_AGAIN:
            if(message_rx_status == NUMBER_COMMUNICATION)
            {
                state = UPDATE_MINIMUM_DISTANCE;
            }
            break;
        case UPDATE_MINIMUM_DISTANCE:
            //----- UPDATE MINIMUM DISTANCE WHILE TRYING TO ESCAPE THE OBSTACLE -----
            if(distance < min_distance)
            {
                min_distance = distance;
            }
            //----- CHECK IF PLANET IS ORIENTED AWAY FROM OBSTACLE -----
            if(distance - min_distance > THRESHOLD_ROTATE)
            {
                state = TOO_CLOSE_ESCAPE;
            }
            else
            {
                state = TOO_CLOSE;
            }
            break;
        case TOO_CLOSE_ESCAPE:
            //----- CHECK IF PLANET IS NEAR THE ORBITING DISTANCE -----
            if(distance > DESIRED_DISTANCE - EPSILON_MARGIN)
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
            if(message_rx_status == NUMBER_COMMUNICATION)
            {
                state = TOO_CLOSE_ESCAPE;
            }
            break;
        case GREATER_THAN_DESIRED:
            //----- ROUTINE FOR CLOCKWISE ORBITING -----
            move(RIGHT);
            set_color(RGB(0,1,0));
            state = MEASURE_DISTANCE;
            break;
        case SMALLER_THAN_DESIRED:
            //----- ROUTINE FOR CLOCKWISE ORBITING -----
            move(LEFT);
            set_color(RGB(1,0,0));
            state = MEASURE_DISTANCE;
            break;
        default:
            break;
    }
    last_state = state;
}



int main() 
{
    kilo_init();
    //----- INITIALIZE RECEPTION -----
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);

    return 0;
}
