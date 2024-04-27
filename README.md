### INTRODUCTION
The project aims to develop a comprehensive hardware and software system to complement the operation of the IIT Delhi Campus Bus System. The system consists of Bus-Mounted Communication Units and Bus Stop Mounted Display Units, and displays an online estimate of the waiting time at each stop.

### FEATURES: 
- *Online Waiting Time Display:* at each stop 
- *Out of Service Display:* displayed at the stops when buses are out of service

### QUICKSTART GUIDE
Each bus and each stop has an ESP Module with an integrated power-on switch, with indicators of power and network connection. Wi-Fi is pre-established and provided to the system for the initial setup, during which time the system syncs to the internet. As soon as the system is done syncing and the bus driver provides his input(through a button on the Bus-Module), the display timers of all stops sync and begin displaying their estimates calculated as mentioned below. 

### TIMER IMPLEMENTATION:
#### Online Wait-Time Display
The online wait-time display is implemented as follows: 
- Through ground research, we have the values X_{i,i+1} for the travel time between adjacent stops i and i+1, and pre-computed the values for each pair of stops i and j. We have also collected estimates for the loading times L_{i} at each stop. 
- Bus Stops communicate with each other through Wi-Fi, and buses communicate with the stops when within 100m range of the stop. 
- When bus B reaches within 100m of a stop, the timer at that stop is reset to 1. When the bus reaches the stop, its timer is set to 0, and every other stop that has B as its next arriving bus gets its timer updated.
- When a bus B leaves the stop i, every display at stop j (whose next arriving bus is also B) has its timer re-set to X_{i,j}+L_j
#### Unexpected Delays
A Bus being delayed is detected when the timer at any stop reaches lower than 1 minute before the bus has reached. The timer is extended by another minute in this case, and then the units at all the stops are updated to display the new timings. 
#### Out of Service Message
A Bus going out of service is detected through an input provided to the bus driver through the bus-mounted unit. In case the input is provided, all the stops display their out of service messages.

### ACCURACY
The displays have their least counts in terms of seconds, and the accuracy of the system is up to a minute. The out-of-service message displays before a maximum of 3 minutes after the bus stops.