#include "Classes.c"
#include "Functions.c"


void Init(string fileName){
  string fileLocation = "file:~/Downloads/HEPP/" + fileName+".root";
  TCanvasFileWriter * iobuffer = new TCanvasFileWriter(fileName);
  TFile *filein = new TFile(fileLocation.c_str(),"READ");
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

  TF1* fitX = GetXFit(beamA,beamB,collisionsA,collisionsB);
  TF1* fitY = GetYFit(beamA,beamB,collisionsA,collisionsB);

  iobuffer->OpenFile();

  PlotMeansXY(beamA,beamB,collisionsA,collisionsB,B1BCID,B2BCID,iobuffer);
  PlotWidthXY(beamA,beamB,collisionsA,collisionsB,B1BCID,B2BCID,iobuffer);
  //PlotFitsXY(beamA,beamB,collisionsA,collisionsB,B1BCID,B2BCID,iobuffer);
  PlotChi2_DOG_XY(fitX,fitY,beamA,beamB,collisionsA,collisionsB,B1BCID,B2BCID,iobuffer);

  iobuffer->CloseFile();
};


void Entry(){

  Init("scan1806012201");
  Init("scan1806021010");



};
