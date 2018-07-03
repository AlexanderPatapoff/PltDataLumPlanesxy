
#include <vector>
#include "ExportData.c"
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
    for (size_t s = 0; s < nBaskets; s++) {
      data->at(i).lumData->at(s) = tree->GetLeaf("lucBiHitOR_BunchInstLumi")->GetValue(s);
    }
    if (i%3 == 0) cout << "."<<endl;
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
    if (i%3 == 0) cout << "."<<endl;
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

void PlotBCIDs_Fit(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionA,vector<vector<Float_t>>* collisionB, TF1* fitX,TF1* fitY,const vector<vector<Int_t>> *B1BCID,const vector<vector<Int_t>> *B2BCID, ExportDataHandler * saveData){
  TCanvas *canvasX;
  TCanvas *canvasY;
  canvasX = new TCanvas("initx","welcome",200,340,500,300);
  canvasX->Print("testX.pdf(");
  canvasY = new TCanvas("inity","welcome",200,340,500,300);
  canvasY->Print("testY.pdf(");
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

    canvasX = new TCanvas(namex,namex,200,10,500,300);
    TGraph* gx = new TGraph(9,x,zx);

    gx->SetTitle(namex);
    gx->Draw("AP*");
    gx->GetYaxis()->SetRangeUser(0,5);

    canvasX->Update();
    fitX->Draw("SAME");

    canvasX->Print("testX.pdf");

    canvasY = new TCanvas(namey,namey,200,340,500,300);
    TGraph* gy = new TGraph(9,y,zy);
    gy->SetTitle(namey);
    gy->Draw("AP*");
    gy->GetYaxis()->SetRangeUser(0,5);

    canvasY->Update();
    fitY->Draw("SAME");

    canvasY->Print("testY.pdf");
    delete canvasX;
    delete canvasY;
  }
  canvasX = new TCanvas("initx","welcome",200,340,500,300);
  canvasX->Print("testX.pdf)");
  canvasY = new TCanvas("inity","welcome",200,340,500,300);
  canvasY->Print("testY.pdf)");

};

void PlotMeansXY(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionA,vector<vector<Float_t>>* collisionB,const vector<vector<Int_t>> *B1BCID,const vector<vector<Int_t>> *B2BCID,TCanvasFileWriter *writer){

  vector<Float_t> * mean = new vector<Float_t>(collisionA->at(0).size());
  Float_t x[9], y[9];
  cout << collisionA->size()<<endl;
  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(9+p).at(i);
      x[p] = beamA->at(9+p).planeCoord-beamB->at(9+p).planeCoord;


    }
    TCanvas * canvasY = new TCanvas("namey","namey",200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle("Mean Test");

    plot->Draw("AP*");
    plot->Fit("h1","L");
    plot->Draw("AP*");
    //function->Draw("SAME");
    //plot->Draw("AP*");
    //function->Draw("SAME");
    cout <<function->GetHistogram()->GetBarWidth()<<endl;
    mean->at(i) = function->GetHistogram()->GetMean(1);
    delete canvasY;

  }



  float z1[B1BCID->at(0).size()];
  float z2[B1BCID->at(0).size()];

    copy(mean->begin(), mean->end(), z1);
    copy(B1BCID->at(0).begin(), B1BCID->at(0).end(), z2);
    TCanvas * canvasY = new TCanvas("namey","namey",200,340,500,300);
    TGraph* plot = new TGraph(B1BCID->at(0).size(),z2,z1);

    plot->SetTitle("MeanX by BCID");

  //  plot->Draw("AP");
    plot->SetMarkerStyle(20);
    plot->SetMarkerSize(0.5);
    plot->SetMarkerColor(2);
    plot->Draw("AP");
    writer->Write(canvasY);


    mean = new vector<Float_t>(collisionA->at(0).size());
    for (size_t i = 0; i < collisionA->at(0).size(); i++) {
      for (size_t p = 0; p < collisionA->size()/2; p++) {
        x[p] = 0;
        y[p] = 0;
        y[p] = collisionA->at(p).at(i);
        x[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;
      }
      TCanvas * canvasY = new TCanvas("namey","namey",200,340,500,300);
      TGraph* plot = new TGraph(9,x,y);
      TF1* function = new TF1("h1","gaus",-1,1);
      plot->SetTitle("voidMean Test");

      plot->Draw("AP*");
      plot->Fit("h1","L");
      plot->Draw("AP*");
      //function->Draw("SAME");
      //plot->Draw("AP*");
      //function->Draw("SAME");
      cout <<function->GetHistogram()->GetMean(1)<<endl;
      mean->at(i) = function->GetHistogram()->GetMean(1);
      delete canvasY;

    }

    copy(mean->begin(), mean->end(), z1);
    copy(B1BCID->at(0).begin(), B1BCID->at(0).end(), z2);
    canvasY = new TCanvas("namey","namey",200,340,500,300);
    plot = new TGraph(B1BCID->at(0).size(),z2,z1);

    plot->SetTitle("MeanY by BCID");




    //canvasY->cd(4);
    plot->Draw("AP");
    plot->SetMarkerStyle(20);
    plot->SetMarkerSize(0.4);
    plot->SetMarkerColor(2);
    plot->Draw("AP");
    writer->Write(canvasY);



}

