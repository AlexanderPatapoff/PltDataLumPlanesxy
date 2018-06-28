#include "Functions.c"

void Entry(){
  TFile *filein = new TFile("file:~/Downloads/scan.root","READ");
  TTree *tree = (TTree*) filein->Get("vdMScanData;1");
  Int_t nentries = tree->GetEntries();

  vector<BeamData> *beamA = new vector<BeamData>(nentries);
  vector<BeamData> *beamB = new vector<BeamData>(nentries);

  sortBeamDataB(beamB,tree);
  sortBeamDataA(beamA,tree);


  const vector<vector<Int_t>> *B1BCID = GetB1CIDA(tree);
  const vector<vector<Int_t>> *B2BCID = GetB2CIDB(tree);

  vector<vector<Float_t>>* collisionsA = SortCollisions(beamA, B1BCID);
  vector<vector<Float_t>>* collisionsB = SortCollisions(beamB, B2BCID);

  GraphTotalAvgCollisions(beamA,beamB,collisionsA,collisionsB);

  TF1* fitX = GetXFit(beamA,beamB,collisionsA,collisionsB);
  TF1* fitY = GetXFit(beamA,beamB,collisionsA,collisionsB);

  PlotBCIDs_Fit(beamB,beamA,collisionsA,collisionsB,fitX,fitY,B1BCID,B2BCID);






  const vector<vector<Float_t>> *PeaksX = GetPeaks()
  const vector<vector<Float_t>> *PeaksY = getPeaksFromFits



};
