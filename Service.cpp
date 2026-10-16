#include "Database.h"
#include "Appointment.h"
#include <iostream>

Service::Service(int id, std::string name, double p, int duration)
    : serviceID(id), serviceName(name), price(p), durationMinutes(duration) {}

int Service::getServiceID() const { return serviceID; }
std::string Service::getName() const { return serviceName; }
double Service::getPrice() const { return price; }
int Service::getDuration() const { return durationMinutes; }

HairService::HairService(int id, std::string name, double p, int duration) 
    : Service(id, name, p, duration) {}

void HairService::showServiceInfo() const {
    std::cout << "[Hair Artistry] " << serviceName << " - LKR " << price << " (" << durationMinutes << " mins)\n";
}

SpaService::SpaService(int id, std::string name, double p, int duration) 
    : Service(id, name, p, duration) {}

void SpaService::showServiceInfo() const {
    std::cout << "[Veda Spa] " << serviceName << " - LKR " << price << " (" << durationMinutes << " mins)\n";
}