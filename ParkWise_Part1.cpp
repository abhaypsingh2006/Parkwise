#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <queue>
using namespace std;

const int TOTAL_SLOTS = 10;

class Vehicle
{
protected:
    string number;
    string owner;
    int duration;

public:
    Vehicle(string n, string o, int d)
        : number(n), owner(o), duration(d) {}

    virtual double getRate() = 0;
    virtual string getType() = 0;

    string getNumber() { return number; }
    string getOwner() { return owner; }
    int getDuration() { return duration; }

    virtual void display()
    {
        cout << left
             << setw(15) << number
             << setw(20) << owner
             << setw(15) << getType()
             << setw(10) << duration
             << "Rs. " << getRate() << endl;
    }

    virtual ~Vehicle() {}
};

class Car : public Vehicle
{
public:
    Car(string n, string o, int d) : Vehicle(n, o, d) {}

    double getRate() { return 50; }
    string getType() { return "Car"; }
};

class Bike : public Vehicle
{
public:
    Bike(string n, string o, int d) : Vehicle(n, o, d) {}

    double getRate() { return 20; }
    string getType() { return "Bike"; }
};

class EmergencyVehicle : public Vehicle
{
public:
    EmergencyVehicle(string n, string o, int d)
        : Vehicle(n, o, d) {}

    double getRate() { return 0; }
    string getType() { return "Emergency"; }
};

class Billing
{
public:
    double calculateBill(Vehicle *v)
    {
        return v->getRate() * v->getDuration();
    }

    void showBill(Vehicle *v, int slot)
    {
        cout << "\n";
        cout << "+==========================================+\n";
        cout << "|              PARKWISE INVOICE            |\n";
        cout << "+==========================================+\n";

        cout << left;
        cout << "| Vehicle Number : " << setw(24)
             << v->getNumber() << "|\n";

        cout << "| Owner Name     : " << setw(24)
             << v->getOwner() << "|\n";

        cout << "| Vehicle Type   : " << setw(24)
             << v->getType() << "|\n";

        cout << "| Parking Slot   : " << setw(24)
             << slot << "|\n";

        cout << "| Duration       : " << setw(24)
             << v->getDuration() << "|\n";

        cout << "| Rate Per Hour  : Rs. "
             << setw(20) << v->getRate() << "|\n";

        cout << "+------------------------------------------+\n";

        cout << "| TOTAL AMOUNT   : Rs. "
             << setw(20) << calculateBill(v) << "|\n";

        cout << "+==========================================+\n";
    }
};

class ParkingSlot
{
private:
    int slotNumber;
    Vehicle *vehicle;

public:
    ParkingSlot() : slotNumber(0), vehicle(NULL) {}

    ParkingSlot(int n) : slotNumber(n), vehicle(NULL) {}

    bool isEmpty() { return vehicle == NULL; }

    int getSlotNumber() { return slotNumber; }

    Vehicle *getVehicle() { return vehicle; }

    void park(Vehicle *v) { vehicle = v; }

    Vehicle *removeVehicle()
    {
        Vehicle *temp = vehicle;
        vehicle = NULL;
        return temp;
    }

    void display()
    {
        cout << left << setw(15) << slotNumber;

        if (vehicle == NULL)
            cout << "[ AVAILABLE ]" << endl;
        else
            cout << "[ OCCUPIED  ] "
                 << vehicle->getNumber()
                 << " (" << vehicle->getType() << ")"
                 << endl;
    }
};

class RecordNode
{
public:
    string vehicleNumber;
    string owner;
    string type;
    int slot;
    int duration;
    double amount;
    RecordNode *next;

    RecordNode(string n, string o, string t,
               int s, int d, double a)
        : vehicleNumber(n), owner(o), type(t),
          slot(s), duration(d), amount(a), next(NULL) {}
};

class RecordList
{
private:
    RecordNode *head;

public:
    RecordList() : head(NULL) {}

    void addRecord(string n, string o, string t,
                   int s, int d, double a)
    {
        RecordNode *newNode =
            new RecordNode(n, o, t, s, d, a);

        if (head == NULL)
            head = newNode;
        else
        {
            RecordNode *temp = head;

            while (temp->next != NULL)
                temp = temp->next;

            temp->next = newNode;
        }
    }

    void displayRecords()
    {
        if (head == NULL)
        {
            cout << "\nNo parking history available.\n";
            return;
        }

        cout << "\n";
        cout << "============================================================\n";
        cout << "                  PARKING HISTORY                           \n";
        cout << "============================================================\n";

        RecordNode *temp = head;

        while (temp != NULL)
        {
            cout << "Vehicle Number : " << temp->vehicleNumber << endl;
            cout << "Owner          : " << temp->owner << endl;
            cout << "Type           : " << temp->type << endl;
            cout << "Slot           : " << temp->slot << endl;
            cout << "Duration       : " << temp->duration << " hour(s)\n";
            cout << "Amount         : Rs. " << temp->amount << endl;
            cout << "------------------------------------------------------------\n";

            temp = temp->next;
        }
    }

    void saveHistory()
    {
        ofstream file("history.txt");

        RecordNode *temp = head;

        while (temp != NULL)
        {
            file << temp->vehicleNumber << "|"
                 << temp->owner << "|"
                 << temp->type << "|"
                 << temp->slot << "|"
                 << temp->duration << "|"
                 << temp->amount << endl;

            temp = temp->next;
        }
    }

    void loadHistory()
    {
        ifstream file("history.txt");

        if (!file)
            return;

        string number, owner, type;
        int slot, duration;
        double amount;

        while (getline(file, number, '|'))
        {
            if (!getline(file, owner, '|')) break;
            if (!getline(file, type, '|')) break;
            if (!(file >> slot)) break;
            file.ignore(1000, '|');
            if (!(file >> duration)) break;
            file.ignore(1000, '|');
            if (!(file >> amount)) break;
            file.ignore(1000, '\n');

            addRecord(number, owner, type,
                      slot, duration, amount);
        }
    }

    ~RecordList()
    {
        RecordNode *temp = head;

        while (temp != NULL)
        {
            RecordNode *nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
    }
};
