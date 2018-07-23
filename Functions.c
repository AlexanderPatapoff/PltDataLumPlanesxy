
#include <vector>
#include <cmath>
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


struct CollisionData{
  const vector<vector<Int_t>>* BCID;
  vector<vector<Float_t>>* LumData;
  vector<vector<Float_t>>* Error;
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

void SortErrors(CollisionData * collision, TTree* tree, int beam){

  collision->Error = new vector<vector<Float_t>> (18, vector<Float_t>(collision->BCID->at(0).size()));





  auto rate = tree->GetLeaf("lucBiHitOR_BunchRawInstLum");

  for (size_t i = 0; i < 18; i++) {
    tree->GetEntry(i);
    auto N = tree->GetLeaf("lucBiHitOR_NOrbPhys");
    for (size_t c = 0; c < collision->Error->at(0).size(); c++) {

      int BCID = collision->BCID->at(0).at(c);
      int Nt = 8;
      Float_t f = rate->GetValue(BCID)/Nt;
      Double_t lumerror = 1./(sqrt(N->GetValue(0)* Nt))* (-1)/log(1-f)*sqrt(f/(1-f));
      if (lumerror == numeric_limits<float>::infinity()) lumerror = 1;
      collision->Error->at(i).at(c) = lumerror * collision->LumData->at(i).at(c);
    }
  }
  tree->GetEntry(0);

};




Float_t Distance(TF1* function, Float_t X, Float_t Y){

  Float_t temp =  function->Eval(X) - Y;
  return temp;
}



void PlotDifferenceOverLay(TCanvas* canvas,TF1* function, float * x, float * y, int size){
  canvas->cd();

  TPad *overlay = new TPad("overlay","",1,1,0,0);
  overlay->SetFillStyle(4000);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(4000);
  overlay->Draw();
  overlay->cd();

  for (size_t i = 0; i < size; i++) {
    y[i] = Distance(function, x[i],y[i]);
  }
  TGraph* plot = new TGraph(size,x,y);
  plot->SetTitle(" ");
  plot->SetMarkerColor(kRed);
  plot->SetMarkerStyle(18);
  plot->SetName("gr2");

  //overlay->SetBBoxCenterY(250);
  overlay->SetBBoxY1(170);
  plot->SetMarkerSize(0.7);
  plot->GetYaxis()->SetTitle("Seperation");
  plot->Draw("AP");

}


void GraphTotalAvgCollisions(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData* collisionA, CollisionData* collisionB){
  cout <<"graphing total collisions" << endl;
  size_t t = collisionB->LumData->size()/2;

  Float_t x[t], y[t],zy[t],zx[t];

    for (size_t i = 0; i < t; i++) {
      zy[i] = 0;
      y[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionB->LumData->at(i).size(); p++) {
        zy[i] +=  collisionB->LumData->at(i).at(p);

      }

      for (size_t s  = 0; s < collisionB->LumData->at(i).size(); s++) {

        zy[i] += collisionB->LumData->at(i).at(s);

      }
      zy[i] = zy[i]/(2*collisionB->LumData->at(i).size());
    }



    for (size_t i = t; i < 2*t; i++) {
      zx[i-t]=0;
      x[i-t] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionB->LumData->at(i).size(); p++) {
        zx[i-t] += collisionB->LumData->at(i).at(p);
      }



      for (size_t p = 0; p < collisionB->LumData->at(i).size(); p++) {
        zx[i-t] += collisionB->LumData->at(i).at(p);
      }
      zx[i-t] = zx[i-t]/(2*collisionB->LumData->at(i).size());
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



TF1* GetXFit(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData* collisionA, CollisionData* collisionB){
    size_t t = collisionB->LumData->size()/2;
    Float_t x[t], y[t],zy[t],zx[t];
    for (size_t i = t; i < 2*t; i++) {
      zx[i-t]=0;
      x[i-t] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionB->LumData->at(i).size(); p++) {
        zx[i-t] += collisionB->LumData->at(i).at(p);
      }



      for (size_t p = 0; p < collisionB->LumData->at(i).size(); p++) {
        zx[i-t] += collisionB->LumData->at(i).at(p);
      }
      zx[i-t] = zx[i-t]/(2*collisionB->LumData->at(i).size());
    }


    TGraph* gx = new TGraph(9,x,zx);
    gx->SetTitle("X-Collisions");
    TF1 *f1 = new TF1("f1","gaus",-0.1,0.1);
    gx->Fit("f1","R");
    return gx->GetFunction("f1");

};

TF1* GetYFit(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData* collisionA, CollisionData* collisionB){
    size_t t = collisionB->LumData->size()/2;
    Float_t x[t], y[t],zy[t],zx[t];


    for (size_t i = 0; i < t; i++) {
      zy[i] = 0;
      y[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;


      for (size_t p = 0; p < collisionB->LumData->at(i).size(); p++) {
        zy[i] +=  collisionB->LumData->at(i).at(p);

      }

      for (size_t s  = 0; s < collisionB->LumData->at(i).size(); s++) {

        zy[i] += collisionB->LumData->at(i).at(s);
      }
      zy[i] = zy[i]/(2*collisionB->LumData->at(i).size());
    }


    TGraph* gy = new TGraph(9,y,zy);
    gy->SetTitle("Y-Collisions");
    TF1 *f2 = new TF1("f2","gaus",-0.1,0.1);
    gy->Fit("f2","R");
    return gy->GetFunction("f2");

};

void PlotBCIDs_Fit(vector<BeamData> *beamA,vector<BeamData> *beamB, CollisionData* collisionA, CollisionData* collisionB, TF1* fitX,TF1* fitY, ExportDataHandler * saveData){
  TCanvas *canvasX;
  TCanvas *canvasY;
  canvasX = new TCanvas("initx","welcome",200,340,500,300);
  canvasX->Print("testX.pdf(");
  canvasY = new TCanvas("inity","welcome",200,340,500,300);
  canvasY->Print("testY.pdf(");
  size_t t = collisionA->LumData->size()/2;
  Float_t x[t], y[t],zy[t],zx[t];
  for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
    for (size_t p = 0; p < t; p++) {
      y[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;
      x[p] = beamA->at(t+p).planeCoord-beamB->at(t+p).planeCoord;

      zx[p]=0;
      zy[p] = 0;


      zx[p] += collisionA->LumData->at((t+p)).at(i);
      zx[p] += collisionB->LumData->at(t+p).at(i);
      zx[p] = zx[p]/2;

      zy[p] +=  collisionA->LumData->at(p).at(i);
      zy[p] += collisionB->LumData->at(p).at(i);
      zy[p] = zy[p]/2;





    }
    string x1 = "X:" + to_string(collisionA->BCID->at(0).at(i));
    string y1 = "Y:" + to_string(collisionA->BCID->at(0).at(i));
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

void PlotMeansXY(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData* collisionA, CollisionData* collisionB,TCanvasFileWriter *writer){

  vector<Float_t> * mean = new vector<Float_t>(collisionA->LumData->at(0).size());
  Float_t x[9], y[9];
  cout << collisionA->LumData->size()<<endl;
  for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->LumData->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->LumData->at(9+p).at(i);
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



  float z1[collisionA->BCID->at(0).size()];
  float z2[collisionA->BCID->at(0).size()];

    copy(mean->begin(), mean->end(), z1);
    copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z2);
    TCanvas * canvasY = new TCanvas("namey","namey",200,340,500,300);
    TGraph* plot = new TGraph(collisionA->BCID->at(0).size(),z2,z1);

    plot->SetTitle("MeanX by BCID");

  //  plot->Draw("AP");
    plot->SetMarkerStyle(20);
    plot->SetMarkerSize(0.5);
    plot->SetMarkerColor(2);
    plot->Draw("AP");
    writer->Write(canvasY);


    mean = new vector<Float_t>(collisionA->LumData->at(0).size());
    for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
      for (size_t p = 0; p < collisionA->LumData->size()/2; p++) {
        x[p] = 0;
        y[p] = 0;
        y[p] = collisionA->LumData->at(p).at(i);
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
    copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z2);
    canvasY = new TCanvas("namey","namey",200,340,500,300);
    plot = new TGraph(collisionA->BCID->at(0).size(),z2,z1);

    plot->SetTitle("MeanY by BCID");




    //canvasY->cd(4);
    plot->Draw("AP");
    plot->SetMarkerStyle(20);
    plot->SetMarkerSize(0.4);
    plot->SetMarkerColor(2);
    plot->Draw("AP");
    writer->Write(canvasY);



}

void PlotFitsXY(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData* collisionA, CollisionData* collisionB,TCanvasFileWriter *writer){

  int size = collisionA->LumData->size()/2;
  Float_t x[size], y[size],e[size];
  for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->LumData->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->LumData->at(size+p).at(i);
      x[p] = beamA->at(9+p).planeCoord-beamB->at(size+p).planeCoord;
      float xe = collisionA->LumData->at(p).at(i);
      float ye = collisionB->LumData->at(p).at(i);
      e[i] = sqrt(pow(collisionB->Error->at(p).at(i),2) + pow(collisionA->Error->at(p).at(i),2));

    }
    string x1 = "X:" + to_string(collisionA->BCID->at(0).at(i));

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraphErrors* plot = new TGraphErrors(9,x,y,0,e);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);
    plot->Fit("h1","L");

    plot->SetMarkerStyle(18);
    plot->SetMarkerSize(0.3);
    plot->SetMarkerColor(1);

    plot->Draw("AP");

    function->Draw("SAME");
    writer->Write(canvasY);



    delete canvasY;

  }



