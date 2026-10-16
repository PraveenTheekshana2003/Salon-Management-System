#include "Database.h"
#include "Appointment.h"
#include <iostream>

Staff::Staff(int id, std::string n, std::string p, std::string e, std::string spec, double rate)
    : Person(n, p, e), staffID(id), specialization(spec), hourlyRate(rate) {}

void Staff::displayDetails() const {
    std::cout << "[Expert #" << staffID << "] " << name 
              << " (" << specialization << ") - Contact: " << phone << "\n";
}

int Staff::getStaffID() const { return staffID; }
std::string Staff::getSpecialization() const { return specialization; }