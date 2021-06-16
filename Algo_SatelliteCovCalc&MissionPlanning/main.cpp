//
//  main.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteSchedulePlanning.hpp"
int main(int argc, const char * argv[]) {
    
//    cout<<SatelliteCovArea::transformedX(1, 80)<<endl;
//    cout<<1 * cos(M_PI/180 * 80);
//    //第一问部分
    SatelliteInfoManagement m;
//
//    m.readSatInfoFile();
//    m.readTarInfoFile();
//    m.saveTarName(0);
////    cout<<m.all_satellite_timetable[0][43357].isInside_polygon(m.all_target_table[0][0]._pos)<<endl;
////    cout<<m.all_satellite_timetable[0][43357].isInside_polygon(EarthPos(255, 10))<<endl;
//
////    cout<<endl<<m.all_satellite_timetable[0][0].isInside_polygon(EarthPos(260, 3))<<endl;
//    m.coverCal(0);
//    m.calAllTargetCoverage();
    
    
//    //第二问部分
    SatelliteSchedulePlanning p;
    p.readSatCovWinFile(0);
    p.readTarInfoFile(1);
//
    p._setActivatedSat(vector<bool>{1,0,0,0,0,0,1,0,0});
//    p._setActivatedSat();
//    p._preprocessing();
    p.integerAlgo();
//    p.outputResult();
    return 0;
}
