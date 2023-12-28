#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>

using namespace std;

// The Record class represents an individual employee record with ID, name, bio, and manager_id fields
class Record {
public:
    int id, manager_id;
    std::string bio, name;

    // The constructor accepts a vector of strings representing the fields and initializes the class members
    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    // The print function displays the record's fields to the console
    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

// The StorageBufferManager class manages reading and writing of employee records to and from a binary file
class StorageBufferManager {
private:
    // Constants for block size and maximum number of pages in main memory
    const int static BLOCK_SIZE = 4096;
    const int static MAX_PAGES = 3;
    string file_name;

    ofstream new_file;
    ifstream given_csv;


    char page[MAX_PAGES * BLOCK_SIZE];
    int index_pg = 0;
    int offest_pg = 0;
    int num_rec = 0;

    // The insertRecord function writes a record to the main memory buffer and flushes to disk if necessary
    void insertRecord(Record record) {
        stringstream ss;
        ss.write(reinterpret_cast<const char*>(&record.id), sizeof(record.id));
        ss << record.name << '\0';
        ss << record.bio << '\0';
        ss.write(reinterpret_cast<const char*>(&record.manager_id), sizeof(record.manager_id));

        string record_str = ss.str();
        int record_size = record_str.size();

        // If the current page doesn't have enough space for the record, write the page to disk
        if (offest_pg + record_size > BLOCK_SIZE) {
            new_file.write(page + index_pg * BLOCK_SIZE, BLOCK_SIZE);
            index_pg = (index_pg + 1) % MAX_PAGES;
            offest_pg = 0;
        }

        // Copy the record into the page buffer
        memcpy(page + index_pg * BLOCK_SIZE + offest_pg, record_str.data(), record_size);
        offest_pg += record_size;
        num_rec++;
    }

public:
    // The constructor initializes the output file
    StorageBufferManager(string NewFileName) : file_name(NewFileName) {
        new_file.open(NewFileName, ios::binary);
    }


    void createFromFile(string csvFName) {
        ifstream file(csvFName);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            vector<string> fields;
            string field;

 
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            Record record(fields);
            insertRecord(record);
        }


        if (offest_pg > 0) {
            new_file.write(page + index_pg * BLOCK_SIZE, BLOCK_SIZE);
        }

        new_file.close();
    }


    Record findRecordById(int id) {
        given_csv.open(file_name, ios::binary);
        char page[BLOCK_SIZE * MAX_PAGES];
        int index_pg = 0;

       
        while (given_csv.read(page + index_pg * BLOCK_SIZE, BLOCK_SIZE)) {
            int offest_pg = 0;


            while (offest_pg < BLOCK_SIZE) {
                int rid;
                memcpy(&rid, page + index_pg * BLOCK_SIZE + offest_pg, sizeof(int));

                if (rid == id) {
                    vector<string> fields;
                    fields.push_back(to_string(rid));
                    offest_pg += sizeof(int);

                    fields.push_back(string(page + index_pg * BLOCK_SIZE + offest_pg));
                    offest_pg += fields.back().size() + 1;

                    fields.push_back(string(page + index_pg * BLOCK_SIZE + offest_pg));
                    offest_pg += fields.back().size() + 1;
                    
                    int manager_id;

                    memcpy(&manager_id, page + index_pg * BLOCK_SIZE + offest_pg, sizeof(int));
                    fields.push_back(to_string(manager_id));

                    Record record(fields);
                    given_csv.close();
                    record.print();
                    return record;
                } 
                
                else {
                
                    offest_pg += sizeof(int);
                    offest_pg += strlen(page + index_pg * BLOCK_SIZE + offest_pg) + 1;
                    offest_pg += strlen(page + index_pg * BLOCK_SIZE + offest_pg) + 1;
                    offest_pg += sizeof(int);
                }
            }
            index_pg = (index_pg + 1) % MAX_PAGES;
        }

         vector<string> fields_null;
        fields_null.push_back(to_string(0));
        fields_null.push_back(to_string(0));
        fields_null.push_back(to_string(0));
        fields_null.push_back(to_string(0));
        Record record1(fields_null);
        given_csv.close();
        
        cout<< "Record not found" <<endl;
        return record1;
      

    }
};