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
void priorityInformation()
    {
        cout << "\n============================================\n";
        cout << "           EMERGENCY PRIORITY SYSTEM         \n";
        cout << "============================================\n";
        cout << "Emergency vehicles are given priority.\n";
        cout << "They receive the next available slot before\n";
        cout << "vehicles in the normal waiting queue.\n";
        cout << "============================================\n";
    }

    void calculateCurrentBill()
    {
        string number;

        cout << "\nEnter vehicle number: ";
        cin >> number;

        for (int i = 0; i < TOTAL_SLOTS; i++)
        {
            Vehicle *v = slots[i].getVehicle();

            if (v != NULL && v->getNumber() == number)
            {
                billing.showBill(v, i + 1);
                return;
            }
        }

        cout << "\nVehicle not found in occupied slots.\n";
    }

    void displayRecords()
    {
        cout << "\n============================================================\n";
        cout << "                 CURRENT PARKING RECORDS                    \n";
        cout << "============================================================\n";

        bool found = false;

        for (int i = 0; i < TOTAL_SLOTS; i++)
        {
            Vehicle *v = slots[i].getVehicle();

            if (v != NULL)
            {
                if (!found)
                {
                    cout << left
                         << setw(15) << "VEHICLE NO."
                         << setw(20) << "OWNER"
                         << setw(15) << "TYPE"
                         << setw(10) << "HOURS"
                         << "RATE\n";

                    cout << "------------------------------------------------------------\n";
                }

                v->display();
                found = true;
            }
        }

        if (!found)
            cout << "No vehicles are currently parked.\n";

        cout << "============================================================\n";
    }

    void displayHistory()
    {
        history.displayRecords();
    }

    void saveRecords()
    {
        ofstream file("vehicles.txt");

        if (!file)
        {
            cout << "\nUnable to save vehicle records.\n";
            return;
        }

        for (int i = 0; i < TOTAL_SLOTS; i++)
        {
            Vehicle *v = slots[i].getVehicle();

            if (v != NULL)
            {
                file << v->getType() << "|"
                     << v->getNumber() << "|"
                     << v->getOwner() << "|"
                     << v->getDuration() << "|"
                     << i + 1 << endl;
            }
        }

        file.close();
        history.saveHistory();

        cout << "\nRecords saved successfully.\n";
    }

    void loadVehicles()
    {
        ifstream file("vehicles.txt");

        if (!file)
        {
            cout << "\nNo saved vehicle file found. Starting fresh.\n";
            history.loadHistory();
            return;
        }

        for (int i = 0; i < TOTAL_SLOTS; i++)
        {
            if (!slots[i].isEmpty())
                delete slots[i].removeVehicle();
        }

        string type, number, owner;
        int duration, slot;

        while (getline(file, type, '|'))
        {
            if (!getline(file, number, '|')) break;
            if (!getline(file, owner, '|')) break;
            if (!(file >> duration)) break;

            file.ignore(1000, '|');

            if (!(file >> slot)) break;

            file.ignore(1000, '\n');

            if (slot < 1 || slot > TOTAL_SLOTS)
                continue;

            if (!slots[slot - 1].isEmpty())
                continue;

            Vehicle *v = NULL;

            if (type == "Car")
                v = new Car(number, owner, duration);
            else if (type == "Bike")
                v = new Bike(number, owner, duration);
            else if (type == "Emergency")
                v = new EmergencyVehicle(number, owner, duration);

            if (v != NULL)
                slots[slot - 1].park(v);
        }

        file.close();
        history.loadHistory();

        cout << "\nSaved vehicle records loaded.\n";
    }

    void menu()
    {
        loadVehicles();

        int choice;

        do
        {
            int available = 0;
            int occupied = 0;

            for (int i = 0; i < TOTAL_SLOTS; i++)
            {
                if (slots[i].isEmpty())
                    available++;
                else
                    occupied++;
            }

            cout << "\n\n";
            cout << "+================================================+\n";
            cout << "|             PARKWISE SMART PARKING             |\n";
            cout << "|              MANAGEMENT SYSTEM                 |\n";
            cout << "+================================================+\n";
            cout << "| DASHBOARD                                      |\n";
            cout << "| Total Slots      : " << setw(2) << left
                 << TOTAL_SLOTS << "                         |\n";
            cout << "| Available Slots  : " << setw(2) << left
                 << available << "                         |\n";
            cout << "| Occupied Slots   : " << setw(2) << left
                 << occupied << "                         |\n";
            cout << "| Emergency Queue  : " << setw(2) << left
                 << emergencyQueue.size() << "                         |\n";
            cout << "| Normal Queue     : " << setw(2) << left
                 << normalQueue.size() << "                         |\n";
            cout << "+------------------------------------------------+\n";
            cout << "|                    MAIN MENU                   |\n";
            cout << "+------------------------------------------------+\n";
            cout << "|  1. Register Vehicle                            |\n";
            cout << "|  2. Display Parking Slots                       |\n";
            cout << "|  3. Park Vehicle / Allocate Slot                |\n";
            cout << "|  4. Remove Vehicle / Checkout                   |\n";
            cout << "|  5. Show Waiting Queue                          |\n";
            cout << "|  6. Priority Vehicle Handling                   |\n";
            cout << "|  7. Calculate Parking Charges                   |\n";
            cout << "|  8. Display Current Vehicle Records             |\n";
            cout << "|  9. Display Parking History                     |\n";
            cout << "| 10. Save Records                                |\n";
            cout << "| 11. Load Records                                |\n";
            cout << "| 12. Exit                                        |\n";
            cout << "+================================================+\n";

            cout << "Select an option (1-12): ";

            if (!(cin >> choice))
            {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "\nPlease enter a valid number.\n";
                continue;
            }

            switch (choice)
            {
                case 1:
                    registerVehicle();
                    break;

                case 2:
                    displaySlots();
                    break;

                case 3:
                {
                    Vehicle *v = createVehicle();

                    if (v != NULL)
                        parkVehicle(v);

                    break;
                }

                case 4:
                    checkout();
                    break;

                case 5:
                    showWaitingQueue();
                    break;

                case 6:
                    priorityInformation();
                    break;

                case 7:
                    calculateCurrentBill();
                    break;

                case 8:
                    displayRecords();
                    break;

                case 9:
                    displayHistory();
                    break;

                case 10:
                    saveRecords();
                    break;

                case 11:
                    loadVehicles();
                    break;

                case 12:
                    saveRecords();

                    cout << "\n";
                    cout << "+==========================================+\n";
                    cout << "|       THANK YOU FOR USING PARKWISE!       |\n";
                    cout << "|             DRIVE SAFELY                  |\n";
                    cout << "+==========================================+\n";
                    break;

                default:
                    cout << "\nInvalid choice! Please select 1 to 12.\n";
            }

            if (choice >= 1 && choice <= 11)
            {
                cout << "\nPress Enter to return to the main menu...";
                cin.ignore(1000, '\n');
                cin.get();
            }

        } while (choice != 12);
    }

    ~ParkingManager()
    {
        for (int i = 0; i < TOTAL_SLOTS; i++)
        {
            if (!slots[i].isEmpty())
                delete slots[i].removeVehicle();
        }

        while (!emergencyQueue.empty())
        {
            delete emergencyQueue.front();
            emergencyQueue.pop();
        }

        while (!normalQueue.empty())
        {
            delete normalQueue.front();
            normalQueue.pop();
        }
    }
};

int main()
{
    ParkingManager manager;
    manager.menu();

    return 0;
}