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

  CollisionData collisionA;
  CollisionData collisionB;


  collisionA.BCID = GetB1CIDA(tree);
  collisionB.BCID = GetB2CIDB(tree);

  collisionA.LumData = SortCollisions(beamA, collisionA.BCID);
  collisionB.LumData = SortCollisions(beamB, collisionB.BCID);

  TF1* fitX = GetXFit(beamA,beamB,&collisionA,&collisionB);
  TF1* fitY = GetYFit(beamA,beamB,&collisionA,&collisionB);

  iobuffer->OpenFile();

  PlotMeansXY(beamA,beamB,&collisionA,&collisionB,iobuffer);
  PlotWidthXY(beamA,beamB,&collisionA,&collisionB,iobuffer);
  //PlotFitsXY(beamA,beamB,collisionsA,collisionsB,B1BCID,B2BCID,iobuffer);
  //PlotChi2_DOG_XY(fitX,fitY,beamA,beamB,collisionA,collisionB,B1BCID,B2BCID,iobuffer);//REDO





  iobuffer->CloseFile();
};


void Compare(string fileName1, string fileName2){
  string fileLocation1 = "file:~/Downloads/HEPP/" + fileName1+".root";
  TCanvasFileWriter * iobuffer = new TCanvasFileWriter(fileName1);
  TFile *filein1 = new TFile(fileLocation1.c_str(),"READ");
  TTree *tree1 = (TTree*) filein1->Get("vdMScanData;1");
  Int_t nentries1 = tree1->GetEntries();


  string fileLocation2 = "file:~/Downloads/HEPP/" + fileName2+".root";
  TFile *filein2 = new TFile(fileLocation2.c_str(),"READ");
  TTree *tree2 = (TTree*) filein2->Get("vdMScanData;1");
  Int_t nentries2 = tree2->GetEntries();


  CollisionData collisionA1;
  CollisionData collisionB1;

  CollisionData collisionA2;
  CollisionData collisionB2;


  vector<BeamData> *beamA1 = new vector<BeamData>(nentries1);
  vector<BeamData> *beamB1 = new vector<BeamData>(nentries1);

  vector<BeamData> *beamA2 = new vector<BeamData>(nentries2);
  vector<BeamData> *beamB2 = new vector<BeamData>(nentries2);


  sortBeamDataB(beamB1,tree1);
  sortBeamDataA(beamA1,tree1);

  sortBeamDataB(beamB2,tree2);
  sortBeamDataA(beamA2,tree2);


  collisionA1.BCID = GetB1CIDA(tree1);
  collisionB1.BCID = GetB2CIDB(tree1);

  collisionA1.LumData = SortCollisions(beamA1, collisionA1.BCID);
  collisionB1.LumData = SortCollisions(beamB1, collisionB1.BCID);

  collisionA2.BCID = GetB1CIDA(tree2);
  collisionB2.BCID = GetB2CIDB(tree2);

  collisionA2.LumData = SortCollisions(beamA2, collisionA2.BCID);
  collisionB2.LumData = SortCollisions(beamB2, collisionB2.BCID);


  ComparePeaks(&collisionA1,&collisionB1,&collisionA2,&collisionB2,iobuffer);
  ComparePoints(3,4,&collisionA1,&collisionB1,NULL);
  CompareAllPoints(&collisionA1,&collisionB1,NULL);
  CompareAllPoints(&collisionA2,&collisionB2,NULL);

}

void Entry(){

  //Init("scan1806012201");
  //Init("scan1806021010");

  Compare("scan1806012201","scan1806021010");



};
