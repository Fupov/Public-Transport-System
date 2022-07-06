# Public-Transport-Simulator
> Public transport simulator
## Requirement

Pour linux et Mac OS : 

```sh
gcc -Wall main.c -o main -pthread
./main passagers.txt
```

## Description: Bus and metro route

The system to be simulated is composed of a bus route and a metro route. These routes consist of 5 bus stations (stations 0 to 4) and 3 metro stations (stations 5 to 7) in which users can get off, get on or simply wait for the bus or the metro. As shown in Figure 1, the bus keeps going around stations 0 to 4 (0, 1, 3, 4, 0, 1, ...) while the metro goes through stations 5 to 7 and then returns to the opposite direction (5, 6, 7, 6, 5, ... ). Stations 0 and 5 allow passengers to transfer between metro and bus routes. Note that a passenger who uses the bus to get to station 5, arrives at his destination when the bus drops him off at station 0. The same is true for the passenger who uses the metro to get to station 0. The vehicles (bus and metro) each have a maximum capacity (8 passengers for the train and 5 passengers for the bus). No passenger may board a vehicle when the latter has reached its maximum capacity. In such a case, the passenger will have to wait for the next passage of the vehicle (in the right direction) or call a taxi.

## Added taxis

Now suppose that 3 taxis are at the disposal of the passengers to take them to the destinations of their choice. We also assume that:

1. Each taxi has a maximum capacity of 1 passenger.
2. A taxi ride being more dispensing than a bus or subway ride, a passenger will wait a certain time, in a station, before deciding to call a taxi. If he calls a taxi, the passenger definitely leaves the station to wait for a taxi.
#### Implantation

This transport system is simulated by a main process and a taxis process (child process of the main process). The main process communicates with the taxis process through a named pipe. Figure 2 shows the conceptual diagram of the system.

#### Main process

The main process consists of three threads: metro, bus, and checker. The bus travels clockwise (indicated by an arrow in Figure 1). Each bus station has a queue of passengers associated with it. The metro provides transport for passengers between stations 5, 6 and 7. Metro users can wait in the same station but for a different direction. To do this, each metro station has two queues (one queue for each direction). All station queues are FIFO type queues. Passengers are represented by a structure containing the following information:
— Unique identification number
— Departure station.
— Arrival station
— Elapsed waiting time
— A transfer between the metro and bus circuit is required (boolean value)
— Maximum waiting time

Passengers are read from a text file. The main process receives the file name from the command line, creates the passengers and inserts them into the correct queue. In the text file, the first line sets the number of passengers while a line preceded by a '##' identifies a passenger. On the same line, the order of the information read corresponds to the order of the variables in the passenger data structure. For example, the line `”## 21 3 6 0 1 8”` identifies passenger 21 who has been at station 3 (since 0 units of time) and wants to go to station 6. His maximum waiting time is 8 units of time. After reading and distributing the passengers to the station queues, the main process creates, in order, the named communication pipe, the taxis process and the threads.
#### Les threads autobus et métro

Bus and metro threads are repetitive (cyclic). At each cycle, each thread (bus and metro) performs the following operations, in order:

1. Increment a station counter according to the direction of the vehicle (metro, bus).
2. Check in his list if a passenger has arrived at his destination. If necessary, it ensures the disembarkation of the passenger(s) in question by adjusting its list of passengers and displays the actions carried out. If necessary, transfer passengers to the tail of stations 0 or 5. The display format is:
    `[bus or metro]: [embarks or disembarks] the passenger {id_passager}`
    `[bus or metro]: passenger transfer {id} to station {id_station}`
3. Board new passengers by looking at the queue that corresponds to the station counter. Pay attention to the maximum capacity of the vehicle.
4. Give control to the verifier once their execution is complete via a bilateral appointment (see the description of a bilateral appointment below). Each cycle of the bus thread runs concurrently with a cycle of the metro thread. These two cycles are always followed by a cycle of the verifier thread.

#### The verifier thread

The verifier is a repetitive thread. at each cycle, he carries out, in order, the actions
following:

1. Cycle through all passenger queues and increment each passenger's wait time.
2. Compare each passenger's wait time with their maximum wait time.
3. Transfer the passenger from the bus/metro queue (if their maximum waiting time has been reached) to the named tube storing passengers waiting for taxis. This assumes that the passenger requests a taxi when their maximum waiting time is reached. The thread then displays the following message:
    `verifier: transfer of passenger {id_passenger} to taxi {id_taxi}`
4. Give control to the bus and the metro via a bilateral rendezvous. Both bus and metro threads are idle while the verifier runs its cycle.

#### The taxi process

The taxis process is made up of three threads (one for each taxi) which are responsible for retrieving requests from the pipe (blocking reading). When a taxi thread receives a request from a passenger, it executes usleep(10) then displays:
    `taxi##{id_taxi}: passenger {id_passenger} is returned to the station {id_station}`
