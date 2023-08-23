/*
* This program does the following operations:
*                                 a) Allows the user to enter the current time
*                                 b) Reads the flights from a file
*                                 c) Writes the flights in the file
*                                 d) Allows the user to enter a new flight
*                                 e) Allows the user to delete (cancel) a flight - by entering the FlightNO
*                                 f) Allows the user to modify the time of a flight 
*                                 g) Allows the user to enter delay in a flight 
*                                 h) Displays all the flights 
*                                 i) Displays departures flights that have not departure yet 
*                                 j) Displays arrival flights that have not arrived yet 
*                                 k) Sorts flights by FlightNO
*                                 l) Sorts flights by time 
* 
* Created by Elizabeth Paula Kozlova 
* 
* Note: for some of the functions I used a help in the Internet 
*       (especially for the sort functions and read/write from/to file functions) 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#define stringify( name ) #name
using namespace std;

//enumerated types for flight type and errorCode 
enum FlightType { Departure, Arrival };
enum errorCode { underflow, overflow, success, fail };

//structure for time description
struct TimeRec {
    int hour; //0 to 23 
    int min; //0 to 59
    int sec; //0 to 59
    TimeRec() : hour(0), min(0), sec(0) {}
    bool isEarlier(TimeRec time, tm t);
};

//structure for flight description 
struct FlightRec {
    string FlightNO;
    string Destination;
    TimeRec Time;
    FlightType Ftype;
    bool Delay;
    TimeRec ExpectedTime; //if the flight is delayed
    bool operator<(const FlightRec& other) const {
        return Time.hour < other.Time.hour || (Time.hour == other.Time.hour && Time.min < other.Time.min);
    }
};

//function, that will be needed to check the difference in time 
bool TimeRec::isEarlier(TimeRec time, tm t)
{
    if (time.hour != t.tm_hour) return time.hour < t.tm_hour;
    if (time.min != t.tm_min) return time.min < t.tm_min;
    return time.sec < t.tm_sec;
}

//node structure 
template <class T>
struct Node {
    T entry;
    Node<T>* next;
    Node(T item, Node<T>* n = NULL);
};

//constructor
template <class T>
Node<T>::Node(T item, Node<T>* n) {
    entry = item;
    next = n;
}

//class List 
template <class T>
class List {
public:
    List();
    bool empty() const;
    Node<T>* getHead();
    errorCode remove(string flightNO);
    errorCode insert(const T& item);
    void print() const;
    void sortByTime();
    void sortByFlightNo();
private:
    Node<T>* head;
};

//implementation of all the functions of the List class

//constractor for List 
template <class T>
List<T>::List() {
    head = NULL;
}

//function, that returns the head of the List 
template <class T>
Node<T>* List<T>::getHead() {
    return head;
}

//empty function, that checks if the List is empty or not 
template <class T>
bool List<T>::empty() const {
    return head == NULL;
}

//(d)
//function, that inserts a new entry to the list if the list is not full 
template <class T>
errorCode List<T>::insert(const T& item) {
    Node<T>* temp = new Node<T>(item);

    //if the list is full
    if (temp == NULL) return overflow;
    
    //else if the list is empty 
    else if (empty()) head = temp;

    //else if the list is not full nor empty
    else if (item < head->entry) {
        temp->next = head;
        head = temp;
    }
    else {
        Node<T>* previous = head, * current = head->next;
        while (current != NULL) {
            if (item < current->entry) {
                temp->next = current;
                break; //exit the loop 
            }
            previous = current;
            current = current->next;
        }
        previous->next = temp;
    }
    return success;
}

//(e)
//function, that removes the flight from the list based on the entered flight number 
template <class T>
errorCode List<T>::remove(string flightNO) {
    Node<T>* current = head;

    //checks if the list is empty
    if (empty()) return underflow;

    //checks if the entered flight number is less than the flight number of the first entry of the list 
    //if it is -> return fail, as the list is sorted 
    if (flightNO < head->entry.FlightNO) return fail;

    //checks if the entered flight number equals to the flight number of the first entry of the list 
    //if it is then just delete the first entry and move the head to the next node 
    if (flightNO == head->entry.FlightNO) {
        head = head->next;
        delete current;
        return success;
    }

    //if other statements where false -> go through each flight number of each entry in the list and find the desired flight 
    Node<T>* previous = current;
    current = current->next;
    while (current != NULL) {
        if (flightNO < current->entry.FlightNO) break; //exit the loop
        if (flightNO == current->entry.FlightNO) {
            previous->next = current->next;
            delete current;
            return success;
        }
        else {
            previous = current;
            current = current->next;
        }
    }

    //if there is no flight with the entered flight number in the list -> return fail 
    return fail;
}

//(h)
//print function, that displays all the flights with their description 
template <class T>
void List<T>::print() const {
    Node<T>* current = head;
    string fType;
    cout << "Flight Number | Destination |   Time   | Flight Type | Delay | Expected Time" << endl;
    cout << "----------------------------------------------------------------------------" << endl;
    while (current != NULL) {
        cout << setw(8) << current->entry.FlightNO;
        cout << setw(15) << current->entry.Destination;
        cout << setw(10) << current->entry.Time.hour << ":" << current->entry.Time.min << ":" << current->entry.Time.sec << "\t  ";
        if (current->entry.Ftype == Departure) fType = "Departure";
        else if (current->entry.Ftype == Arrival) fType = "Arrival";
        cout << setw(10) << fType;
        if (current->entry.Delay) {
            cout << setw(8) << "YES";
            cout << setw(8) << current->entry.ExpectedTime.hour << ":" << current->entry.ExpectedTime.min << ":" << current->entry.ExpectedTime.sec;
        }
        else {
            cout << setw(8) << "NO";
            cout << setw(8) << "N/A";
        }
        cout << endl;
        current = current->next;
    }
}

//(l)
//function, that sorts flights by time 
//used in the function, that writes all the flights to the file 
template <class T>
void List<T>::sortByTime() {
    for (Node<T>* i = head; i != nullptr; i = i->next) {
        for (Node<T>* j = i->next; j != nullptr; j = j->next) {
            if (j->entry < i->entry) {
                swap(i->entry, j->entry);
            }
        }
    }
}

//(k)
//function, that sorts flights by flight number 
//used in the function, that writes all the flights to the file 
template <class T>
void List<T>::sortByFlightNo() {
    for (Node<T>* i = head; i != nullptr; i = i->next) {
        for (Node<T>* j = i->next; j != nullptr; j = j->next) {
            if (j->entry.FlightNO < i->entry.FlightNO) {
                swap(i->entry, j->entry);
            }
        }
    }
}

//(b)
//function to read the flights from a file 
void readFlightsFromFile(List<FlightRec>& flights, const string& filename)
{
    ifstream file(filename);

    //checks if the file can be opened 
    if (file.is_open())
    {
        //create a new FlightRec object to write each flight from the file
        FlightRec flight;

        //convert enumerated type to a string
        string type = stringify(flight.Ftype);

        //run through a loop while there are flights 
        //during each iteration write all the description of the flight to the created FlightRec object
        while (file >> flight.FlightNO >> flight.Destination >> flight.Time.hour >> flight.Time.min >> flight.Time.sec >> type >> flight.Delay >> flight.ExpectedTime.hour >> flight.ExpectedTime.min >> flight.ExpectedTime.sec)
        {
            //add this FlightRec object to the existing List
            flights.insert(flight);
        }

        //close the file
        file.close();
    }

    //if there is no such file -> cout error message 
    else
    {
        cout << "Unable to open file " << filename << endl;
    }
}

//(c)
//function to write the flights to the file
void writeFlightsToFile(List<FlightRec>& flights, const string& filename)
{
    ofstream file(filename);

    //checks if the file can be opened 
    if (file.is_open())
    {
        //sort the flights by time
        flights.sortByTime();

        //sort the flights by flight number
        flights.sortByFlightNo();

        //write the flights to the file 
        //for-loop which iterates until there is no flights in the List
        for (Node<FlightRec>* node = flights.getHead(); node != nullptr; node = node->next)
        {
            file << node->entry.FlightNO << " " << node->entry.Destination << " " << node->entry.Time.hour << " " << node->entry.Time.min << " " << node->entry.Time.sec << " " << node->entry.Ftype << " " << node->entry.Delay << " " << node->entry.ExpectedTime.hour << " " << node->entry.ExpectedTime.min << " " << node->entry.ExpectedTime.sec << endl;
        }

        //close the file
        file.close();
    }

    //if there is no such file -> cout error message 
    else
    {
       cout << "Unable to open file " << filename << endl;
    }
}

//additional functions 

//function, that print the menu 
void menu()
{
    cout << "Menu:" << endl;
    cout << "1. Enter a new flight" << endl;
    cout << "2. Delete a flight by flight number" << endl;
    cout << "3. Change time of a flight" << endl;
    cout << "4. Add delay to a flight" << endl;
    cout << "5. Display all flights" << endl;
    cout << "6. Display departures flights that have not departed yet" << endl;
    cout << "7. Display arrival flights that have not arrived yet" << endl;
    cout << "8. Read flights from a file" << endl;
    cout << "9. Write flights to a file" << endl;
    cout << "10. Exit" << endl;
    cout << "Enter your choice: ";
}

//addition to (d)
//function that checks the entered character, that must represent each of the flight types is correct 
//and afterwards assigns an according value to the flight type 
void flightTypeChecker(FlightRec& flight, char typeChar)
{
    bool checker = false;
    if (typeChar == 'D' || typeChar == 'd')
    {
        flight.Ftype = Departure;
        checker = true;
    }
    else if (typeChar == 'A' || typeChar == 'a')
    {
        flight.Ftype = Arrival;
        checker = true;
    }
    else
    {
        while (!checker)
        {
            cout << "Invalid character. Please, enter again: ";
            cin >> typeChar;
            if (typeChar == 'D' || typeChar == 'd')
            {
                flight.Ftype = Departure;
                checker = true;
            }
            else if (typeChar == 'A' || typeChar == 'a')
            {
                flight.Ftype = Arrival;
                checker = true;
            }
            else
                checker = false;
        }
    }
}

//(f)
//function, that changes the time of the flight 
void changeTime(List<FlightRec>& flightList, string flightNO, TimeRec newTime) {
    Node<FlightRec>* current = flightList.getHead();
    while (current != NULL) {
        if (current->entry.FlightNO == flightNO) {
            current->entry.Time = newTime;
            break;
        }
        current = current->next;
    }
}

//(g)
//function, that adds a delay to the flight and changes the time according to the entered new time 
void addDelay(List<FlightRec>& flightList, string flightNO, TimeRec delayTime) {
    Node<FlightRec>* current = flightList.getHead();

    //while-loop iterates until it finds the desired flight number and changes the flight's time and delay status 
    while (current != NULL) {
        if (current->entry.FlightNO == flightNO) {
            current->entry.Delay = true;
            current->entry.ExpectedTime.hour = current->entry.Time.hour + delayTime.hour;
            current->entry.ExpectedTime.min = current->entry.Time.min + delayTime.min;
            current->entry.ExpectedTime.sec = current->entry.Time.sec + delayTime.sec;
            break;
        }
        current = current->next;
    }
}

//(i)
//function, that display only the departures flights that have not departured yet 
void displayNotDeparted(List<FlightRec>& flightList) {
    Node<FlightRec>* current = flightList.getHead();

    //get the local current time 
    time_t now = time(0);
    tm localtime_now;
    localtime_s(&localtime_now, &now);

    //while-loop iterates and display the flights but only those which time is less than the current time 
    while (current != NULL) {
        if (current->entry.Ftype == Departure && current->entry.Time.isEarlier(current->entry.Time, localtime_now)) {
            cout << "Flight Number: " << current->entry.FlightNO << endl;
            cout << "Destination: " << current->entry.Destination << endl;
            cout << "Departure Time: " << setw(2) << setfill('0') << current->entry.Time.hour << ":" << setw(2) << setfill('0') << current->entry.Time.min << ":" << setw(2) << setfill('0') << current->entry.Time.sec << endl;
        }
        current = current->next;
    }
}

//(j) 
//function, that displays only the arrival flights that have not arrived yet 
void displayNotArrived(List<FlightRec>& flightList) {
    Node<FlightRec>* current = flightList.getHead();

    //get the local current time 
    time_t now = time(0);
    tm localtime_now;
    localtime_s(&localtime_now, &now);

    //while-loop iterates and display the flights but only those which time is less than the current time 
    while (current != NULL) {
        if (current->entry.Ftype == Arrival && current->entry.Time.isEarlier(current->entry.Time, localtime_now)) {
            cout << "Flight Number: " << current->entry.FlightNO << endl;
            cout << "Destination: " << current->entry.Destination << endl;
            cout << "Departure Time: " << setw(2) << setfill('0') << current->entry.Time.hour << ":" << setw(2) << setfill('0') << current->entry.Time.min << ":" << setw(2) << setfill('0') << current->entry.Time.sec << endl;
        }
        current = current->next;
    }
}

//main function 

//has a while-loop and a switch statement, so that the while-loop iterates until the program is not terminated by case 10 - EXIT
//switch statement is used to do different actions, that the user wants to based on their choice 
int main() {

    List<FlightRec> flightList;
    FlightRec newFlight;
    TimeRec newTime;
    errorCode result;

    int choice;
    char typeChar;
    string flightNO;
    string flightsFileName;

    while (true) {

        //display the menu
        menu();

        //accept the user's choice
        cin >> choice;

        switch (choice) {
        
        //enter a new flight -> enter all of the necessary details about the flight 
        case 1: 
            cout << "Enter flight number: ";
            cin >> newFlight.FlightNO;
            cout << "Enter destination: ";
            cin >> newFlight.Destination;
            cout << "Enter time (hour minute second): ";
            cin >> newFlight.Time.hour >> newFlight.Time.min >> newFlight.Time.sec;
            cout << "Enter flight type (D for departure, A for arrival): ";
            cin >> typeChar;
            flightTypeChecker(newFlight, typeChar);

            //insert a flight to the List and check if it was inserted correctly 
            result = flightList.insert(newFlight);
            if (result == success) {
                cout << "Flight added successfully!" << endl;
            }
            else {
                cout << "Error adding flight!" << endl;
            }
            break;

        //delete a flight by using a flight number
        case 2:

            //ask and accept from the user the flight number of the flight that they want to delete (cancel) 
            cout << "Enter flight number: ";
            cin >> flightNO;

            //remove the flight and check if it was removed correctly 
            result = flightList.remove(flightNO);
            if (result == success) {
                cout << "Flight removed successfully!" << endl;
            }
            else {
                cout << "Error removing flight!" << endl;
            }
            break;

        //change time of the flight
        case 3:

            //ask and accept from the user flight number of the flight of which they want to change the time 
            cout << "Enter a flight number of the flight, that you what to change time: ";
            cin >> flightNO;

            //ask and accept from the user the new time of the flight 
            cout << "Enter new time: ";
            cin >> newTime.hour >> newTime.min >> newTime.sec;

            //change the time 
            changeTime(flightList, flightNO, newTime);
            break;

        //add delay to the flight 
        case 4:

            //ask and accept from the user flight number of the flight for which they want to add a delay
            cout << "Ënter a flight number of the flight, that you want to add a delay: ";
            cin >> flightNO;

            //ask and accept from the user the delay time of the flight 
            cout << "Enter a delay time: ";
            cin >> newTime.hour >> newTime.min >> newTime.sec;

            //add the delay to the flight
            addDelay(flightList, flightNO, newTime);
            break;

        //display all flights 
        case 5:

            //call function print
            flightList.print();
            break;

        //display departure flights that have not departed yet
        case 6:

            //call function displayNotDeparted
            displayNotDeparted(flightList);
            break;

        //display arrival flights that have not arrived yet
        case 7: 

            //call function displayNotArrived
            displayNotArrived(flightList);
            break;

        //read flights (and their descriptions) from a file
        case 8:

            //ask and accept from the user the name of the file, from which they want to read the flights and their description 
            cout << "Enter a name of the file, from which, you want to read the flights: ";
            cin >> flightsFileName;

            //read flights from a file
            readFlightsFromFile(flightList, flightsFileName);
            break;

        //write flights (and their descriptions) to the file  
        case 9:

            //ask and accept from the user the name of the file, in which they want to write the flights and their description 
            cout << "Enter a name of the file, where you want to save the flights information: ";
            cin >> flightsFileName;

            //write flights to the file 
            writeFlightsToFile(flightList, flightsFileName);
            break;

        //exit 
        case 10:

            //terminates the program 
            return 0;

        //default -> for cases when the choice is invalid 
        default:

            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    }

    return 0;
}
