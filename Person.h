#pragma once
#include "Theme.h"

// Base class for people stored in the system
public ref class Person
{
public:
    property String^ Id;
    property String^ Name;
    property String^ Phone;
    property String^ Email;

    Person() { Id = L""; Name = L""; Phone = L""; Email = L""; }
    Person(String^ id, String^ name, String^ phone, String^ email)
    {
        Id = id; Name = name; Phone = phone; Email = email;
    }

    virtual String^ Describe() { return Id + L" - " + Name; }
};
