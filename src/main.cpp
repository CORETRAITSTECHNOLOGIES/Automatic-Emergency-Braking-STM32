#include <mbed.h>
#include<math.h>
#include "Config_Pin.h"
#include "Data_Config.h"
/*Thread for Iginition*/
Thread Ignition;
/*Thread for Engine */
Thread Engine;
/*Thread for Event control*/
Thread event_thread; 
/*Thread for Data Capture*/
Thread Data_Sense;
/*Thread for Controlling and Handling */
Thread Control_Sense;
/*Engine Ignition Control and Control*/
InterruptIn Engine_control_Interrupt(BUTTON1);
/*Engine Parking Mode Enabler and Disabler*/
InterruptIn Parking_Interrupt(Button2);
/*Ignition Led*/
DigitalOut Iginition_led(LED1);
/*Parking Indication Led*/
DigitalOut PARK_LED(D3);
/*Speed Indication lead*/
PwmOut SPEED_INDICATION(D6);
/*Indication of Engine*/
DigitalOut ENGINE_led(LED2);
/*Input for Vehicle speed*/
AnalogIn   VSelspeed(ALED_5);
/*Input for Vehicle Position*/
AnalogIn   Vpos(ALED_3);
/*Input for Obstacle  Position*/
AnalogIn   Opos(ALED_4);
/*Event Queue for Events*/
EventQueue queue(32 * EVENTS_EVENT_SIZE);
/*State Count*/
int count_y=0;
/* MAil for Data exchange*/
Mail<Data_sense_t, 6> Data_queue;
/*Indication to driver */
BusOut driver_led(D5,D9,D10,D11,D2,D4);
/*Indication to driver  in parking mode*/
BusOut Park_led(D10,D9,D11);
/* Sense flag*/
bool SENSE_FLAG = false;
/* Conntrol  flag*/
bool CONTROL_FLAG = false;
/*Braking Flag*/
bool BRAKING_FLAG = false;
/*Parking flag*/
bool PARKING_FLAG = false;
/*Engine flag*/
bool ENGINE_FLAG = false;
/* Current speed Global*/
 int VSpeed;