  for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->LumData->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->LumData->at(p).at(i);
      x[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;


    }

    string x1 = "Y:" + to_string(collisionA->BCID->at(0).at(i));

    const char* namex = x1.c_str();



    TCanvas * canvasX = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(9,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    plot->Draw("AP");
    plot->Fit("h1","L");

    plot->SetMarkerStyle(18);
    plot->SetMarkerSize(0.3);
    plot->SetMarkerColor(1);

    plot->Draw("AP");

    function->Draw("SAME");
    writer->Write(canvasX);
    delete canvasX;

  }
}


void PlotWidthXY(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData* collisionA, CollisionData* collisionB,TCanvasFileWriter *writer){

  vector<Double_t> * width = new vector<Double_t>(collisionA->LumData->at(0).size());
  Float_t x[9], y[9];
  for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->LumData->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->LumData->at(9+p).at(i);
      x[p] = beamA->at(9+p).planeCoord-beamB->at(9+p).planeCoord;


    }
    string x1 = "X:" + to_string(collisionA->BCID->at(0).at(i));

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
  float z1[collisionA->BCID->at(0).size()];
  float z2[collisionA->BCID->at(0).size()];

  copy(width->begin(), width->end(), z1);
  copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z2);
  cout<<z1<<endl;
  TCanvas * widthC = new TCanvas("WidthX","namex",200,340,500,300);
  TGraph* plot = new TGraph(collisionA->BCID->at(0).size(),z2,z1);
  plot->SetTitle("Width X");
  plot->Draw("AP");

  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.4);
  plot->SetMarkerColor(2);
  plot->Draw("AP");

  writer->Write(widthC);







  for (size_t i = 0; i < collisionA->LumData->at(0).size(); i++) {
    for (size_t p = 0; p < collisionA->LumData->size()/2; p++) {
      x[p] = 0;
      y[p] = 0;
      y[p] = collisionA->LumData->at(p).at(i);
      x[p] = beamA->at(p).planeCoord-beamB->at(p).planeCoord;


    }
    string x1 = "Y:" + to_string(collisionA->BCID->at(0).at(i));

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
  copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z2);

  widthC = new TCanvas("WidthX","namex",200,340,500,300);
  plot = new TGraph(collisionA->BCID->at(0).size(),z2,z1);
  plot->SetTitle("Width Y");
  plot->Draw("AP");

  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.4);
  plot->SetMarkerColor(2);
  plot->Draw("AP");

  writer->Write(widthC);


}




