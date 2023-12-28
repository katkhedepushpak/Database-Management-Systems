#include <iostream>
#include "classes.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>

using namespace std;

int main(int argc, char* const argv[]) {
    // Create the EmployeeRelation file from Employee.csv
    StorageBufferManager manager("EmployeeRelation");
    manager.createFromFile("Employee.csv");

    // Loop to lookup IDs until user is ready to quit
    // while (true) {
        // cout << "Enter an employee ID to search for, or type 'q' to quit: ";
        // string input;
        // cin >> input;

        if (argc > 1) {
        int search_id = std::stoi(argv[1]);
  

        try {
            int id = stoi(argv[1]);
            Record result = manager.findRecordById(id);
            cout << "Employee found:\n";
            result.print();
        } 
        catch (const exception& e) {
            cout << "Invalid input. Please enter a valid employee ID or 'q' to quit." << endl;
        }
    }
    else{
        cout<< "provide ID as an argument";
    }

return 0;
}