/* Feed back to Driver*/
void feedback_to_driver(int mask)
{
    driver_led = mask;
    ThisThread::sleep_for(250);
}
/* Feed back to Driver for parking*/
void parking_indication(int mBit)
{
       Park_led = mBit;
    ThisThread::sleep_for(250);
}
/* Handler when parking is ENABLED*/
void Parking_state()
{
PARKING_FLAG = !PARKING_FLAG;
PARK_LED = !PARK_LED;
}
/* Fetching Data */
void Sensing_Data()
{
   int Speed =0;
   int Front_Distance=0;
   int Rear_Distance =0;
   while (true)
   {
if(SENSE_FLAG)
{
      float sel_speed = VSelspeed.read();
      Speed = round(sel_speed * MAX_SPEED);
     if(Speed > 0 )
     {
      Front_Distance  = round(.4166 * Speed);
     }
      float Rear_Distance_V = Opos.read();
      Rear_Distance = round(Rear_Distance_V * MAX_DISTANCE_R);
      queue.call(printf,"current vehicle speed:  %d\n",VSpeed);
      queue.call(printf,"Selected vehicle speed: %d\n",Speed);
      queue.call(printf, "Vehicle Position: %d\n",Front_Distance);
      queue.call(printf,"Obstacle distance:  %d\n",Rear_Distance);
      queue.call(printf,"<------------------------------------------>\n");
      if(!Data_queue.full())
      {
         Data_sense_t *Data_msg = Data_queue.try_alloc_for(Kernel::wait_for_u32_forever); 
         Data_msg->Current_Speed_Veh = VSpeed;
         Data_msg->User_speed = Speed;
         Data_msg->Vehicle_Position =Front_Distance;
         Data_msg->Obstacle_Position = Rear_Distance;
         Data_queue.put(Data_msg);
      }
}
ThisThread::sleep_for(1000ms);
   }
}
/*Control function for Handling with Input Data */
void Control()
{
   while(true){
     if(CONTROL_FLAG)
     {
       queue.call(printf, "receiving from Queue\n");
       Data_sense_t *msg_data_r = Data_queue.try_get_for(Kernel::wait_for_u32_forever);
       queue.call(printf, "Read from Queue Complete\n"); 
       if(PARKING_FLAG == true)
       {
         queue.call(printf,"-->Parking Mode:\n");
         if(detectCollision_Rear_brake(msg_data_r->Vehicle_Position,msg_data_r->Obstacle_Position))
         {
           VSpeed = .05 * msg_data_r->User_speed;
           update_speed(VSpeed);
           queue.call(printf,"-------------------------\n");
           queue.call(printf,"Oject near:\n");
           queue.call(printf,"-------------------------\n");
           queue.call(parking_indication, 1);
         }
         else
         {
           VSpeed = .5*msg_data_r->User_speed;
           update_speed(VSpeed);
           queue.call(feedback_to_driver, 2);
           queue.call(printf,"-------------------------\n");
           queue.call(printf,"No object in rear:\n");
           queue.call(printf,"-------------------------\n");
         }
         if(detectCollision_Rear_HARD_brake(msg_data_r->Vehicle_Position,msg_data_r->Obstacle_Position))
         {
           SENSE_FLAG =false;
           queue.call(parking_indication, 4);
           Thread Braking_Sense;
           Braking_Sense.start([&]() {Automatic_Braking(msg_data_r);});
           Braking_Sense.set_priority(osPriorityHigh);
           Braking_Sense.join();
           queue.call(printf,"-------------------------\n");
           queue.call(printf,"Eergency Brake Applied:\n");
           queue.call(printf,"-------------------------\n");
         }
         Data_queue.free(msg_data_r);
       }
       else
       {
         if (detectCollision_Partial_brake(msg_data_r->Vehicle_Position,msg_data_r->Obstacle_Position))
         {
           if(msg_data_r->User_speed > 10)
           {
             int distance = msg_data_r->Obstacle_Position - msg_data_r->Vehicle_Position;
             distance = 2*distance;
             VSpeed = msg_data_r->User_speed -distance;
             update_speed(VSpeed);
             queue.call(feedback_to_driver,1 );
             queue.call(printf,"-------------------------\n");
             queue.call(printf,"Apply Partial Brake:\n");
             queue.call(printf,"-------------------------\n");
           }
           else
           {
             VSpeed  = msg_data_r->User_speed;
             update_speed(VSpeed);
             queue.call(feedback_to_driver, 2);
             queue.call(printf,"-------------------------\n");
             queue.call(printf,"-->Speed slow Object near:\n");
             queue.call(printf,"-------------------------\n");
           }
         }
         else
         {
           VSpeed  = msg_data_r->User_speed;
           update_speed(VSpeed);
           queue.call(feedback_to_driver, 2);
           queue.call(printf,"-------------------------\n");
           queue.call(printf,"-->Manual speed:\n");
           queue.call(printf,"-------------------------\n");
         }
         if(detectCollision_Hard_Brake(msg_data_r->Vehicle_Position,msg_data_r->Obstacle_Position))
         {
           if(msg_data_r->User_speed > 2 )
           {
             SENSE_FLAG =false;
             Thread Braking_Sense;
             Braking_Sense.start([&]() {Automatic_Braking(msg_data_r);});
             Braking_Sense.set_priority(osPriorityHigh);
             Braking_Sense.join();
           }
         }
         if (msg_data_r->User_speed == msg_data_r->Current_Speed_Veh)
         {
           queue.call(feedback_to_driver, 16);
           queue.call(printf," Speed Adjusted\n");
         }
         Data_queue.free(msg_data_r);
         ThisThread::sleep_for(250ms);
       }
     }
     ThisThread::sleep_for(250ms);
   }
}
void update_speed(int speed)
{
  SPEED_INDICATION = .001*speed;
}
bool detectCollision_Partial_brake(int vehiclePosition, int obstaclePosition) {
    int distance = obstaclePosition - vehiclePosition;
    return (distance <= TARGET_DISTANCE_PARTIAL_BRAKE);
}
bool detectCollision_Hard_Brake(int vehiclePosition, int obstaclePosition) {
    int distance = obstaclePosition - vehiclePosition;
    return (distance <= TARGET_DISTANCE_HARD_BRAKE);
}
bool detectCollision_Rear_brake(int vehiclePosition, int obstaclePosition) {
    int distance = obstaclePosition - vehiclePosition;
    return (distance <= TARGET_DISTANCE_PARTIAL_BRAKE);
}
bool detectCollision_Rear_HARD_brake(int vehiclePosition, int obstaclePosition) {
    int distance = obstaclePosition - vehiclePosition;
    return (distance <= TARGET_DISTANCE_HARD_BRAKE);
}
void Automatic_Braking(Data_sense_t *msg)
{
  while((msg->User_speed > 0) && CONTROL_FLAG == true)
  {
    update_speed(msg->User_speed);
    msg->User_speed -=5;
    VSpeed = msg->User_speed;
    queue.call(feedback_to_driver,4);
    queue.call(printf,"Applied Hard Brake:\n");
    ThisThread::sleep_for(250ms);
  }
  if(CONTROL_FLAG == true)
  {
    queue.call(printf,"Existing Braking\n");
    SENSE_FLAG = true;
  }
}
void state_handling()
{
   count_y++;
   switch(count_y)
   {
    case 1:Ignition.flags_set(0x42); break;
    case 2:Ignition.flags_set(0x43); break;
    case 3:Ignition.flags_set(0x44); break;
    default:
      Ignition.flags_set(0x00);
      count_y=0;
   }
}
void Engine_ignition()
{
int Ignition_cnt=0;
Iginition_led= 0;
while(true)
{
 if (ThisThread::flags_wait_any(0x42))
 {
    printf("Ignition_on\n");
    Iginition_led = 1;
    queue.call(feedback_to_driver,0 );
    Ignition_cnt++;
    ThisThread::sleep_for(100ms);
 }
 if(ThisThread::flags_wait_any(0x43))
 {
    ENGINE_led = 1;
    ThisThread::sleep_for(100ms);
    printf("Engine_Running\n");
    SENSE_FLAG =true;
    CONTROL_FLAG = true;
    ENGINE_FLAG =true;
    Ignition_cnt++;
    ThisThread::sleep_for(100ms);
 }
 if(ThisThread::flags_wait_any(0x44))
 {
    ThisThread::sleep_for(100ms);
    printf("Engine_Stoped\n");
    printf("Ignition_off\n");
    queue.call(feedback_to_driver,1 );
    SENSE_FLAG =false;
    CONTROL_FLAG =false;
    ENGINE_FLAG =false;
    SPEED_INDICATION =0;
    PARK_LED =0;
    Iginition_led = 0;
    ENGINE_led =0;
    Ignition_cnt=0;
 }
}
}
int main()
{
  Engine_control_Interrupt.rise(&state_handling);
  Parking_Interrupt.rise(&Parking_state);
  event_thread.start(callback(&queue, &EventQueue::dispatch_forever));
  Ignition.start(Engine_ignition);
  Data_Sense.start(Sensing_Data);
  Control_Sense.start(Control);
  thread_sleep_for(osWaitForever);
  return 0;
}
