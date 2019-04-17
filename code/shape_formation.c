#include <kilolib.h>

#define desired_distance 65
#define epsilon_margin 5
#define too_close 45
#define motor_delay 500
#define move_delay 0
#define threshold_rotate 12
/*#define number_communication 1*/

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

#define NEIGHBOURS_IN_RANGE 16
#define COMPARE_DESIRED_DISTANCE 17
#define ORBIT_AND_UPDATE_INDEX 18

int state, last_state, message_rx_status, temp, index=3, check = 0, x, y, max_index=2;
float distance, last_distance, min_distance;
int number_communication = 3;
int reception_id[3] = {0,0,0};
int reception_distance[3] = {0,0,0};
int neighbours[7][2] = {{0,0},{0,0},{0,0},{1,2},{2,3},{3,4},{4,5}};
float distance_multiplier[7][2] = {{0,0}, {0,0}, {0,0}, {1,1}, {1,1.4}, {1,1}, {1,1}};
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
    state = NEIGHBOURS_IN_RANGE;
    set_color(RGB(0,0,1));
}

void loop() 
{
    switch(state)
    {
        case NEIGHBOURS_IN_RANGE:
            //set_color(RGB(1,0,0));
            number_communication = 3;
            state = COMPARE_DESIRED_DISTANCE;
            message_rx_status = 0;
            break;
        case COMPARE_DESIRED_DISTANCE:
            if(message_rx_status == number_communication)
            {
                //set_color(RGB(1,1,0));
                for(int i=0; i<number_communication; i++)
                {
                    for(int j=i+1; j<number_communication; j++)
                    {
                        if(reception_id[i] == neighbours[index][0] && reception_id[j] == neighbours[index][1])
                        {
                            x = i;
                            y = j;
                            check = 1;
                            break;
                        }
                    }
                    if(check == 1)
                    {
                        break;
                    }
                }
                if(check == 1)
                {
                    if(reception_distance[x]>distance_multiplier[index][0]*desired_distance-epsilon_margin && reception_distance[x]<distance_multiplier[index][0]*desired_distance+epsilon_margin)
                    {
                        if(reception_distance[y]>distance_multiplier[index][1]*desired_distance-epsilon_margin && reception_distance[y]<distance_multiplier[index][1]*desired_distance+epsilon_margin)
                        {
                            state = FINISH;
                        }
                        else
                        {
                            state = ORBIT_AND_UPDATE_INDEX;
                        }
                    }
                    else
                    {
                        state = ORBIT_AND_UPDATE_INDEX;
                    }
                }
                else
                {
                    //set_color(RGB(1,0,0));
                    state = ORBIT_AND_UPDATE_INDEX;
                }
            }
            break;
        case ORBIT_AND_UPDATE_INDEX:
            //set_color(RGB(0,0,1));
            if(distance > desired_distance)
            {
                set_color(RGB(1,0,0));
                move(RIGHT);
                state = NEIGHBOURS_IN_RANGE;
            }
            else
            {
                set_color(RGB(0,0,1));
                move(LEFT);
                state = NEIGHBOURS_IN_RANGE;
            }
            //index=max_index+1;
            break;
        case FINISH:
            set_color(RGB(0,1,0));
            break;
        default:
            // SHOW ERROR ON LED
            set_color(RGB(1,0,0));
            delay(200);
            set_color(RGB(0,0,0));
            delay(200);
            set_color(RGB(1,0,0));
            delay(200);
            break;
        /*case MEASURE_DISTANCE:*/
            /*//measure distance function*/
            /*message_rx_status = 0;*/
            /*state = WAIT;*/
            /*break;*/
        /*case WAIT:*/
            /*if(message_rx_status == number_communication)*/
            /*{*/
                /*state = DISTANCE_CHECK_ORBIT;*/
            /*}*/
            /*break;*/
        /*case DISTANCE_CHECK_ORBIT:*/
            /*if(distance < too_close)*/
            /*{*/
                /*state = TOO_CLOSE;*/
                /*min_distance = distance;*/
            /*}*/
            /*else*/
            /*{*/
                /*if(distance > desired_distance)*/
                /*{*/
                    /*state = GREATER_THAN_DESIRED;*/
                /*}*/
                /*else*/
                /*{*/
                    /*state = SMALLER_THAN_DESIRED;*/
                /*}*/
            /*}*/
            /*break;*/
        /*case TOO_CLOSE:*/
            /*set_color(RGB(1,0,0));*/
            /*move(LEFT);*/
            /*// measure distance*/
            /*state = MEASURE_AGAIN;*/
            /*break;*/
        /*case MEASURE_AGAIN:*/
            /*message_rx_status = 0;*/
            /*state = WAIT_AGAIN;*/
            /*break;*/
        /*case WAIT_AGAIN:*/
            /*if(message_rx_status == number_communication)*/
            /*{*/
                /*state = UPDATE_MINIMUM_DISTANCE;*/
            /*}*/
            /*break;*/
        /*case UPDATE_MINIMUM_DISTANCE:*/
            /*if(distance < min_distance)*/
            /*{*/
                /*min_distance = distance;*/
            /*}*/
            /*if(distance - min_distance > threshold_rotate)*/
            /*{*/
                /*state = TOO_CLOSE_ESCAPE;*/
            /*}*/
            /*else*/
            /*{*/
                /*state = TOO_CLOSE;*/
            /*}*/
            /*break;*/
        /*case FINISH:*/
            /*set_color(RGB(0,1,0));*/
            /*break;*/
        /*case TOO_CLOSE_ESCAPE:*/
            /*set_color(RGB(0,0,1));*/
            /*if(distance > desired_distance - epsilon_margin)*/
            /*{*/
                /*state = MEASURE_DISTANCE;*/
            /*}*/
            /*else*/
            /*{*/
                /*move(FORWARD);*/
                /*state = MEASURE_ESCAPE;*/
            /*}*/
            /*break;*/
        /*case MEASURE_ESCAPE:*/
            /*message_rx_status = 0;*/
            /*state = WAIT_ESCAPE;*/
            /*break;*/
        /*case WAIT_ESCAPE:*/
            /*if(message_rx_status == number_communication)*/
            /*{*/
                /*state = TOO_CLOSE_ESCAPE;*/
            /*}*/
            /*break;*/
        /*case GREATER_THAN_DESIRED:*/
            /*move(RIGHT);*/
            /*set_color(RGB(0,1,0));*/
            /*state = MEASURE_DISTANCE;*/
            /*break;*/
        /*case SMALLER_THAN_DESIRED:*/
            /*move(LEFT);*/
            /*set_color(RGB(1,0,0));*/
            /*state = MEASURE_DISTANCE;*/
            /*break;*/
        /*default:*/
            /*break;*/
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
        reception_id[message_rx_status] = (*m).data[0];
        if(reception_id[message_rx_status]>max_index)
        {
            max_index=reception_id[message_rx_status];
        }
        reception_distance[message_rx_status] = temp;
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
