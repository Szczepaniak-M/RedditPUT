#include "database.h"

int main(void) {
    
    ServerStatus status;
    int error;
    
    // Creating database
    error = createDatabase(&status);
    if (error != SQLITE_OK) {
        return error;
    }

    //Creating tables
    error = createTables(&status);
    if (error != SQLITE_OK) {
        return error;
    }
  
    
    sqlite3_close(status.db);
    return 0;
}

