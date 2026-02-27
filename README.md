# WEBSERVER-ESP32WROOM

AVIOT: IoT Instrumentation System for Environmental Monitoring and Control in Poultry Farms
This project is part of the Information System to Optimize Production Processes at Paicol Farm (SIOP) by SENA-CDATH. Its goal is to implement automated monitoring and control of environmental variables to enhance productivity and animal welfare.

👥 Instructors & Contributors
Adrian Fernando Chavarro

Octavio Clavijo

Smylle Leonardo Alvarado

Ruben Dario Ramirez Cuellar

Julio Florez de la Hoz

Last Updated: December 18, 2025

📝 Project Context
The Problem
Productivity at the Paicol Farm is hindered by factors such as heat stress, poor water/bedding conditions, and the physical distance from the training center. These factors necessitate timely actions to optimize technical assistance and improve agricultural practices for both instructors and apprentices within the SENA-CDATH productive environments.

The Proposal
To implement an information system that integrates a network of smart sensors and actuators to maintain optimal conditions automatically. The project focuses on interdisciplinarity, scalability, and open-source infrastructure to develop solutions applied to the productive sector.

🏗️ System Architecture (The Heart of the Project)
The system is divided into three functional layers:

Perception Layer: Sensor nodes that capture physical variables (Air Temperature, Air Humidity, Bedding Humidity, Temperature-Humidity Index (THI), and Water pH).

Network Layer: Gateway for local processing and data transmission to the cloud.

Application Layer: A dashboard that facilitates data visualization and informed decision-making.

📊 Instrumentation Variables
The following variables are measured to optimize the farm environment:

Variable	Suggested Sensor	Ideal Range
Temperature	DHT11	24°C - 32°C
Relative Humidity	DHT11	50% - 70%
THI (Temp-Hum Index)	Indirect Measurement	< 110
Bedding Humidity	Soil Moisture Sensor	< 10
Water pH	pH Sensor	6.0 - 6.8

Exportar a Hojas de cálculo

🛠️ Technical Development & Connectivity
The tools used for development include:

Microcontroller: ESP32 (WiFi Module).

Communication Protocols: * Sensors: RS485 Modbus.

IoT: TCP/IP for Webserver on ESP32.

Visualization: Dashboard developed in HTML and CSS.

Storage: Local real-time database (with future expansion plans).

🤖 Control Algorithm (Automation)
The control logic is based on predefined conditions:

Heat Stress: If THI > 110, trigger alarms.

Climate Control: If Temperature > 30°C, turn on exhaust fans and the misting system.

Water Quality: If pH > 7, trigger alarms for immediate action.

🚀 Future Work
The system aims to evolve according to user needs:

Role-based Login: Access levels for Admin, User, and Visitor.

Database Scaling: Transitioning to MySQL for higher storage capacity.

GUI Enhancements: Real-time data reading and improved user interface per role.

Data Analytics: Features for exporting data and analytical tools.

Knowledge Transfer: Production of guides, articles, and open-source code for the apprentice and instructor community.

✅ Expected Results & Conclusions
Cost & Mortality Reduction: Optimization of energy and water usage.

Animal Welfare: Reduction of diseases caused by heat stress and gastrointestinal issues.

Scalability: The system can grow to cover multiple sheds from a single central hub.

Multidisciplinarity: Involvement of apprentices from various technical areas in field-based technological implementation.

SENA-CDATH - 2026

