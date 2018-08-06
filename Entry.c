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
  desc->points->push_back(plot->GetY());
  desc->param3->push_back(function->GetParam(3));
  desc->param4->push_back(function->GetParam(4));
  if(function->GetWidthB() >0.015) desc->trackBCID->push_back(desc->areaRatio->at(desc->areaRatio->size()-1));
  desc->chi2->push_back(function->GetFunction()->GetChisquare());

}

void AreaAnalysisDG(GaussianDesc* desc, Function *functiond,TGraphErrors* plot){
  DoubleGaussFunction * function = (DoubleGaussFunction*) functiond;
  desc->sGauss = false;
}

void Init(string fileName,string outputName,Double_t range,FrameComparision *ioE){
  int x = 1/0;
  BeamDataHandler *beamA = new BeamDataHandler(fileName.c_str(),"beamA",0);
  BeamDataHandler *beamB = new BeamDataHandler(fileName.c_str(),"beamB",1);
  BeamCollisionHandler output(beamA, beamB);
  DoubleGaussFunction * dfunction = new DoubleGaussFunction("DG",range);
  SingleGaussFunction * sfunction = new SingleGaussFunction("SG",range);
  string temp = outputName + "DG";
  AnalysisEngine doubleGauss(&output, dfunction,&doubleGaussF,temp,ioE);
  temp = fileName + "SingleGauss";
  AnalysisEngine singleGauss(&output, sfunction,&singleGaussF,temp,ioE);

}


void Entry(){
  TCanvasFileWriter * iowriter = new TCanvasFileWriter("comparisonPartFits","Fill_8011|15,Fill_8011|9,Fill_7082|15");
  FrameComparision * frame = new FrameComparision(iowriter);

  Init("scan1808010442","Fill_8011|15_C9_T",0.04,frame);
  //Init("scan1808010442","Fill_8011|15",1,frame);
  //Init("scan1808011349","Fill_8011|9",1,frame);
//  Init("scan1807082249","Fill_7082|15_C9",0.04,frame);
//  Init("scan1807082249","Fill_7082|15",1,frame);


  //Init("scan1806261336","Fill_6261_NTRAIN|9",1,frame);

  //Init("scan1805182231","ERRORCHECK_9a",1,frame);
  //Init("scan1805190017","ERRORCHECK_9b",1,frame);
  //Init("scan1805182255","ERRORCHECK_13",1,frame);

//  frame->Draw();
  iowriter->CloseFile();




};