vector<vector<Float_t>>* getCollisionError(){

  return NULL;
}

void ComparePeaks(CollisionData* collisionA1,CollisionData* collisionB1,CollisionData* collisionA2,CollisionData* collisionB2,TCanvasFileWriter * writer){
  int size = collisionA1->LumData->at(0).size();
  double x[size];
  double y[size];
  for (size_t i = 0; i < collisionA1->LumData->at(0).size(); i++) {
    x[i] = 0;
    y[i] = 0;
    y[i] = (collisionA1->LumData->at(3).at(i) + collisionB1->LumData->at(3).at(i))/2;
    x[i] = collisionA1->BCID->at(0).at(i);


    }
    string x1 = "Y = ComparisonPeak";

    const char* namex = x1.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(size,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    //plot->Draw("AP");
    //plot->Fit("h1","L");

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(0.6);
    plot->SetMarkerColor(1);

    plot->Draw("AP");
    writer->Write(canvasY);

    for (size_t i = 0; i < collisionA2->LumData->at(0).size(); i++) {
      x[i] = 0;
      y[i] = 0;
      y[i] = (collisionA2->LumData->at(3).at(i) + collisionB2->LumData->at(3).at(i))/2;
      x[i] = collisionA2->BCID->at(0).at(i);


    }

    plot = new TGraph(size,x,y);



    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(0.6);
    plot->SetMarkerColor(2);

    plot->Draw("SAMEP");

    writer->Write(canvasY);



};


void ComparePoints(int x1, int x2,CollisionData* collisionA1,CollisionData* collisionB1,TCanvasFileWriter * writer){
  int size = collisionA1->LumData->at(0).size();
  double x[size];
  double y[size];
  for (size_t i = 0; i < collisionA1->LumData->at(0).size(); i++) {
    x[i] = 0;
    y[i] = 0;
    y[i] = ((collisionA1->LumData->at(x1).at(i) + collisionB1->LumData->at(x1).at(i))/2)
          /((collisionA1->LumData->at(x2).at(i) + collisionB1->LumData->at(x2).at(i))/2);
    x[i] = collisionA1->BCID->at(0).at(i);


    }
    string xx = "X: ComparePoints" + to_string(x1) + " " + to_string(x2);

    const char* namex = xx.c_str();

    TCanvas * canvasY = new TCanvas(namex,namex,200,340,500,300);
    TGraph* plot = new TGraph(size,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);
    plot->SetTitle(namex);

    //plot->Draw("AP");
    //plot->Fit("h1","L");

    plot->SetMarkerStyle(18);
    plot->SetMarkerSize(0.3);
    plot->SetMarkerColor(2);

    plot->Draw("AP");

    writer->Write(canvasY);

}
void CompareAllPoints(CollisionData* collisionA1,CollisionData* collisionB1,TCanvasFileWriter * writer){
  int size = collisionA1->LumData->at(0).size();
  int point = 4;
  double rate[9];
  double x[size];
  double y[size];
  TCanvas * canvasY = new TCanvas("ets","test",200,340,500,300);
  TGraph* plot;
  for (size_t l = 0; l < 9; l++) {
    /* code */
    for (size_t i = 0; i < collisionA1->LumData->at(0).size(); i++) {
      x[i] = 0;
      y[i] = 0;
      y[i] = ((collisionA1->LumData->at(l).at(i) + collisionB1->LumData->at(l).at(i))/2)
      /((collisionA1->LumData->at(point).at(i) + collisionB1->LumData->at(point).at(i))/2);
      x[i] = collisionA1->BCID->at(0).at(i);



    }


    string xx = "X: ComparePoints" + to_string(4) + " " + to_string(l);

    const char* namex = xx.c_str();

     plot = new TGraph(size,x,y);
     TF1* function = new TF1("h1","pol1",-1,1);
    plot->SetTitle(namex);



    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(0.6);
    plot->SetMarkerColor(l+1);
    plot->GetYaxis()->SetRangeUser(0,1);


    if(l>0)plot->Draw("SAMEP");
    else plot->Draw("AP");
    plot->Fit("h1");

    rate[l] = function->GetParameter(1);

  }

  canvasY = new TCanvas("d","dd",200,340,500,300);

  string xx = "X: ComparePoints" + to_string(4) + " ALL" ;

  const char* namex = xx.c_str();
  double z[9] = {0,1,2,3,4,5,6,7,8};
   plot = new TGraph(9,z,rate);

  plot->SetTitle(namex);

  //plot->Draw("AP");


  plot->SetMarkerStyle(21);
  plot->SetMarkerSize(0.6);
  plot->SetMarkerColor(1);
  //plot->GetYaxis()->SetRangeUser(0,1);

  plot->Draw("AP");

  writer->Write(canvasY);



}

template <class T> int FindIndex(vector<T> data, T objective){
  for (size_t i = 0; i < data.size(); i++) {
    if(data.at(i) == objective) return i;
  }
  return -1;
}
float Chi2(float observed, float expected){
  return (pow((observed - expected),2)/expected);
};
float calculateChi2(TF1* func, float* x,float* y ){
  float chi = 0;
  for (size_t i = 0; i < 9; i++) {
    chi += Chi2(y[i],func->Eval(x[i]));
  }

  cout <<chi<< endl;
  return chi;

};


void PlotChi2_XY(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData *collisionA,CollisionData *collisionB,TCanvasFileWriter * writer){
  int size = 9;

  vector<Float_t> *chai2 = new vector<Float_t>();
  for (size_t c = 0; c < collisionA->BCID->at(0).size(); c++) {
    /* code */
    float x[size];
    float y[size];
    float e[size];
    for (size_t i = 0; i < size; i++) {
      x[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;
      y[i] = (collisionA->LumData->at(i).at(c) + collisionB->LumData->at(i).at(c))/2;
      float xe = collisionA->LumData->at(i).at(c);
      float ye = collisionB->LumData->at(i).at(c);
      e[i] = sqrt(pow(collisionB->Error->at(i).at(c),2) + pow(collisionA->Error->at(i).at(c),2));
    }



    TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
    TGraphErrors* plot = new TGraphErrors(size,x,y,0,e);
    TF1* function = new TF1("h1","gaus",-1,1);

    plot->Fit("h1");
    plot->Draw("AP");


    chai2->push_back(plot->Chisquare(function));
    cout << chai2->at(c) << ",";
    delete canvas;
  }

  cout <<endl;
  float z1[collisionA->BCID->at(0).size()];
  float z2[collisionA->BCID->at(0).size()];
  float z2e[collisionA->BCID->at(0).size()];

  copy(chai2->begin(), chai2->end(), z2e);
  copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z2);

  TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
  TGraph* plot = new TGraph(collisionA->BCID->at(0).size(),z2,z2e);

  plot->SetTitle("Chi2 Y");
  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.3);
  plot->SetMarkerColor(2);

  plot->Draw("AP");

  writer->Write(canvas);


  chai2 = new vector<Float_t>();
  for (size_t c = 0; c < collisionA->BCID->at(0).size(); c++) {
    /* code */
    float x[size];
    float y[size];
    float e[size];
    float zero[size];
    for (size_t i = 9; i < size*2; i++) {
      x[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;
      y[i] = (collisionA->LumData->at(i).at(c) + collisionB->LumData->at(i).at(c))/2;
      float xe = collisionA->LumData->at(i).at(c);
      float ye = collisionB->LumData->at(i).at(c);
      e[i] = sqrt(pow((xe/2),2)*pow(collisionB->Error->at(i).at(c),2) + pow((ye/2),2)*pow(collisionA->Error->at(i).at(c),2));
    }



    TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
    TGraphErrors* plot = new TGraphErrors(size,x,y,0,e);
    TF1* function = new TF1("h1","gaus",-1,1);

    plot->Fit("h1");
    plot->Draw("AP");


    chai2->push_back(plot->Chisquare(function));
    cout << chai2->at(c) << ",";
    delete canvas;
  }

  cout <<endl;

  float z22[collisionA->BCID->at(0).size()];
  float z2e2[collisionA->BCID->at(0).size()];

  copy(chai2->begin(), chai2->end(), z2e2);
  copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z22);

  canvas = new TCanvas("test","ttt",200,340,500,300);
  plot = new TGraph(collisionA->BCID->at(0).size(),z2,z2e);

  plot->SetTitle("Chi2 X");
  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.3);
  plot->SetMarkerColor(2);

  plot->Draw("AP");

  writer->Write(canvas);
}

