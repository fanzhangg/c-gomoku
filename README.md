# C Gomoku
This is an online terminal gomoku game. It is written in C, and uses socket to support networking.

## Usage
```shell
nc -C "hostname" "port" # lowercase -c on Mac
```
`hostname` is the full name of the machine on which your server is running, and `port` is the port on which your server is listening. To get the host name of the machine, run `hostname -f`. Use `localhost` in place of the host name if you are sure that the server and client are both on the same server.
