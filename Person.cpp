#include "Database.h"
#include "Appointment.h"

Person::Person(std::string n, std::string p, std::string e) 
    : name(n), phone(p), email(e) {}

std::string Person::getName() const { return name; }
std::string Person::getPhone() const { return phone; }
std::string Person::getEmail() const { return email; }