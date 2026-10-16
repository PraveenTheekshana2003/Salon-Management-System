#pragma once
#include "Theme.h"

public ref class Appointment
{
public:
    property int      Id;
    property String^ CustomerName;
    property String^ CustomerEmail;
    property String^ Phone;
    property int      ServiceId;          // 0 = none
    property String^ StaffId;            // "" = no preference
    property DateTime AppointmentTime;
    property String^ Status;             // Pending / Confirmed / Completed / Cancelled
    property String^ Notes;

    Appointment()
    {
        Id = 0; CustomerName = L""; CustomerEmail = L""; Phone = L"";
        ServiceId = 0; StaffId = L"";
        AppointmentTime = DateTime::Now;
        Status = L"Pending"; Notes = L"";
    }
};
