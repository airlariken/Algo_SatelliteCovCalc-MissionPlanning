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
    m.readTarInfoFile();
//    m.getTime(3601, m.satellite_1_starttime);
    cout<<endl<<m.all_satellite_timetable[0][0].isInside_polygon(EarthPos(260, 3))<<endl;
    m.coverCal(0);
    return 0;
}
