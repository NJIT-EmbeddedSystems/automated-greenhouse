#include <iostream>
#include "greencontrol.h"
using namespace std;

//

plant::plant(String name, int water, double plantMin, double plantMax)
{
    name = name;
    waterNeeds = water; //Different types of value. So 0 = none, 3 = high
    minTemp = plantMin;
    maxTemp = plantMax;
}

int plant::indicate_water_lvl()
{
    return WATER_SENSOR;
}

void plant::set_water(int waterLevel)
{
    switch (waterLevel)
    {
        case 0:
            cout << "setting water level to NONE" << endl;
            break;

        case 1:
            cout << "setting water level to LOW" << endl;
            break;

        case 2:
            cout << "setting water level to MEDIUM" << endl;
            break;

        case 3:
            cout << "setting water level to HIGH" << endl;
            break;
    }

    if (get_temp() < MIN_WATER_TEMP || get_temp() > MAX_WATER_TEMP ) {
        temp_adjust(MIN_WATER_TEMP, MAX_WATER_TEMP);
    }
}

double plant::get_temp()
{
    return TEMP_SENSOR;
}

void plant::temp_adjust(double minTemp, double maxTemp)
{
    if (TEMP_SENSOR < minTemp) {
        UV_LIGHT = 1; //UV_LIGHT turns on
    }

    else if (TEMP_SENSOR > maxTemp) {
        UV_LIGHT = 0; //UV_LIGHT shuts off when it passes temeprature
    }
    else {
        UV_LIGHT = 2; //something where UV_LIGHT auto adjust temps
    }
}
