PROYECTO FINAL DE CURSADA. INTRODUCCIÓN A LA ROBÓTICA. FACULTAD DE INGENIERÍA. UNIVERISDAD NACIONAL DE LA PLATA

Profesor adjunto: Dr. Rosendo Juan Luis
Jefe de Trabajos Prácticas: Ing. Leonardo Navarría

Grupo de Alumnos: - Ceccato, Matias
                  - Barbosa, Agustin

Este trabajo se basa en el proyecto diffbot de ROS 1.

Inicialmente, la carpeta "db_world" debe ser copiada en la siguiente ruta: catkin_ws/diffbot/diffbot_gazebo/models. Allí se debe reemplazar la carpeta para incluir el laberinto del proyecto

Para arrancar el proyecto, abrir tres terminales:

En la primera:  roslaunch diffbot_gazebo diffbot.launch

En la segunda:  roslaunch diffbot_slam diffbot_slam.launch slam_method:=gmapping

En la tercera:  cd catkin_ws   //Raiz del proyecto
                rosrun tpRos app

Presentación de Google Slides: https://docs.google.com/presentation/d/1WJ6VF20pD4adq78i5wzH47m_MTvOtlJXvvtRNJSYjbI/edit?usp=sharing
