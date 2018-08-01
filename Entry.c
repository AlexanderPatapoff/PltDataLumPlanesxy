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
  DoubleGaussFunction * dfunction = new DoubleGaussFunction("DG",0.2);
  SingleGaussFunction * sfunction = new SingleGaussFunction("SG",1);
  string temp = outputName + "DoubleGuass";
  AnalysisEngine doubleGauss(&output, dfunction,&doubleGaussF,temp);
  temp = fileName + "SingleGauss";
  //AnalysisEngine singleGauss(&output, sfunction,&singleGaussF,temp);

}


void Entry(){
  //TCanvasFileWriter * iowriter = new TCanvasFileWriter("comparison");
  //FrameComparison

  //9 points
  //Init("9pt1","9pt1PreceedErrors");
  //Init("9pt2","9pt2PreceedErrors");

  //13 point Scan
  //Init("13pt1","13pt1PreceedErrors");
  Init("13pt2","13pt2PreceedErrorsZOOM");

  //15 point Scan range
  //Init("15pt1","15pt1PreceedErrorsZOOMAGAIN");






};
