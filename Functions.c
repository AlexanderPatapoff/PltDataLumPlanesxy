#include "ExportData.c"

#include <vector>
struct BeamData{
  Float_t scanRun;
  Int_t scanLumBlock;
  Int_t scanStep;
  char plane;
  Float_t planeCoord;
  int beamnumber;
  vector<Float_t> * lumData;
};
void printBeam(BeamData * id){

  cout <<id->scanRun <<" " <<id->scanLumBlock << " "<<id->scanStep << " "<< id->plane<< " "<< id->planeCoord << endl;

};
void Test(){
  cout << "test" << endl;
}


void sortBeamDataA(vector<BeamData> * data, TTree* tree){
 cout<< "sorting Beam data A" << endl;
  Int_t nentries = tree->GetEntries();
  for (size_t i = 0; i < nentries; i++) {

    tree->GetEntry(i);
    data->at(i).scanRun = tree->GetLeaf("ScanRun")->GetValue(0);
    data->at(i).scanLumBlock = tree->GetLeaf("ScanLB")->GetValue(0);
    data->at(i).scanStep = tree->GetLeaf("ScanStep")->GetValue(0);
    if( i >= 9){
      data->at(i).plane = 'x';
      data->at(i).planeCoord = tree->GetLeaf("B1DeltaXSet")->GetValue(0);

    }else{
      data->at(i).plane = 'y';
      data->at(i).planeCoord = tree->GetLeaf("B1DeltaYSet")->GetValue(0);
    }

    Int_t nBaskets = tree->GetLeaf("lucBiHitOR_BunchInstLumi")->GetLen();

    data->at(i).lumData = new vector<Float_t>(nBaskets);
    cout << data->at(i).lumData->size() << endl;
    for (size_t s = 0; s < nBaskets; s++) {
      data->at(i).lumData->at(s) = tree->GetLeaf("lucBiHitOR_BunchInstLumi")->GetValue(s);
    }
    cout << "."<<endl;
  }
};

void sortBeamDataB(vector<BeamData> * data, TTree* tree){
 cout<< "sorting Beam data B" << endl;
  Int_t nentries = tree->GetEntries();
  for (size_t i = 0; i < nentries; i++) {

    tree->GetEntry(i);
    data->at(i).scanRun = tree->GetLeaf("ScanRun")->GetValue(0);
    data->at(i).scanLumBlock = tree->GetLeaf("ScanLB")->GetValue(0);
    data->at(i).scanStep = tree->GetLeaf("ScanStep")->GetValue(0);
    if( i >= 9){
      data->at(i).plane = 'x';
      data->at(i).planeCoord = tree->GetLeaf("B2DeltaXSet")->GetValue(0);

    }else{
      data->at(i).plane = 'y';
      data->at(i).planeCoord = tree->GetLeaf("B2DeltaYSet")->GetValue(0);
    }

    Int_t nBaskets = tree->GetLeaf("lucBi2HitOR_BunchInstLumi")->GetLen();

    data->at(i).lumData = new vector<Float_t>(nBaskets);
    for (size_t s = 0; s < nBaskets; s++) {
      data->at(i).lumData->at(s)= tree->GetLeaf("lucBi2HitOR_BunchInstLumi")->GetValue(s);
    }
    cout << "."<<endl;
  }
};

vector<vector<Int_t>> *  GetB1CIDA(TTree* tree){
  cout << "Retrieving B1BCIDs" << endl;
  tree->GetEntry(0);
  Int_t nBaskets = tree->GetLeaf("LuminousBCIDs")->GetLen();

  vector<vector<Int_t>> * BCID = new vector<vector<Int_t>>(18, vector<Int_t>(nBaskets));

  for (size_t p = 0; p < 18; p++) {
    tree->GetEntry(p);
    for (size_t i = 0; i < nBaskets; i++) {
       BCID->at(p).at(i) = tree->GetLeaf("LuminousBCIDs")->GetValue(i);
    }
  }

 return BCID;
};

vector<vector<Int_t>> *  GetB2CIDB(TTree* tree){
  cout << "Retrieving B2BCIDs" << endl;
  tree->GetEntry(0);
  Int_t nBaskets = tree->GetLeaf("LuminousBCIDs")->GetLen();

  vector<vector<Int_t>> * BCID = new vector<vector<Int_t>>(18, vector<Int_t>(nBaskets));
  for (size_t p = 0; p < 18; p++) {
    tree->GetEntry(p);
    for (size_t i = 0; i < nBaskets; i++) {
       BCID->at(p).at(i)= tree->GetLeaf("LuminousBCIDs")->GetValue(i);

    }
  }

 return BCID;
};

vector<vector<Float_t>>* SortCollisions(vector<BeamData> *beam,const vector<vector<Int_t>> *BNBCID){

  int y = beam->size();
  int z = BNBCID->at(0).size();
  vector<vector<Float_t>> * collisions = new vector<vector<Float_t>> (y,vector<Float_t>(z));

  for (size_t i = 0; i < y; i++) {
    for (size_t p = 0; p < z; p++) {
      collisions->at(i).at(p)= beam->at(i).lumData->at(BNBCID->at(i).at(p));
    }
  }

  return collisions;
};


