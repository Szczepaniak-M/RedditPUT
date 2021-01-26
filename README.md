# RedditPUT
Simple publish/subcribe system. 

## Main information
- Client program use Java 11 and JavaFx.
- Server program use C, pthreads, BSD sockets and SQLite3.
- Each client can write messages on channels.
- When there are new messages on channels, which client subscribes, the client gets notification about it.

## How to run server?
1. Run command if you don't have sqlite3 `sudo apt-get install libsqlite3-dev`
2. Go to catalog `server`
3. Run command `make` to build executable file
4. Run command `./serwer <port>` to start server. <port> - port on which server will be listening

## How to run client?:
1. Download Java 11 and Maven.
2. In catalog client run command `mvn javafx:run` 
