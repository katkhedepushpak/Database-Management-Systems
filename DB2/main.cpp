/*
Skeleton code for storage and buffer management
*/
#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"

using namespace std;

int main(int argc, char *const argv[]) {
    // Create the EmployeeIndex file and DataFile
    // insert records from Employee.csv to the DataFile and relevant information in the EmployeeIndex file
    // Loop to lookup IDs until user is ready to quit
   string line;
    BPlusTree b;
    b.createFromFile("Employee.csv");

    BPlusTree loaded_tree;
    loaded_tree.de_ser("Index");
   
    fstream file("Data.bin");

    while (1) {
        string id;
        cout << "Enter employee Id:" << endl;
        cin >> id;

        int dPosition =  line.find(":");
        int startAdd = stoi(line.substr(0, dPosition));
        int endAdd = stoi(line.substr(dPosition + 1, line.length() - 1)) - 1;
       
        //cout << startAddress << endAddress << endl;
        file.seekg(startAdd);
        char content[endAdd - startAdd + 1];
        file.read(content, endAdd - startAdd + 1);
       
    }  
    return 0;
}