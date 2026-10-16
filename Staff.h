#pragma once
#include "Person.h"

public ref class Staff : public Person
{
public:
    property String^ Position;

    Staff() : Person() { Position = L""; }
    Staff(String^ id, String^ name, String^ position, String^ phone, String^ email)
        : Person(id, name, phone, email)
    {
        Position = position;
    }

    virtual String^ Describe() override { return L"Staff " + Person::Describe() + L" (" + Position + L")"; }
};
