#include <string.h>
#include <string>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <vector>

using namespace std;
const int BLOCK_SIZE = 56;
const int MAX_BLOCKS = 22;

const string EMP_FILE = "Emp.csv";
const string DEPT_FILE = "Dept.csv";
const string JOIN_FILE = "Join.csv";

struct ReadBuffer 
{
    string line;
    ifstream* file;
};

int get_managerid(string d) 
{
    stringstream ds(d);
    string managerid;
    for (int i = 0; i < 4; i++) {
        getline(ds, managerid, ',');
    }
    return stoi(managerid);
}

int get_eid(string e)
{
    int eid;
    sscanf(e.c_str(), "%d,", &eid);
    return eid;
}

bool C_employee(string a, string b) 
{
    int eid_a;
    sscanf(a.c_str(), "%d,", &eid_a);

    int eid_b;
    sscanf(b.c_str(), "%d,", &eid_b);

    return eid_a < eid_b;
}

bool C_dept(string a, string b)
 {
    stringstream as(a);
    stringstream bs(b);

    string managerid_a;
    string managerid_b;
    for (int i = 0; i < 4; i++) {
        getline(as, managerid_a, ',');
        getline(bs, managerid_b, ',');
    }

    return stoi(managerid_a) < stoi(managerid_b);
}

int C_dept_employee(string dept, string emp) 
{
    int eid;
    sscanf(emp.c_str(), "%d,", &eid);

    stringstream as(dept);
    string managerid;
    for (int i = 0; i < 4; i++) {
        getline(as, managerid, ',');
    }

    return stoi(managerid) - eid;
}

int to_runs(string record_file_name, bool (*compar)(string, string), string file_base)
 {
    int runs_written;

    ifstream record_file(record_file_name, ifstream::in);

    for (runs_written = 0; record_file; runs_written++) {
        int num_read = 0;

        vector<string> records;
        string line;
        while (num_read < MAX_BLOCKS && getline(record_file, line)) {
            records.push_back(line);
            num_read++;
        }

        sort(records.begin(), records.end(), compar);

        string run_file_name = file_base + to_string(runs_written) + ".csv";
        ofstream run_file(run_file_name, ofstream::out | ofstream::trunc);
        for (int w = 0; w < num_read; w++) {
            run_file << records[w] << "\n";
        }
    }
    return runs_written;
}

vector<ReadBuffer> read_all(vector<ifstream*> files)
 {
    vector<ReadBuffer> tuples;
    string tuple;
    for (auto& file : files) {
        int pos = file->tellg();

        getline(*file, tuple);
        tuples.push_back({tuple, file});

        file->seekg(pos);
    }
    return tuples;
}

string peek(ifstream& file) 
{
    string tuple;
    int pos = file.tellg();

    getline(file, tuple);

    file.seekg(pos);
    return tuple;
}

void advance(ReadBuffer& r)
 {
    string temp;
    getline(*r.file, temp);

    r.line = peek(*r.file);
}

bool avl_buffer(vector<ReadBuffer> rbs)
 {
    for (auto& r : rbs) {
        if (r.line != "") {
            return true;
        }
    }
    return false;
}

vector<int> b_read(vector<ReadBuffer> rb, int (as_int)(string))
 {
    vector<int> as_key;
    for (int i = 0; i < rb.size(); i++) {
        if (rb[i].line != "") {
            as_key.push_back(as_int(rb[i].line));
        }
    }
    return as_key;
}

void run_join(string a_base, string b_base, int a_runs, int b_runs, int(a_as_int)(string), int (b_as_int)(string), bool b_not)
 {
    ofstream join_file(JOIN_FILE, ofstream::out | ofstream::trunc);

    vector<ifstream*> a_files;
    vector<ifstream*> b_files;

    for (int i = 0; i < a_runs; i++) {
        a_files.push_back(new ifstream(a_base + to_string(i) + ".csv", ifstream::in));
    }
    for (int i = 0; i < b_runs; i++) {
        b_files.push_back(new ifstream(b_base + to_string(i) + ".csv", ifstream::in));
    }
    vector<ReadBuffer> a_in_buffer = read_all(a_files);
    vector<ReadBuffer> b_in_buffer = read_all(b_files);

    while (avl_buffer(a_in_buffer) && avl_buffer(b_in_buffer)) {
        vector<int> a_keys = b_read(a_in_buffer, a_as_int);
        vector<int> b_keys = b_read(b_in_buffer, b_as_int);

        auto min_apos = min_element(a_keys.begin(), a_keys.end());
        auto min_bpos = min_element(b_keys.begin(), b_keys.end());

        int a_i = distance(a_keys.begin(), min_apos);
        int b_i = distance(b_keys.begin(), min_bpos);

        int min_a = *min_apos;
        int min_b = *min_bpos;

        if (min_a == min_b) {
            join_file << b_in_buffer[b_i].line << "," << a_in_buffer[a_i].line << "\n";
            if (b_not) {
                advance(b_in_buffer[b_i]);
            } else {
                advance(a_in_buffer[a_i]);
                advance(b_in_buffer[b_i]);
            }
        } else if (min_a > min_b) {
            advance(b_in_buffer[b_i]);
        } else {
            advance(a_in_buffer[a_i]);
        }
    }
}

int main(int argc, char** argv) {
    int run_emp = to_runs(EMP_FILE, C_employee, "emp");
    int run_dept = to_runs(DEPT_FILE, C_dept, "dept");

    run_join("emp", "dept", run_emp, run_dept, get_eid, get_managerid, true);
}
  