//
//  main.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteInfoManagement.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    SatelliteInfoManagement m;
    m.readSatInfoFile();
    m.getTime(3601, m.satellite_1_starttime);
    return 0;
}