void GraphTotalAvgCollisions(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionsA,vector<vector<Float_t>>* collisionsB){
  cout <<"graphing total collisions" << endl;
  size_t t = collisionsB->size()/2;

  Float_t x[t], y[t],zy[t],zx[t];

    for (size_t i = 0; i < t; i++) {
      zy[i] = 0;
      y[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionsA->at(i).size(); p++) {
        zy[i] +=  collisionsA->at(i).at(p);

      }

      for (size_t s  = 0; s < collisionsB->at(i).size(); s++) {

        zy[i] += collisionsB->at(i).at(s);

      }
      zy[i] = zy[i]/(2*collisionsB->at(i).size());
    }



    for (size_t i = t; i < 2*t; i++) {
      zx[i-t]=0;
      x[i-t] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionsA->at(i).size(); p++) {
        zx[i-t] += collisionsA->at(i).at(p);
      }



      for (size_t p = 0; p < collisionsB->at(i).size(); p++) {
        zx[i-t] += collisionsB->at(i).at(p);
      }
      zx[i-t] = zx[i-t]/(2*collisionsB->at(i).size());
    }






  TCanvas *cx = new TCanvas("cx","xPlaneCollisions",200,10,500,300);
  TGraph* gx = new TGraph(9,x,zx);
  gx->SetTitle("X-TotalAvgCollisions");
  TF1 *f1 = new TF1("f1","gaus",-0.1,0.1);
  gx->Fit("f1","R");
  gx->Draw("AP*");


  TCanvas *cy = new TCanvas("cy","yPlaneCollisions",200,340,500,300);
  TGraph* gy = new TGraph(9,y,zy);
  gy->SetTitle("Y-TotalAvgCollisions");
  TF1 *f2 = new TF1("f2","gaus",-0.1,0.1);
  gy->Fit("f2","R");
  gy->Draw("AP*");

};

TF1* GetXFit(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionsA,vector<vector<Float_t>>* collisionsB){
    size_t t = collisionsB->size()/2;
    Float_t x[t], y[t],zy[t],zx[t];
    for (size_t i = t; i < 2*t; i++) {
      zx[i-t]=0;
      x[i-t] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionsA->at(i).size(); p++) {
        zx[i-t] += collisionsA->at(i).at(p);
      }



      for (size_t p = 0; p < collisionsB->at(i).size(); p++) {
        zx[i-t] += collisionsB->at(i).at(p);
      }
      zx[i-t] = zx[i-t]/(2*collisionsB->at(i).size());
    }


    TGraph* gx = new TGraph(9,x,zx);
    gx->SetTitle("X-Collisions");
    TF1 *f1 = new TF1("f1","gaus",-0.1,0.1);
    gx->Fit("f1","R");
    return gx->GetFunction("f1");

};

TF1* GetYFit(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionsA,vector<vector<Float_t>>* collisionsB){
    size_t t = collisionsB->size()/2;
    Float_t x[t], y[t],zy[t],zx[t];


    for (size_t i = 0; i < t; i++) {
      zy[i] = 0;
      y[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionsA->at(i).size(); p++) {
        zy[i] +=  collisionsA->at(i).at(p);

      }

      for (size_t s  = 0; s < collisionsB->at(i).size(); s++) {

        zy[i] += collisionsB->at(i).at(s);
      }
      zy[i] = zy[i]/(2*collisionsB->at(i).size());
    }


    TGraph* gy = new TGraph(9,y,zy);
    gy->SetTitle("Y-Collisions");
    TF1 *f2 = new TF1("f2","gaus",-0.1,0.1);
    gy->Fit("f2","R");
    return gy->GetFunction("f2");

};

void PlotBCIDs_Fit(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionA,vector<vector<Float_t>>* collisionB, TF1* fitX,TF1* fitY,const vector<vector<Int_t>> *B1BCID,const vector<vector<Int_t>> *B2BCID){
  Test();
  TCanvas *canvasX[collisionA->at(0).size()];
  TCanvas *canvasY[collisionA->at(0).size()];
  size_t t = collisionA->size()/2;
  Float_t x[t], y[t],zy[t],zx[t];
  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < t; p++) {
      y[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;
      x[p] = beamA->at(t+p).planeCoord-beamB->at(t+p).planeCoord;

      zx[p]=0;
      zy[p] = 0;


      zx[p] += collisionA->at((t+p)).at(i);
      zx[p] += collisionB->at(t+p).at(i);
      zx[p] = zx[p]/2;

      zy[p] +=  collisionA->at(p).at(i);
      zy[p] += collisionB->at(p).at(i);
      zy[p] = zy[p]/2;





    }
    string x1 = "X:" + to_string(B1BCID->at(0).at(i));
    string y1 = "Y:" + to_string(B1BCID->at(0).at(i));
    const char* namex = x1.c_str();
    const char* namey = y1.c_str();

    canvasX[i] = new TCanvas(namex,namex,200,10,500,300);
    TGraph* gx = new TGraph(9,x,zx);

    gx->SetTitle(namex);
    gx->Draw("AP*");
    gx->GetYaxis()->SetRangeUser(0,5);

    canvasX[i]->Update();
    fitX->Draw("SAME");



    canvasY[i] = new TCanvas(namey,namey,200,340,500,300);
    TGraph* gy = new TGraph(9,y,zy);
    gy->SetTitle(namey);

    gy->Draw("AP*");
    gy->GetYaxis()->SetRangeUser(0,5);

    canvasY[i]->Update();
    fitY->Draw("SAME");

  }

//SaveCanvasPNG("X_BCID_FIT", canvasX,collisionA->at(0).size());
//SaveCanvasPNG("Y_BCID_FIT", canvasY,collisionA->at(0).size());

};
