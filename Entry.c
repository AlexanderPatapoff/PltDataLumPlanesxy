#include "Classes.c"





void Entry(){

  BeamDataHandler *beamA = new BeamDataHandler("scan1806012201","beamA",0);
  BeamDataHandler *beamB = new BeamDataHandler("scan1806012201","beamA",1);
  BeamCollisionHandler output(beamA, beamB);



  //
  //IInit("scan1806021010");



};
