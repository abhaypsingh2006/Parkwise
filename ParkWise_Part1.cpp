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

class ParkingManager
{
private:
    ParkingSlot slots[TOTAL_SLOTS];
    queue<Vehicle *> normalQueue;
    queue<Vehicle *> emergencyQueue;
    RecordList history;
    Billing billing;

public:
    ParkingManager()
    {
        for (int i = 0; i < TOTAL_SLOTS; i++)
            slots[i] = ParkingSlot(i + 1);
    }

    Vehicle *createVehicle()
    {
        string number, owner;
        int duration, type;

        cout << "\nEnter vehicle number: ";
        cin >> number;

        cout << "Enter owner name (single word): ";
        cin >> owner;

        cout << "Enter parking duration in hours: ";

        if (!(cin >> duration) || duration <= 0)
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\nInvalid duration.\n";
            return NULL;
        }

        cout << "\nSelect Vehicle Type\n";
        cout << "1. Car\n";
        cout << "2. Bike\n";
        cout << "3. Emergency Vehicle\n";
        cout << "Enter type: ";

        if (!(cin >> type))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\nInvalid vehicle type.\n";
            return NULL;
        }

        if (type == 1) return new Car(number, owner, duration);
        if (type == 2) return new Bike(number, owner, duration);
        if (type == 3) return new EmergencyVehicle(number, owner, duration);

        cout << "\nInvalid vehicle type.\n";
        return NULL;
    }

    void registerVehicle()
    {
        cout << "\n============================================\n";
        cout << "           VEHICLE REGISTRATION             \n";
        cout << "============================================\n";

        Vehicle *v = createVehicle();

        if (v == NULL)
            return;

        cout << "\nVehicle registered successfully!\n";
        cout << "Vehicle Number : " << v->getNumber() << endl;
        cout << "Vehicle Type   : " << v->getType() << endl;
        cout << "Parking Rate   : Rs. " << v->getRate()
             << " per hour\n";

        delete v;
    }

    void displaySlots()
    {
        cout << "\n================================================\n";
        cout << "               PARKING SLOT STATUS              \n";
        cout << "================================================\n";

        cout << left << setw(15) << "SLOT NUMBER" << "STATUS\n";
        cout << "------------------------------------------------\n";

        for (int i = 0; i < TOTAL_SLOTS; i++)
            slots[i].display();

        cout << "================================================\n";
    }

    int findFreeSlot()
    {
        for (int i = 0; i < TOTAL_SLOTS; i++)
            if (slots[i].isEmpty())
                return i;

        return -1;
    }

    void parkVehicle(Vehicle *v)
    {
        if (v == NULL)
            return;

        int slot = findFreeSlot();

        if (slot == -1)
        {
            cout << "\nParking is full.\n";

            if (v->getType() == "Emergency")
            {
                emergencyQueue.push(v);
                cout << "Emergency vehicle added to priority queue.\n";
            }
            else
            {
                normalQueue.push(v);
                cout << "Vehicle added to waiting queue.\n";
            }

            return;
        }

        slots[slot].park(v);

        cout << "\n============================================\n";
        cout << "          VEHICLE PARKED SUCCESSFULLY        \n";
        cout << "============================================\n";
        cout << "Vehicle Number : " << v->getNumber() << endl;
        cout << "Vehicle Type   : " << v->getType() << endl;
        cout << "Allocated Slot : " << slot + 1 << endl;
        cout << "============================================\n";
    }

    void checkout()
    {
        string number;

        cout << "\nEnter vehicle number for checkout: ";
        cin >> number;

        for (int i = 0; i < TOTAL_SLOTS; i++)
        {
            Vehicle *v = slots[i].getVehicle();

            if (v != NULL && v->getNumber() == number)
            {
                billing.showBill(v, i + 1);

                history.addRecord(
                    v->getNumber(),
                    v->getOwner(),
                    v->getType(),
                    i + 1,
                    v->getDuration(),
                    billing.calculateBill(v)
                );

                delete slots[i].removeVehicle();

                cout << "\nCheckout completed successfully.\n";

                allocateWaitingVehicle();
                return;
            }
        }

        cout << "\nVehicle not found in the parking area.\n";
    }

    void allocateWaitingVehicle()
    {
        int slot = findFreeSlot();

        if (slot == -1)
            return;

        Vehicle *v = NULL;

        if (!emergencyQueue.empty())
        {
            v = emergencyQueue.front();
            emergencyQueue.pop();
            cout << "\nPriority allocation: Emergency vehicle.\n";
        }
        else if (!normalQueue.empty())
        {
            v = normalQueue.front();
            normalQueue.pop();
            cout << "\nWaiting vehicle allocated a slot.\n";
        }

        if (v != NULL)
        {
            slots[slot].park(v);
            cout << "Vehicle Number : " << v->getNumber() << endl;
            cout << "Allocated Slot : " << slot + 1 << endl;
        }
    }

    void showWaitingQueue()
    {
        cout << "\n============================================\n";
        cout << "                WAITING QUEUE                \n";
        cout << "============================================\n";

        queue<Vehicle *> tempEmergency = emergencyQueue;
        queue<Vehicle *> tempNormal = normalQueue;

        cout << "\nEMERGENCY PRIORITY QUEUE\n";
        cout << "--------------------------------------------\n";

        if (tempEmergency.empty())
            cout << "No emergency vehicles waiting.\n";

        while (!tempEmergency.empty())
        {
            Vehicle *v = tempEmergency.front();
            cout << v->getNumber() << " - " << v->getType() << endl;
            tempEmergency.pop();
        }

        cout << "\nNORMAL WAITING QUEUE\n";
        cout << "--------------------------------------------\n";

        if (tempNormal.empty())
            cout << "No normal vehicles waiting.\n";

        while (!tempNormal.empty())
        {
            Vehicle *v = tempNormal.front();
            cout << v->getNumber() << " - " << v->getType() << endl;
            tempNormal.pop();
        }

        cout << "============================================\n";
    }
