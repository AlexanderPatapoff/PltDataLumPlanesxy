#include "Classes.c"





void Entry(){

  BeamDataHandler *beamA = new BeamDataHandler("scan1806012201","beamA",0);
  BeamDataHandler *beamB = new BeamDataHandler("scan1806012201","beamB",1);
  BeamCollisionHandler output(beamA, beamB);
  SingleGaussFunction * function = new SingleGaussFunction("gauss",1);
  Engine plotter(&output, function,"scan1806012201");


  //IInit("scan1806021010");



};
