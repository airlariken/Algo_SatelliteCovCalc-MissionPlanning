//
//  main.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteSchedulePlanning.hpp"

int main(int argc, const char * argv[]) {
    
    SatelliteInfoManagement m;

    m.readSatInfoFile();
    m.readTarInfoFile();
//    cout<<m.all_satellite_timetable[0][43357].isInside_polygon(m.all_target_table[0][0]._pos)<<endl;
//    cout<<m.all_satellite_timetable[0][43357].isInside_polygon(EarthPos(255, 10))<<endl;
    
//    cout<<endl<<m.all_satellite_timetable[0][0].isInside_polygon(EarthPos(260, 3))<<endl;
    m.coverCal(0);
//    m.calAllTargetCoverage();
    return 0;
}
