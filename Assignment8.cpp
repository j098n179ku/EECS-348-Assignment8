#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <map>
#include <ctime>
#include <fstream> //A Needed to add fstream to fix error: variable ‘std::ifstream infile’ has initializer but incomplete type
using namespace std; //makes it so printing strings dont requite std:: before cout so code can be written faster

int arrivalCounter = 0; //the arrival order variable will be used to track when emails come in, the more recent the order, the higher the priority
bool is_read = false; //A Added this is_read boolean so READ doesnt discard emails not read yet
/*
    Code written by chatgpt
    The parsedata fucntion was made to convert a string
    into a time_t datatype, which uses dates and time. this
    will be used to
*/
// Convert MM-DD-YYYY to time_t for easy comparison
time_t parseDate(const string& dateStr) { //initializes a data parser function to turn the date string into a time_t data type so that way the day it was sent can be compared
    struct tm tm{};
    sscanf(dateStr.c_str(), "%d-%d-%d", &tm.tm_mon, &tm.tm_mday, &tm.tm_year);
    tm.tm_mon -= 1;           // struct tm months start from 0
    tm.tm_year -= 1900;       // struct tm years are since 1900
    return mktime(&tm);
}

// Map sender category to priority
map<string, int> senderPriority = { //map in order to equate strings to an integer. the higher the string priority, the higher the integer value, which mneans higher priority
    {"Boss", 5},//the string Boss has the highest priority, and will have the highest integer out of all the possible senders
    {"Subordinate", 4},//subordinate is the second highest priority, so it will have the second highest integer
    {"Peer", 3},//peer string is given a priority level of 3, making it the middle most in the priority list
    {"ImportantPerson", 2},//important person is designated to be the second lowest on the priority list
    {"OtherPerson", 1}//anyone else not listed will be given the lowest priority, such as spam emails and ads
};

class Email { //created the Email object, which will go inside of the heap
public: //public facing values and functions of the email class
    string senderCategory; //sender category will help determine the priority of the email
    string subject; //subject of the email is just a string and doesnt have any effect on the importance of the email
    string dateStr; //date string is when the email will send, the more recently the email was sent the higher the priority the email
    time_t date; //time_t data type which will be used to find the actual date datatype from the string
    int arrivalOrder;

    Email(string sender, string subj, string date_string) {//this is a constructor to fill the parameters of the email class with the parameters parsed through to the email class
        senderCategory = sender; //the sender category attribute will be set to the sender string passed through from the constructor
        subject = subj;
        dateStr = date_string;
        date = parseDate(date_string);
        arrivalOrder = arrivalCounter++;
    }

    // Compare by priority
    bool operator>(const Email& other) const {
        if (senderPriority[senderCategory] != senderPriority[other.senderCategory])
            return senderPriority[senderCategory] > senderPriority[other.senderCategory];
        if (date != other.date)
            return date > other.date;
        return arrivalOrder < other.arrivalOrder; // newer (lower arrivalOrder) is higher priority
    }

    void display() const {
        cout << "Sender: " << senderCategory << endl;
        cout << "Subject: " << subject << endl;
        cout << "Date: " << dateStr << endl;
    }
};

class MaxHeap {
private:
    vector<Email> heap;

    void heapifyUp(int idx) {
        while (idx > 0 && heap[idx] > heap[(idx - 1) / 2]) {
            swap(heap[idx], heap[(idx - 1) / 2]);
            idx = (idx - 1) / 2;
        }
    }

    void heapifyDown(int idx) {
        int size = heap.size();
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < size && heap[left] > heap[largest]) largest = left;
            if (right < size && heap[right] > heap[largest]) largest = right;

            if (largest != idx) {
                swap(heap[idx], heap[largest]);
                idx = largest;
            } else {
                break;
            }
        }
    }

public:
    void push(const Email& email) {
        heap.push_back(email);
        heapifyUp(heap.size() - 1);
    }

    Email peek() {
        if (!heap.empty()) return heap[0];
        throw runtime_error("No emails to read.");
    }

    void pop() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
    }

    int size() const {
        return heap.size();
    }

    bool empty() const {
        return heap.empty();
    }
};

class EmailManager {
private:
    MaxHeap heap;
    bool hasCurrent = false;
    Email currentEmail = Email("", "", "01-01-2000");

public:
    void processLine(const string& line) {
        if (line.rfind("EMAIL ", 0) == 0) {
            string rest = line.substr(6);
            stringstream ss(rest);
            string sender, subject, date;

            getline(ss, sender, ',');
            getline(ss, subject, ',');
            getline(ss, date, ',');

            Email email(sender, subject, date);
            heap.push(email);
        } else if (line.find("COUNT") != string::npos) { //A
        //D        } else if (line == "COUNT") {
            cout << "There are " << heap.size() << " emails to read.\n" << endl;
        } else if (line.find("NEXT") != string::npos) { //A
        // D} else if (line == "NEXT") {
            if (!hasCurrent) {
                if (!heap.empty()) {
                    currentEmail = heap.peek();
                    hasCurrent = true;
                }
            }
            if (hasCurrent) {
                cout << "Next email:" << endl;
                currentEmail.display();
                is_read = true;
                cout << endl;
            } else {
                cout << "No emails to read.\n" << endl;
            }
        } else if (line.find("READ") != string::npos) { //A
        //D } else if (line == "READ") {
            if (is_read == false) //A 
            {//A
                cout << "email not read yet. \n" << endl; //A
            }//A
            else //A
            {
                if (hasCurrent) {
                    heap.pop();
                    hasCurrent = false;
                } else if (!heap.empty()) {
                    heap.pop();
                }
                is_read = false;//A
            }

        }
    }
};

/*
    This block of code written by chatgpt
    uses the fstream operations to open and read the file,
    nothing in this block of code needed to be changed or modified
    during bug testing, and works as intended
*/
// Read from a file
void runFromFile(const string& filename) {//initializes the function that will handle opening the file and passing the information to get the line from the file
    ifstream infile(filename);//command from fstream that opens up the file
    string line; //initializes a line variable that the line of the file will be tied to so the data has a place to be
    EmailManager manager;//initializes a new instance of the email manager class, which is where all of the emails will be sorted, organized, read, and discarded.

    while (getline(infile, line)) {//while loop that continued until every line in the 
        if (!line.empty()) {//if the line is not empty, then continue, if not, go to the next line.
            manager.processLine(line);//calls the processline function from the email manager class, passing the line into the classes function
        }
    }
}

/*
    The main function of this code that was written
    by chat gpt was used only to pass the file into
    the file extracting function. the only thing that needed
    to be changed here was changing the name of the hardcoded file
*/
int main() {
    // Test using provided test file
    runFromFile("Assignment8_Test_File.txt"); //A changed the runFromFile parameter being passed through to be Assignment8_Test_file.txt, as the program was stated in the assignment that this will be the file run
//D    runFromFile("test.txt"); This was deleted and replaced
    return 0;//this just returns to end the program
}
