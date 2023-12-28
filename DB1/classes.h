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
    ofstream out_file;
    ifstream in_file;
    char page[MAX_PAGES * BLOCK_SIZE];
    int page_idx = 0;
    int page_offset = 0;
    int numRecords = 0;

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
        if (page_offset + record_size > BLOCK_SIZE) {
            out_file.write(page + page_idx * BLOCK_SIZE, BLOCK_SIZE);
            page_idx = (page_idx + 1) % MAX_PAGES;
            page_offset = 0;
        }

        // Copy the record into the page buffer
        memcpy(page + page_idx * BLOCK_SIZE + page_offset, record_str.data(), record_size);
        page_offset += record_size;
        numRecords++;
    }

public:
    // The constructor initializes the output file
    StorageBufferManager(string NewFileName) : file_name(NewFileName) {
        out_file.open(NewFileName, ios::binary);
    }

    // The createFromFile function reads records from a CSV file and writes them to the binary file
    void createFromFile(string csvFName) {
        ifstream file(csvFName);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            vector<string> fields;
            string field;

            // Read fields from the CSV line and store them in a vector
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            Record record(fields);
            insertRecord(record);
        }

        // If the last page in the buffer has data, write it to disk
        if (page_offset > 0) {
            out_file.write(page + page_idx * BLOCK_SIZE, BLOCK_SIZE);
        }

        out_file.close();
    }

    // The findRecordById function searches for an employee record by ID and returns it as a Record object
    Record findRecordById(int id) {
        in_file.open(file_name, ios::binary);
        char page[BLOCK_SIZE * MAX_PAGES];
        int page_idx = 0;

        // Read pages from the binary file into the main memory buffer
        while (in_file.read(page + page_idx * BLOCK_SIZE, BLOCK_SIZE)) {
            int page_offset = 0;

            // Iterate through the records within the page
            while (page_offset < BLOCK_SIZE) {
                int record_id;
                memcpy(&record_id, page + page_idx * BLOCK_SIZE + page_offset, sizeof(int));

                // If the record ID matches the search ID, parse and return the record
                if (record_id == id) {
                    vector<string> fields;
                    fields.push_back(to_string(record_id));
                    page_offset += sizeof(int);

                    fields.push_back(string(page + page_idx * BLOCK_SIZE + page_offset));
                    page_offset += fields.back().size() + 1;

                    fields.push_back(string(page + page_idx * BLOCK_SIZE + page_offset));
                    page_offset += fields.back().size() + 1;
                    
                    int manager_id;
                    memcpy(&manager_id, page + page_idx * BLOCK_SIZE + page_offset, sizeof(int));
                    fields.push_back(to_string(manager_id));

                    Record record(fields);
                    in_file.close();
                    record.print();
                    return record;
                } else {
                    // If the record ID doesn't match, move to the next record
                    page_offset += sizeof(int);
                    page_offset += strlen(page + page_idx * BLOCK_SIZE + page_offset) + 1;
                    page_offset += strlen(page + page_idx * BLOCK_SIZE + page_offset) + 1;
                    page_offset += sizeof(int);
                }
            }
            page_idx = (page_idx + 1) % MAX_PAGES;
        }

         vector<string> fields_null;
        fields_null.push_back(to_string(0));
        fields_null.push_back(to_string(0));
        fields_null.push_back(to_string(0));
        fields_null.push_back(to_string(0));
        Record record1(fields_null);
        in_file.close();
        
        cout<< "Record not found" <<endl;
        return record1;
      

    }
};