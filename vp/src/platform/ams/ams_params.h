#ifndef AMS_PARAMS_H
#define AMS_PARAMS_H

// HEAT SYSTEM PARAMS
#define airMass_m 0.001269 // [kg] - m = heated air mass
#define specificHeat_c_p 1005// [J/(K*kg)] - c_p = specific heat capacity of air
#define transferCoeff_alpha 5.6// [W/(K*m^2)] heat transfer coefficient
#define airVol_area 0.06// [m^2] - area of heated air volume-mass
#define init_T_0 0.0// [K] -- inital temperature, usually ambient

// HEAT SOURCE PARAMS
#define source_heaterWatts 5.0

#endif // AMS_PARAMS_H
