#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Bool.h>
#include <sensor_msgs/LaserScan.h>
#include <cmath>

ros::Publisher cmd_vel_pub;
ros::Subscriber odom_sub;
ros::Subscriber scan_sub;

geometry_msgs::Pose current_pose;
geometry_msgs::Twist cmd_vel;
float lidarMinDistance;
int angleMinDistance;
int angleTolerance = 20;
bool is_wall_ahead = false;
bool end = false;
double vel_linear_avance = 0.4;
double vel_linear_freno = 0.1;
double vel_linear_arranque = 0.2;
double vel_angular_noventa = 2;
double vel_angular_esquina = -0.85;
double vel_angular_avance = -0.2;


void odomCallback(const nav_msgs::Odometry::ConstPtr& msg);
void scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg);
void rotate_90_degrees(void);
void run(void);

void odomCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    // Obtener la pose actual del robot
    current_pose = msg->pose.pose;
}

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg) {
    // Obtener las distancias y las intensidades en los angulos del Sensor LIDAR
    std::vector<float> lidarData = msg->ranges;
    std::vector<float> lidarIntensities = msg->intensities;
    
    // Verificar si hay obstaculo delante (rango de verificación: -10° a 10°)
    for(int i = 350; i < lidarData.size(); i++){
    	if(lidarData[i] < 0.5){ 
    	    is_wall_ahead = true;
            // Verificar si la pared que esta delante es un objeto reflectante (fin del laberinto)
            if(lidarIntensities[i]>0) end = true; 
    	    break;
    	}
    }
    if(!is_wall_ahead){
    	for(int i = 0; i < 10; i++){
    	    if(lidarData[i] < 0.5){
    	        is_wall_ahead = true;
                // Verificar si la pared que esta delante es un objeto reflectante (fin del laberinto)
                if(lidarIntensities[i]>0) end = true; 
    	        break;
    	    }
    	}
    }
    
    // Acercarse a pared mas cercana mientras se avanza
    cmd_vel.angular.z = vel_angular_avance;
    cmd_vel_pub.publish(cmd_vel);
    
    // Si lo que hay a la derecha del robot está "lejos"
    if(lidarData[280] > 0.9){
        //Aumentar velocidad angular para doblar en esquina
    	cmd_vel.angular.z = vel_angular_esquina;
    	cmd_vel_pub.publish(cmd_vel);
    	ros::Duration(0.4).sleep();
    }
    
}

void rotate_90_degrees(){
    // Frenar robot
    geometry_msgs::Twist cmd_vel_local;
    cmd_vel_local.linear.x = vel_linear_freno;
    cmd_vel_pub.publish(cmd_vel_local);
    
    // Rotar 90 grados
    cmd_vel_local.angular.z = vel_angular_noventa;
    cmd_vel_pub.publish(cmd_vel_local);
    ros::Duration(2.0).sleep();
    cmd_vel_local.angular.z = 0;
    cmd_vel_pub.publish(cmd_vel_local);
    
    // Arrancar robot
    cmd_vel_local.linear.x = vel_linear_arranque;
    cmd_vel_pub.publish(cmd_vel_local);
    is_wall_ahead = false;
}

void run() {
    ROS_INFO_STREAM("Entrando run");
    // Loop hasta llegar al final del laberinto o que el master muera
    while (ros::ok() && !end) {
        // Setear la velocidad linear en el eje x y publicarla
        cmd_vel.linear.x = vel_linear_avance;
        cmd_vel_pub.publish(cmd_vel);
        // Comprobar si hay pared delante
        if(is_wall_ahead){
            // Si hay pared delante rotar 90° grados
            rotate_90_degrees();
        }
        ros::Duration(0.1).sleep();
        // Esperar interrupciones de los topicos de Odom o Scan
        ros::spinOnce();
    }
    if(end) ROS_INFO_STREAM("Recorrido Finalizado! Congrats!");
}  

int main(int argc, char** argv) {
    // Inicializar diffbot_navigator
    ros::init(argc, argv, "diffbot_navigator");
    // Inicializar nodo de ROS
    ros::NodeHandle nh;

    // Crear el publicador para la velocidad 
    cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/diffbot/mobile_base_controller/cmd_vel", 10);

    // Crear los suscriptores para la odometria y el LIDAR
    odom_sub = nh.subscribe("/diffbot/mobile_base_controller/odom", 10, &odomCallback);
    scan_sub = nh.subscribe("/diffbot/scan", 10, &scanCallback);

    ROS_INFO_STREAM("Entrando main");
    run();

    return 0;
}

