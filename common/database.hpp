#ifndef _DATABASE_HPP
#define _DATABASE_HPP

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

using namespace soci;
using namespace std;

class DBHelper {
    private:
        DBHelper();
        session sql;

        static DBHelper dbHelper;

    public:
        ~DBHelper();
        static DBHelper * getInstance() {
            if (dbHelper == NULL)
                dbHelper = new DBHelper();
            return dbHelper;
        }
        init();
        term();
};

#endif
