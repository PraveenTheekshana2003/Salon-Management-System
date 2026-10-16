#pragma once
#include "Theme.h"

public ref class Service
{
public:
    property int     Id;
    property String^ Name;
    property String^ Description;
    property Decimal Price;
    property int     DurationMinutes;

    Service()
    {
        Id = 0; Name = L""; Description = L"";
        Price = Decimal(0); DurationMinutes = 60;
    }
};
