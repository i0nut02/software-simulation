# software-simulation

## TODO:
- [] make the message an integer and save the relationship integer - message
- make client and server for simulation

## tests

- 50 clients and 1 server 
    - each day a client make two request to the server and the server that take 1ms to respond 30 DAYS Simulation [1 minute]
    - each 5 seconds a client make a request to the server and the server that take 1 millisecond to respond 30 minutes simulation [6 minutes]
    - each 2 and half seconds a client make a request to the server that take 1 millisecond to respond 15 minutes simulation [5 minutes and 32 seconds]
    - each second a client make a request and the server that take 1 millisecond to respond 10 minutes simulation [9 minutes]
    - each half second a client make a request and the server that take 1 millisecond to respond 10 minutes simulation [FAILED]

- 100 clients and 1 server
    - each 2 and half seconds a client make a request to the server that take 1 millisecond to respond 15 minutes simulation [11 minutes]