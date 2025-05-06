#include <iostream> //input and output library for user inputs and outputs and cout commands
#include <vector> //library for allowing the use of vectors
#include <string> //library to allow saving strings into variables
#include <sstream> //this uses strings as streams so it can parse strings as inputs
#include <algorithm>//algorithms gives access to the find and sort commands for c++
#include <iomanip>//iomanip is a maniopulation header to maniupulate iostream input and output
#include <map> //map will be used to connect strings to integers using the map command
#include <ctime> //library that can be used for a bunch of time based functions, structs, and other stuff
#include <fstream> //A Needed to add fstream to fix error: variable ‘std::ifstream infile’ has initializer but incomplete type
using namespace std; //makes it so printing strings dont requite std:: before cout so code can be written faster

int arrivalCounter = 0; //the arrival order variable will be used to track when emails come in, the more recent the order, the higher the priority
bool is_read = false; //A Added this is_read boolean so READ doesnt discard emails not read yet
/*
    Code written by chatgpt
    The parsedata fucntion was made to convert a string
    into a time_t datatype, which uses dates and time. this
    will be used to determine when the email was sent. the more recent the email,
    the higher priority it has
*/
time_t parseDate(const string& dateStr) { //initializes a data parser function to turn the date string into a time_t data type so that way the day it was sent can be compared
    struct tm tm{}; //this struct is used to create calender dates, and is broken up into a bunch of attributes like year, month, and day
    sscanf(dateStr.c_str(), "%d-%d-%d", &tm.tm_mon, &tm.tm_mday, &tm.tm_year);
    tm.tm_mon -= 1; //for month, because it starts at 0, the month entered and parsed through needs to be subtracted by 1
    tm.tm_year -= 1900; //this designates a year since 1900, so it needs to subtract the year by 1900 to get the proper year
    return mktime(&tm); //returns the full date struct as a whole so it can be compared to other dates
}

/*
    this code was all chat gpt given for the eecs 348 assignment 8
    this mapping function takes the string and maps it to have an equivalent integer
    according to C++ documentation, map pairs two values together kind of like a dictionary, where a number gets tied to a string
*/
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
        if (senderPriority[senderCategory] != senderPriority[other.senderCategory]) //if the sender categories are not the same
            return senderPriority[senderCategory] > senderPriority[other.senderCategory]; //return whichever
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
    MaxHeap heap; //initializes a new instance of the heap to store emails
    bool hasCurrent = false; //this boolean checks to make sure that there is a current email that is available to be looked at by the program
    Email currentEmail = Email("", "", "01-01-2000"); //this is a placeholder dummy email just in case the next function is called.

public: //this is all the publically facing attributes and functions
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
            if (hasCurrent) {//hascurrent variable is used to make sure that there is an email being looked at by the program, 
                cout << "Next email:" << endl;//lets the user know that the next email is being printed out
                currentEmail.display(); //calls the function to display the current email
                is_read = true; //A sets the is_read to be true so the email can be discarded
                cout << endl; //end line to make a new line
            } else {
                cout << "No emails to read.\n" << endl; //let the user know that there are no emails to be read
            }
        } else if (line.find("READ") != string::npos) { //A If the word READ is in the line
        //D } else if (line == "READ") {
            if (is_read == false) //A If the email hasnt been read yet, then it shouldnt be discarded
            {//A
                cout << "email not read yet. \n" << endl; //A Let the user know that the email wasnt discarded
            }//A
            else //A Otherwise, if it has been read
            {
                if (hasCurrent) { //if the hascurrent boolean is true, that means there is a current node being looked at
                    heap.pop();//attempts to pop the node to remove the email from the top
                    hasCurrent = false; //sets hascurrent to false because there wont be a new 
                } else if (!heap.empty()) { //if the heap is not empty, then pop the node
                    heap.pop(); //calls the pop function for the initialized heap object to get rid of the email at the top of the heap
                }
                is_read = false;//A sets is_read to false so the next email doesnt get discarded before its looked at
            }//A

        }
    }
};

/*
    This block of code written by chatgpt
    uses the fstream operations to open and read the file,
    nothing in this block of code needed to be changed or modified
    during bug testing, and works as intended
*/
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
