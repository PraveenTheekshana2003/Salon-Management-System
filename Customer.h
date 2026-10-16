#pragma once
#include "Person.h"

public ref class Customer : public Person
{
public:
    Customer() : Person() {}
    Customer(String^ id, String^ name, String^ phone, String^ email) : Person(id, name, phone, email) {}

    virtual String^ Describe() override { return L"Customer " + Person::Describe(); }
};
