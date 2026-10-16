#include "Database.h"
#include "Appointment.h"
#include <iostream>

Appointment::Appointment(int id, Customer c, Staff s, Service* serv, std::string dt, std::string stat)
    : appointmentID(id), customer(c), staff(s), service(serv), dateTime(dt), status(stat) {}

void Appointment::printAppointmentDetails() const {
    std::cout << "Appointment #" << appointmentID << " | Date: " << dateTime 
              << " | Customer: " << customer.getName() << " | Expert: " << staff.getName()
              << " | Service: " << service->getName() << " | Status: " << status << "\n";
}

int Appointment::getAppointmentID() const { return appointmentID; }
std::string Appointment::getDateTime() const { return dateTime; }
std::string Appointment::getCustomerName() const { return customer.getName(); }
std::string Appointment::getStaffName() const { return staff.getName(); }
std::string Appointment::getServiceName() const { return service->getName(); }
double Appointment::getTotalAmount() const { return service->getPrice(); }