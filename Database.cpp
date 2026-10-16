#include "Database.h"
#include "Appointment.h"

Database::Database() {
    seedInitialData();
}

Database::~Database() {
    for (auto s : services) {
        delete s;
    }
}

void Database::seedInitialData() {
    // Experts from Salon Zelora UI
    staffMembers.push_back(Staff(1, "Eliza", "077-1234567", "eliza@zelora.lk", "Master Stylist", 2500.0));
    staffMembers.push_back(Staff(2, "Sarah", "071-9876543", "sarah@zelora.lk", "Hair Specialist", 2000.0));
    staffMembers.push_back(Staff(3, "Michael", "070-5554433", "michael@zelora.lk", "Senior Stylist", 2200.0));
    staffMembers.push_back(Staff(4, "Leah", "075-1122334", "leah@zelora.lk", "Colorist & Spa Expert", 2100.0));

    // Services
    services.push_back(new HairService(101, "Signature Hair Cut & Styling", 3500.0, 45));
    services.push_back(new HairService(102, "Advanced Hair Coloring & Balayage", 8500.0, 120));
    services.push_back(new SpaService(103, "Veda Ayurvedic Scalp Tonic Treatment", 4500.0, 60));
    services.push_back(new SpaService(104, "Luxury Foot Sanctuary & Pedicure", 5000.0, 50));
}

const std::vector<Staff>& Database::getStaffList() const { return staffMembers; }
const std::vector<Service*>& Database::getServiceList() const { return services; }
const std::vector<Appointment>& Database::getAppointmentList() const { return appointments; }

void Database::addCustomer(const Customer& c) { customers.push_back(c); }
void Database::addAppointment(const Appointment& a) { appointments.push_back(a); }