void PlotFitsXY(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionA,vector<vector<Float_t>>* collisionB,const vector<vector<Int_t>> *B1BCID,const vector<vector<Int_t>> *B2BCID,TCanvasFileWriter *writer){

  Float_t x[9], y[9];
  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(9+p).at(i);
      x[p] = beamA->at(9+p).planeCoord-beamB->at(9+p).planeCoord;


    }
    string x1 = "X:" + to_string(B1BCID->at(0).at(i));

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(3);
    plot->SetMarkerColor(1);

    plot->Draw("AP");

    function->Draw("SAME");
    writer->Write(canvasY);



    delete canvasY;

  }



  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(p).at(i);
      x[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;


    }

    string x1 = "Y:" + to_string(B1BCID->at(0).at(i));

    const char* namex = x1.c_str();



    TCanvas * canvasX = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(3);
    plot->SetMarkerColor(1);

    plot->Draw("AP");

    function->Draw("SAME");
    writer->Write(canvasX);
    delete canvasX;

  }
}


void PlotWidthXY(vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionA,vector<vector<Float_t>>* collisionB,const vector<vector<Int_t>> *B1BCID,const vector<vector<Int_t>> *B2BCID,TCanvasFileWriter *writer){

  vector<Double_t> * width = new vector<Double_t>(collisionA->at(0).size());
  Float_t x[9], y[9];
  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(9+p).at(i);
      x[p] = beamA->at(9+p).planeCoord-beamB->at(9+p).planeCoord;


    }
    string x1 = "X:" + to_string(B1BCID->at(0).at(i));

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(3);
    plot->SetMarkerColor(1);

    plot->Draw("AP*");



    width->at(i) = function->GetHistogram()->GetStdDev();

    delete canvasY;

  }
  float z1[B1BCID->at(0).size()];
  float z2[B1BCID->at(0).size()];

  copy(width->begin(), width->end(), z1);
  copy(B1BCID->at(0).begin(), B1BCID->at(0).end(), z2);
  cout<<z1<<endl;
  TCanvas * widthC = new TCanvas("WidthX","namex",200,340,500,300);
  TGraph* plot = new TGraph(B1BCID->at(0).size(),z2,z1);
  plot->SetTitle("Width X");
  plot->Draw("AP");

  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.4);
  plot->SetMarkerColor(2);
  plot->Draw("AP");

  writer->Write(widthC);







  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(p).at(i);
      x[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;


    }
    string x1 = "Y:" + to_string(B1BCID->at(0).at(i));

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(3);
    plot->SetMarkerColor(1);

    plot->Draw("AP*");



    width->at(i) = function->GetHistogram()->GetStdDev();

    delete canvasY;

  }

  copy(width->begin(), width->end(), z1);
  copy(B1BCID->at(0).begin(), B1BCID->at(0).end(), z2);

  widthC = new TCanvas("WidthX","namex",200,340,500,300);
  plot = new TGraph(B1BCID->at(0).size(),z2,z1);
  plot->SetTitle("Width Y");
  plot->Draw("AP");

  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.4);
  plot->SetMarkerColor(2);
  plot->Draw("AP");

  writer->Write(widthC);


}


void PlotChi2_DOG_XY(TF1* TotalFitx,TF1* TotalFity,vector<BeamData> *beamA,vector<BeamData> *beamB,vector<vector<Float_t>>* collisionA,vector<vector<Float_t>>* collisionB,const vector<vector<Int_t>> *B1BCID,const vector<vector<Int_t>> *B2BCID,TCanvasFileWriter *writer){
  int DOG = 9-2;
  vector<Double_t> * chi = new vector<Double_t>(collisionA->at(0).size());
  Float_t x[9], y[9];
  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(9+p).at(i);
      x[p] = beamA->at(9+p).planeCoord-beamB->at(9+p).planeCoord;


    }
    string x1 = "chi X:" + to_string(B1BCID->at(0).at(i));

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(3);
    plot->SetMarkerColor(1);

    plot->Draw("AP*");



    chi->at(i) = function->GetHistogram()->Chisquare(TotalFity);

    delete canvasY;

  }
  float z1[B1BCID->at(0).size()];
  float z2[B1BCID->at(0).size()];

  copy(chi->begin(), chi->end(), z1);
  copy(B1BCID->at(0).begin(), B1BCID->at(0).end(), z2);
  cout<<z1<<endl;
  TCanvas * widthC = new TCanvas("WidthX","namex",200,340,500,300);
  TGraph* plot = new TGraph(B1BCID->at(0).size(),z2,z1);
  plot->SetTitle("chi X");
  plot->Draw("AP");

  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.4);
  plot->SetMarkerColor(2);
  plot->Draw("AP");

  writer->Write(widthC);




  chi = new vector<Double_t>(collisionA->at(0).size());

  for (size_t i = 0; i < collisionA->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->at(p).at(i);
      x[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;


    }
    string x1 = "chi Y:" + to_string(B1BCID->at(0).at(i));

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(3);
    plot->SetMarkerColor(1);

    plot->Draw("AP*");



    chi->at(i) = function->GetHistogram()->Chisquare(TotalFitx);

    delete canvasY;

  }


  copy(chi->begin(), chi->end(), z1);
  copy(B1BCID->at(0).begin(), B1BCID->at(0).end(), z2);
  cout<<z1<<endl;
   widthC = new TCanvas("WidthX","namex",200,340,500,300);
   plot = new TGraph(B1BCID->at(0).size(),z2,z1);
  plot->SetTitle("chi Y");
  plot->Draw("AP");

  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.4);
  plot->SetMarkerColor(2);
  plot->Draw("AP");

  writer->Write(widthC);


}























































//emd
