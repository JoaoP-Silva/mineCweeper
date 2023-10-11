# mineCweeper
A minesweeper game using C sockets.

# About
**mineCweeper** is a simple C socket programming project that emulates a minesweeper game server. The board game is represented as a 4x4 matrix 0-indexed. Client/server connection and comunication are made throught the terminal, and the client can perform the following commands:
- **start**: start a game.
- **reveal x,y**: reveal a cell (x and y are integers $\in [0, 3]$).
- **flag x,y**: add flag to a cell (x and y are integers $\in [0, 3]$).
- **remove_flag x,y**: remove a flag from a cell (x and y are integers $\in [0, 3]$).
- **reset**: reset the current game.
- **exit**: end connection.

After the client sends the respective command to the server, the server responds with the new state of the board and waits for new commands. The board is printed in the terminal using the following notation:
- **-** represents a hidden cell.
- **\*** represents a bomb.
- **>** represents a flag.
- Integers represent the number of bombs in the neighbors cells.

The board configuration is an txt file where $-1$ represent bombs and positive integers represent the number of bombs in neighbors cells (see [example](https://github.com/JoaoP-Silva/mineCweeper/blob/main/input/in.txt)).
# Build and run instructions
Run ``make`` in the root dir to build client and server binaries. To run the server execute:

``./bin/server <ipversion> <port> <input>``

Where <ipversion> is the type of address where the server will be opened (v4 for ipv4 and v6 for ipv6), <port> is the port number and <input> is the board input file.
To run the client execute:

``./bin/client <address> <port>``

Where \<address> and \<port> are the server address(local address) and port. 
Example:
![image](https://github.com/JoaoP-Silva/mineCweeper/assets/73205375/72e789ea-6535-48dc-ad81-cb3db8466237)

# Final considerations
That wasn't the first time that i worked with C socket programming, but now with more maturity i was able to write much cleaner code and really understand the basics around network programming. Aways learning!
