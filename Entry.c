#include "Classes.c"







void singleGaussF(GaussianDesc* desc, Function *functiond,TGraphErrors* plot){
  SingleGaussFunction * function = (SingleGaussFunction*)functiond;
  desc->sGauss = true;
  desc->peak->push_back(function->GetFunction()->GetHistogram()->GetMaximum());
  desc->mean->push_back(function->GetFunction()->GetHistogram()->GetMean(1));
  desc->width->push_back(function->GetFunction()->GetHistogram()->GetRMS());
  desc->chi2->push_back(plot->Chisquare(function->GetFunction()));


}

void doubleGaussF(GaussianDesc* desc, Function *functiond,TGraphErrors* plot){
  DoubleGaussFunction * function = (DoubleGaussFunction*) functiond;
  desc->sGauss = false;
  desc->peak->push_back(function->GetFunction()->GetHistogram()->GetMaximum());
  desc->mean->push_back(function->GetFunction()->GetHistogram()->GetMean(1));
  desc->width->push_back(function->GetFunction()->GetHistogram()->GetRMS());
  desc->widthA->push_back(function->GetWidthA());
  desc->widthB->push_back(function->GetWidthB());
  desc->widthRatio->push_back(function->GetWidthRation());
  Float_t temp = function->GetAreaRatio(-1,1);
  desc->areaRatio->push_back(temp);
  desc->widthDifference->push_back(function->GetWidthDifference());
  //cout<<desc->areaRatio->at(0)<<

  desc->chi2->push_back(function->GetFunction()->GetChisquare());

}

void Init(string fileName,string outputName){
  int x = 1/0;
  BeamDataHandler *beamA = new BeamDataHandler(fileName.c_str(),"beamA",0);
  BeamDataHandler *beamB = new BeamDataHandler(fileName.c_str(),"beamB",1);
  BeamCollisionHandler output(beamA, beamB);
  DoubleGaussFunction * dfunction = new DoubleGaussFunction("DG",1);
  SingleGaussFunction * sfunction = new SingleGaussFunction("SG",1);
  string temp = outputName + "DG";
  AnalysisEngine doubleGauss(&output, dfunction,&doubleGaussF,temp);
  temp = fileName + "SingleGauss";
  //AnalysisEngine singleGauss(&output, sfunction,&singleGaussF,temp);

}


void Entry(){
  //TCanvasFileWriter * iowriter = new TCanvasFileWriter("comparison");
  //FrameComparison

  Init("scan1808010442","Fill_8011|15");
  Init("scan1808011349","Fill_8011|9");
  Init("scan1807082249","Fill_7082|15");

  Init("scan1806261336","Fill_6261_NTRAIN|9");

  Init("scan1805182231","ERRORCHECK_9a");
  Init("scan1805190017","ERRORCHECK_9b");
  Init("scan1805182255","ERRORCHECK_13");





};
