#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <stdexcept>

class Restaurant {
    const std::string LAYOUT_FILE = "code/Reservation-Project/layout.txt";
    const std::string GUEST_FILE = "code/Reservation-Project/guest_info.txt";

public:
    std::map<std::string, std::vector<std::string>> readLayout() {
        std::map<std::string, std::vector<std::string>> layout;
        try {
            std::ifstream reader(LAYOUT_FILE);
            if (!reader) {
                throw std::runtime_error("File not found.");
            }
            std::string line;
            while (std::getline(reader, line)) {
                std::istringstream ss(line);
                std::string part;
                std::vector<std::string> parts;
                while (std::getline(ss, part, ',')) {
                    parts.push_back(part);
                }
                layout[parts[0]] = {parts[1], parts[2]};
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        return layout;
    }

    void updateLayout(const std::map<std::string, std::vector<std::string>>& layout) {
        try {
            std::ofstream writer(LAYOUT_FILE);
            if (!writer) {
                throw std::runtime_error("File not found.");
            }
            for (const auto& entry : layout) {
                writer << entry.first << "," << entry.second[0] << "," << entry.second[1] << "\n";
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void addPartyToWaitlist(const std::string &name, int size, const std::string &contact) {
        try {
            std::ofstream writer(GUEST_FILE, std::ios::app);
            if (writer.is_open()) {
                writer << name << "," << size << "," << contact << "\n";
                writer.close();
            } else {
                throw std::ios_base::failure("Error opening file");
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    std::vector<std::vector<std::string>> readWaitlist() {
        std::vector<std::vector<std::string>> waitlist;
        try {
            std::ifstream reader(GUEST_FILE);
            if (!reader.is_open()) {
                throw std::ios_base::failure("Error opening file");
            }
            std::string line;
            while (std::getline(reader, line)) {
                std::stringstream ss(line);
                std::vector<std::string> partyDetails;
                std::string detail;
                while (std::getline(ss, detail, ',')) {
                    partyDetails.push_back(detail);
                }
                waitlist.push_back(partyDetails);
            }
            reader.close();
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        return waitlist;
    }

    void seatParty() {
        std::vector<std::vector<std::string>> waitlist = readWaitlist();
        std::map<std::string, std::vector<std::string>> layout = readLayout();

        if (waitlist.empty()) {
            std::cout << "The waitlist is empty." << std::endl;
            return;
        }

        auto iterator = waitlist.begin();
        while (iterator != waitlist.end()) {
            std::vector<std::string> party = *iterator;
            std::string partyName = party[0];
            int partySize = std::stoi(party[1]);

            for (auto& table : layout) {
                std::vector<std::string> tableDetails = table.second;
                int tableSize = std::stoi(tableDetails[0]);
                std::string tableOccupant = tableDetails[1];
                if (tableSize >= partySize && tableOccupant == "null") {
                    // Seat the party at this table
                    layout[table.first] = {std::to_string(tableSize), partyName};
                    updateLayout(layout);

                    // Remove the party from the waitlist
                    iterator = waitlist.erase(iterator);
                    updateWaitlist(waitlist);

                    std::cout << "Seated party " << partyName << " at table " << table.first << std::endl;
                    return;
                }
            }
            iterator++;
        }

        std::cout << "No suitable table available for any party on the waitlist." << std::endl;
    }

    void updateWaitlist(const std::vector<std::vector<std::string>>& waitlist) {
        try {
            std::ofstream writer(GUEST_FILE);
            for (const auto& party : waitlist) {
                writer << party[0] << "," << party[1] << "," << party[2] << "\n";
            }
            writer.close();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void clearTable(const std::string &tableNumber) {
        auto layout = readLayout();

        if (layout.find(tableNumber) != layout.end()) {
            auto& tableDetails = layout[tableNumber];
            tableDetails[1] = "null"; // Mark the table as unoccupied
            updateLayout(layout);

            std::cout << "Table " << tableNumber << " has been cleared." << std::endl;
        } else {
            std::cout << "Table " << tableNumber << " does not exist." << std::endl;
        }
    }

    std::vector<std::string> viewWaitlist() {
        auto waitlist = readWaitlist();
        std::vector<std::string> waitlistInfo;

        for (const auto& party : waitlist) {
            waitlistInfo.push_back("Party Name: " + party[0] + ", Size: " + party[1] + ", Contact: " + party[2]);
        }

        return waitlistInfo;
    }

    std::vector<std::string> viewOccupiedTables() {
        auto layout = readLayout();
        std::vector<std::string> occupiedTables;

        for (const auto& entry : layout) {
            const std::string& tableNumber = entry.first;
            const auto& tableDetails = entry.second;
            if (tableDetails[1] != "null") {
                occupiedTables.push_back("Table " + tableNumber + ": Occupied by " + tableDetails[1]);
            }
        }

        return occupiedTables;
    }

};

int main() {
    Restaurant r;

    // Add parties to the waitlist
    r.addPartyToWaitlist("Juanita", 3, "246810");
    r.addPartyToWaitlist("George", 5, "121416");

    // View the occupied tables
    std::cout << "Occupied Tables:" << std::endl;
    for (const auto& info : r.viewOccupiedTables()) {
        std::cout << info << std::endl;
    }

    // Clear a table
    std::cout << std::endl;
    r.clearTable("1");

    // Try to seat parties
    std::cout << std::endl;
    r.seatParty();
    r.seatParty();

    // View the updated waitlist and occupied tables
    std::cout << "\nUpdated Waitlist:" << std::endl;
    for (const auto& info : r.viewWaitlist()) {
        std::cout << info << std::endl;
    }

    std::cout << "\nUpdated Occupied Tables:" << std::endl;
    for (const auto& info : r.viewOccupiedTables()) {
        std::cout << info << std::endl;
    }

    return 0;
};