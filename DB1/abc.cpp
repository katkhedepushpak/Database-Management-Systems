#include <iostream>
#include "classes1.h"
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

    int choice = 1;
    int search_id;


    do {
  cout << "Please enter employee ID : ";
  cin >> search_id;
    cout<<endl;
    Record result = manager.findRecordById(search_id);
            
 
cout<<endl;
cout << "Do you want to continue lookup? Enter 1 for yes or any other number to exit : " ;
cin >> choice;
cout<<endl;
} while (choice == 1);


cout<<"exiting....." <<endl;
return 0;
}