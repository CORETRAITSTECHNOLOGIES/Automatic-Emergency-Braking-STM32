#ifndef DATA_CONFIG_H
#define DATA_CONFIG_H
#define MAX_SPEED 120
#define MAX_DISTANCE_F 50
#define MAX_DISTANCE_R 100
#define MINIMUM_FRONT_DISTANCE 20
#define MINIMUM_FRONT_COLLISION_DISTANCE 5
#define MINIMUM_REAR_COLLISION_DISTANCE -5
#define TARGET_DISTANCE_PARTIAL_BRAKE 10
#define TARGET_DISTANCE_HARD_BRAKE 5
#define MINIMUM_REAR_DISTANCE -20
typedef struct Data_sense {
 int User_speed;
 int Current_Speed_Veh;
 int Vehicle_Position;
 int Obstacle_Position;
} Data_sense_t;
void Safe_Distance_Algo(Data_sense_t *Msg_data);
void update_speed(int speed);
bool detectCollision_Partial_brake(int vehiclePosition, int obstaclePosition);
bool detectCollision_Hard_Brake(int vehiclePosition, int obstaclePosition);
void Automatic_Braking(Data_sense_t *msg);
bool detectCollision_Rear_brake(int vehiclePosition, int obstaclePosition);
bool detectCollision_Rear_HARD_brake(int vehiclePosition, int obstaclePosition);
#endif