void PlotChi2_XY_DoubleGuas(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData *collisionA,CollisionData *collisionB,TCanvasFileWriter * writer){

  int size = 9;
  vector<Float_t> *chi2 = new vector<Float_t>();

  int count = collisionA->BCID->at(0).size();
  for (size_t index = 0; index < count; index++) {
      /* code */

    float x[size];
    float y[size];
    float e[size];

    for (size_t i = 0; i < size; i++) {
      x[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;
      y[i] = (collisionA->LumData->at(i).at(index) + collisionB->LumData->at(i).at(index))/2;
      float xe = collisionA->LumData->at(i).at(index);
      float ye = collisionB->LumData->at(i).at(index);
      e[i] = sqrt(pow((xe/2),2)*pow(collisionB->Error->at(i).at(index),2) + pow((ye/2),2)*pow(collisionA->Error->at(i).at(index),2));
    }



    //TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
    TGraphErrors* plot = new TGraphErrors(size,x,y,0,e);
    plot->SetTitle("DoubleGuass_comparison");

    //0 - height
    //1 - Mean
    //2 - capsigma (Generalize width)
    //3 - fraction of 2nd Gaussian
    //4 - multiplier of second width
    TF1* doubleGuass = new TF1("doubleGuass","[0]*( (1-[3])*exp(-0.5*((x-[1])/([2]/(1-[3]+[3]*[4])))**2) + [3]*exp(-0.5*((x-[1])/([2]*[4]/(1-[3]+[3]*[4])))**2))",-1,1);
    TF1 * fit = new TF1("h1","gaus",-1,1);

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(0.6);
    plot->SetMarkerColor(1);
    //plot->Draw("AP");

    plot->Fit("h1");

    float p0,p1,p2,p3,p4,cdof2;
    p0 = fit->GetParameter(0);
    p1 = fit->GetParameter(1);
    p2 = fit->GetParameter(2);


    doubleGuass->SetParameter(0,p0);
    doubleGuass->SetParameter(1,p1);
    doubleGuass->SetParameter(2,p2);
    doubleGuass->SetParameter(3,0.5);
    doubleGuass->SetParameter(4,5.0);
    doubleGuass->SetParLimits(4,1,1000);


    plot->Fit("doubleGuass","MESQ");

    p0 = doubleGuass->GetParameter(0);
    p1 = doubleGuass->GetParameter(1);
    p2 = doubleGuass->GetParameter(2);
    p3 = doubleGuass->GetParameter(3);
    p4 = doubleGuass->GetParameter(4);
    //cdof2 = (doubleGuass->chi2()/max(f))

    chi2->push_back(plot->Chisquare(doubleGuass));
    //delete canvas;

  }

  float z2[collisionA->BCID->at(0).size()];
  float z2e[collisionA->BCID->at(0).size()];

  copy(chi2->begin(), chi2->end(), z2e);
  copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z2);

  TCanvas * canvasF = new TCanvas("test","ttt",200,340,500,300);

  TGraph *plotF = new TGraph(collisionA->BCID->at(0).size(),z2,z2e);

  plotF->SetTitle("Chi2 DoubleGuass Y");
  plotF->SetMarkerStyle(18);
  plotF->SetMarkerSize(0.3);
  plotF->SetMarkerColor(2);

  plotF->Draw("AP");

  writer->Write(canvasF);



  delete canvasF;





  chi2 = new vector<Float_t>();

  count = collisionA->BCID->at(0).size();
  for (size_t index = 0; index < count; index++) {
      /* code */

    float x[size];
    float y[size];
    float e[size];

    for (size_t i = size; i < size*2; i++) {
      x[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;
      y[i] = (collisionA->LumData->at(i).at(index) + collisionB->LumData->at(i).at(index))/2;
      float xe = collisionA->LumData->at(i).at(index);
      float ye = collisionB->LumData->at(i).at(index);
      e[i] = sqrt(pow((xe/2),2)*pow(collisionB->Error->at(i).at(index),2) + pow((ye/2),2)*pow(collisionA->Error->at(i).at(index),2));
    }



    //TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
    TGraphErrors* plot = new TGraphErrors(size,x,y,0,e);
    plot->SetTitle("DoubleGuass_comparison");

    //0 - height
    //1 - Mean
    //2 - capsigma (Generalize width)
    //3 - fraction of 2nd Gaussian
    //4 - multiplier of second width
    TF1* doubleGuass = new TF1("doubleGuass","[0]*( (1-[3])*exp(-0.5*((x-[1])/([2]/(1-[3]+[3]*[4])))**2) + [3]*exp(-0.5*((x-[1])/([2]*[4]/(1-[3]+[3]*[4])))**2))",-1,1);
    TF1 * fit = new TF1("h1","gaus",-1,1);

    plot->SetMarkerStyle(21);
    plot->SetMarkerSize(0.6);
    plot->SetMarkerColor(1);
    //plot->Draw("AP");

    plot->Fit("h1");

    float p0,p1,p2,p3,p4,cdof2;
    p0 = fit->GetParameter(0);
    p1 = fit->GetParameter(1);
    p2 = fit->GetParameter(2);


    doubleGuass->SetParameter(0,p0);
    doubleGuass->SetParameter(1,p1);
    doubleGuass->SetParameter(2,p2);
    doubleGuass->SetParameter(3,0.5);
    doubleGuass->SetParameter(4,5.0);
    doubleGuass->SetParLimits(4,1,1000);


    plot->Fit("doubleGuass","MESQ");

    p0 = doubleGuass->GetParameter(0);
    p1 = doubleGuass->GetParameter(1);
    p2 = doubleGuass->GetParameter(2);
    p3 = doubleGuass->GetParameter(3);
    p4 = doubleGuass->GetParameter(4);
    //cdof2 = (doubleGuass->chi2()/max(f))

    chi2->push_back(plot->Chisquare(doubleGuass));
    //delete canvas;

  }

  float z22[collisionA->BCID->at(0).size()];
  float z2e2[collisionA->BCID->at(0).size()];

  copy(chi2->begin(), chi2->end(), z2e2);
  copy(collisionA->BCID->at(0).begin(), collisionA->BCID->at(0).end(), z22);

  TCanvas * can = new TCanvas("test","ttt",200,340,500,300);

  TGraph *plotFs = new TGraph(collisionA->BCID->at(0).size(),z2,z2e);

  plotFs->SetTitle("Chi2 DoubleGuass X");
  plotFs->SetMarkerStyle(18);
  plotFs->SetMarkerSize(0.3);
  plotFs->SetMarkerColor(2);

  plotFs->Draw("AP");

  writer->Write(can);



  delete can;





}


void DoubleGuass_Y(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData *collisionA,CollisionData *collisionB, int BCID,TCanvasFileWriter * writer){

  int size = 9;


  float x[size];
  float y[size];
  float e[size];
  int index = FindIndex<Int_t>(collisionA->BCID->at(0), BCID);

  for (size_t i = 0; i < size; i++) {
    x[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;
    y[i] = (collisionA->LumData->at(i).at(index) + collisionB->LumData->at(i).at(index))/2;
    float xe = collisionA->LumData->at(i).at(index);
    float ye = collisionB->LumData->at(i).at(index);
    e[i] = sqrt(pow((xe/2),2)*pow(collisionB->Error->at(i).at(index),2) + pow((ye/2),2)*pow(collisionA->Error->at(i).at(index),2));
  }



  TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
  TGraphErrors* plot = new TGraphErrors(size,x,y,0,e);
  plot->SetTitle("DoubleGuass_comparison");

  //0 - height
  //1 - Mean
  //2 - capsigma (Generalize width)
  //3 - fraction of 2nd Gaussian
  //4 - multiplier of second width
  TF1* doubleGuass = new TF1("doubleGuass","[0]*( (1-[3])*exp(-0.5*((x-[1])/([2]/(1-[3]+[3]*[4])))**2) + [3]*exp(-0.5*((x-[1])/([2]*[4]/(1-[3]+[3]*[4])))**2))",-1,1);
  TF1 * fit = new TF1("h1","gaus",-1,1);

  plot->SetMarkerStyle(21);
  plot->SetMarkerSize(0.6);
  plot->SetMarkerColor(1);
  plot->Draw("AP");

  plot->Fit("h1","MESQ");

  float p0,p1,p2,p3,p4,cdof2;
  p0 = fit->GetParameter(0);
  p1 = fit->GetParameter(1);
  p2 = fit->GetParameter(2);


  doubleGuass->SetParameter(0,p0);
  doubleGuass->SetParameter(1,p1);
  doubleGuass->SetParameter(2,p2);
  doubleGuass->SetParameter(3,0.5);
  doubleGuass->SetParameter(4,5.0);
  doubleGuass->SetParLimits(4,1,1000);


  plot->Fit("doubleGuass","MESQ");

  p0 = doubleGuass->GetParameter(0);
  p1 = doubleGuass->GetParameter(1);
  p2 = doubleGuass->GetParameter(2);
  p3 = doubleGuass->GetParameter(3);
  p4 = doubleGuass->GetParameter(4);
  //cdof2 = (doubleGuass->chi2()/max(f))



  writer->Write(canvas);

  delete canvas;

}

void Densities(CollisionData *collisionA,CollisionData *collisionB, int BCID_init, int BCID_fin,int point,TCanvasFileWriter* writer){
  vector<Float_t> *density = new vector<Float_t>();
  vector<Int_t> * BCID = new vector<Int_t>();
  int lastIndex = -1;
  TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
  int size = BCID_fin - BCID_init;


  for (size_t i = 0; i < size; i++) {


    if(FindIndex<Int_t>(collisionA->BCID->at(0), i+BCID_init) != -1){
      lastIndex++;
      BCID->push_back(i + BCID_init);
      int index = FindIndex<Int_t>(collisionA->BCID->at(0), i+BCID_init);

      density->push_back(collisionA->LumData->at(point).at(index)/collisionA->LumData->at(4).at(index));
      //cout <<density->at(lastIndex)<<endl;
    }

  }

  float z1[density->size()];
  float z2[BCID->size()];
  cout <<density->size() << endl;
  copy(density->begin(), density->end()-1, z1);
  copy(BCID->begin(), BCID->end()-1, z2);
  for (size_t i = 0; i < density->size(); i++) {
    cout << z2[i] << " : " << z1[i] << endl;
  }

  TGraph* plot = new TGraph(density->size()-1,z2,z1);
  plot->SetTitle("DensityComparison");


  plot->SetMarkerStyle(18);
  plot->SetMarkerSize(0.6);
  plot->SetMarkerColor(2);

  plot->Draw("AP");


  writer->Write(canvas);

}


void PlotG(vector<BeamData> *beamA,vector<BeamData> *beamB,CollisionData *collisionA,CollisionData *collisionB,TCanvasFileWriter * writer){
  int size = 9;
  int c = 94;

    /* code */
    float x[size];
    float y[size];
    float e[size];
    for (size_t i = 0; i < size; i++) {
      x[i] = beamA->at(i).planeCoord-beamB->at(i).planeCoord;
      y[i] = (collisionA->LumData->at(i).at(c) + collisionB->LumData->at(i).at(c))/2;
      float xe = collisionA->LumData->at(i).at(c);
      float ye = collisionB->LumData->at(i).at(c);
      //e[i] = sqrt(pow((xe/2),2)*pow(collisionB->Error->at(i).at(c),2) + pow((ye/2),2)*pow(collisionA->Error->at(i).at(c),2));
      e[i] = sqrt(pow(collisionB->Error->at(i).at(c),2) + pow(collisionA->Error->at(i).at(c),2));
      cout << e[i] << " " << collisionB->Error->at(i).at(c) << endl;

    }



    TCanvas * canvas = new TCanvas("test","ttt",200,340,500,300);
    TPad *pad = new TPad("Graph","",0,0,1,1);
    pad->SetBBoxY1(100);
    pad->SetBBoxCenterY(100);
    pad->Draw();
    pad->cd();

    TGraphErrors* plot = new TGraphErrors(size,x,y,0,e);
    //TGraph *plot = new TGraph(size,x,y);
    TF1* function = new TF1("h1","gaus",-1,1);

    plot->SetTitle("Chi2 Y");
    plot->SetMarkerStyle(18);
    plot->SetMarkerSize(0.3);
    plot->SetMarkerColor(2);

    plot->Fit("h1");
    plot->Draw("AP");
    PlotDifferenceOverLay(canvas,function,x,y,9);
    writer->Write(canvas);


};

















































//end
