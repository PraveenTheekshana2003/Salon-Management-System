#include "Database.h"
#include "Appointment.h"
#include <iostream>

Customer::Customer(int id, std::string n, std::string p, std::string e, int points)
    : Person(n, p, e), customerID(id), loyaltyPoints(points) {}

void Customer::displayDetails() const {
    std::cout << "[Customer #" << customerID << "] " << name 
              << " | Phone: " << phone << " | Points: " << loyaltyPoints << "\n";
}

int Customer::getCustomerID() const { return customerID; }
int Customer::getLoyaltyPoints() const { return loyaltyPoints; }
void Customer::addLoyaltyPoints(int pts) { loyaltyPoints += pts; }