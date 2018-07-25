
struct CollisionData{
  vector<vector<Int_t>>* BCID;
  vector<vector<Float_t>>* LumData;
  vector<vector<Float_t>>* Error;
};
struct BeamDataDesc{
  Float_t scanRun;
  Int_t scanLumBlock;
  Int_t scanStep;
  char plane;
  Float_t planeCoord;
  int beamnumber;
};

struct BeamDataHandlerDesc{
  Int_t scanSteps;
};

class TCanvasFileWriter{
  private:
  string fileName;


  public:

  TCanvasFileWriter(string fileName_){
    fileName = fileName_ + ".pdf";
  }

  void OpenFile(){
    TCanvas * open = new TCanvas("namey","namey",200,340,500,300);
    string temp = fileName +"(";
    TPaveLabel *title = new TPaveLabel(0.1,0.94,0.9,0.98,fileName.c_str());
    title->SetFillColor(16);
    title->SetTextFont(52);
    title->Draw();

    open->Print(temp.c_str());
  }

  void Write(TCanvas * canvas){
    canvas->Print(fileName.c_str());

  }

  void CloseFile(){
    TCanvas * close = new TCanvas("namey","namey",200,340,500,300);
    string temp = fileName + ")";
    close->Print(temp.c_str());
  }


};

template <class T> class Vector2D{
  T x_m,y_m;

  public:
  Vector2D(T x,T y){
    x_m  = x;
    y_m = y;
  }

  T GetX(){
    return x_m;
  }

  T GetY(){
    return y_m;
  }

  void SetPoint(T x, T y){
    x_m = x;
    y_m = y;
  }

  void SetX(T x){
    x_m = x;
  }

  void SetY(T y){
    y_m = y;
  }

  void Offset(T x, T y){
    x_m += x;
    y_m += y;
  }

  void OffsetX(T x){
    x_m +=x;
  }

  void OffsetY(T y){
    y_m += y;
  }

  void Scale(int x){
    x_m *=x;
    y_m *=x;
  }
  void Scale(int x, int y){
    x_m *=x;
    y_m *=y;
  }
  void ScaleX(int x){
    x_m*=x;
  }
  void ScaleY(int y){
    y_m*=y;
  }


};

class Function{
  string name;
  TF1* function;
  bool functionfitted = false;

  public:
  Function(string name){

    this->name = name;
    IntializeFunction();


  };
  virtual void IntializeFunction();

  void Fit(TGraph* plot){
    plot->Fit(function, "MESQ");
    functionfitted = true;

  };

  void Fit(TGraphErrors * plot){
    plot->Fit(function, "MESQ");
    functionfitted = true;
  };

  Float_t GetChi2(TGraph* plot,const char* opt){
    plot->Fit(function, opt);

    return plot->Chisquare(function);
  };
  Float_t GetChi2(TGraphErrors* plot,const char* opt){
    plot->Fit(function, opt);

    return plot->Chisquare(function);
  };

  TF1* GetFunction(){
    try{
      if (functionfitted)return function;
      else throw "Fucntion not Fitted";

    }catch (char* c){
      cout << c << endl;
    }
  };
};


class Plot{

  TGraph* plot;
  Function *function;

  public:
  Plot(TGraph* plot){
    this->plot = plot;
  }
  void FitToFunction(Function * function){
    this->function = function;
    function->Fit(plot);
  }

  void Draw();

};

class Frame{
  Vector2D<Int_t> *size;
  int columns;
  vector<Plot*> *plots;

  public:

  Frame(Vector2D<Int_t> *size,int columns){
    this->size = size;
    this->columns = columns;
  };

  void AddPlot(Plot *plot){
    plots->push_back(plot);
  };

  void Draw(){

    bool finished = false;
    int row = 0;
    int ySize = size->GetY()/(plots->size()/columns);
    int yadjust = size->GetY() - ySize;
    int xadjust = size->GetX() - size->GetX()/columns;

    int temp = 0;
    while(!(temp == plots->size())){

      for (size_t i = 0; i < columns; i++) {
        TPad *pad = new TPad("Graph","",0,0,1,1);
        pad->SetBBoxX2(xadjust);
        pad->SetBBoxY2(yadjust);
        pad->SetBBoxCenterX((size->GetX()/columns + size->GetX()/columns*i)/2);
        pad->SetBBoxCenterY((ySize + row * ySize)/2);
        pad->Draw();
        pad->cd();

        plots->at(i)->Draw();



        temp++;


      }



    }

  }


};

class BeamDataHandler{

  BeamDataHandlerDesc dataHandlerDesc;
  vector<BeamDataDesc> beamDataDesc;
  TCanvasFileWriter * ioHandler;
  CollisionData collision;
  TTree* tree;

  int beamNumber = -1;

  public:

  BeamDataHandler(string fileName, string exportFileName, int beamNumber){

    string fileLocation = "file:~/Downloads/HEPP/" + fileName+".root";
    TCanvasFileWriter *ioHandler = new TCanvasFileWriter(exportFileName);
    TFile *filein = new TFile(fileLocation.c_str(),"READ");
    TTree *tree = (TTree*) filein->Get("vdMScanData;1");

    dataHandlerDesc.scanSteps = tree->GetEntries();
    this->beamNumber = beamNumber;



  }

  void sortBeamData(){


    string number;
    if(beamNumber ==0){number = "1";}else{ number = "2";}

    cout<< "sorting Beam data " << endl;
    Int_t nentries = dataHandlerDesc.scanSteps;
    for (size_t i = 0; i < nentries; i++) {
      beamDataDesc.push_back(BeamDataDesc());

      tree->GetEntry(i);
      beamDataDesc.at(i).scanRun = tree->GetLeaf("ScanRun")->GetValue(0);
      beamDataDesc.at(i).scanLumBlock = tree->GetLeaf("ScanLB")->GetValue(0);
      beamDataDesc.at(i).scanStep = tree->GetLeaf("ScanStep")->GetValue(0);
      if( i >= nentries/2){
        beamDataDesc.at(i).plane = 'x';
        string temp = "B"+number+"DeltaXSet";
        beamDataDesc.at(i).planeCoord = tree->GetLeaf(temp.c_str())->GetValue(0);

      }else{
        beamDataDesc.at(i).plane = 'y';
        string temp = "B"+number+"DeltaYSet";
        beamDataDesc.at(i).planeCoord = tree->GetLeaf(temp.c_str())->GetValue(0);
      }


      if (i%3 == 0) cout << "."<<endl;
    }
    ResetTree();
  };

  void sortCollisionDataLum(){

    string temp;
    if(beamNumber ==0){temp = "lucBiHitOR_BunchInstLumi";}else{temp = "lucBi2HitOR_BunchInstLumi";}

    int y = dataHandlerDesc.scanSteps;
    int z = collision.BCID->at(0).size();
    collision.LumData = new vector<vector<Float_t>> (y,vector<Float_t>(z));


    for (size_t i = 0; i < y; i++) {
      for (size_t p = 0; p < z; p++) {
        collision.LumData->at(i).at(p)= tree->GetLeaf(temp.c_str())->GetValue(collision.BCID->at(i).at(p));
      }
    }


  };

  void sortCollisionDataBCID(){

    cout << "Retrieving B1BCIDs" << endl;
    ResetTree();
    Int_t nBaskets = tree->GetLeaf("LuminousBCIDs")->GetLen();

    collision.BCID = new vector<vector<Int_t>>(dataHandlerDesc.scanSteps, vector<Int_t>(nBaskets));

    for (size_t p = 0; p < dataHandlerDesc.scanSteps; p++) {
      tree->GetEntry(p);
      for (size_t i = 0; i < nBaskets; i++) {
         collision.BCID->at(i).at(p) = i;
      }
    }
    ResetTree();

  };

  void sortCollisionError(){

    collision.Error = new vector<vector<Float_t>> (dataHandlerDesc.scanSteps, vector<Float_t>(collision.BCID->at(0).size()));



    string tempRaw;
    string tempNOrb;

    if(beamNumber ==0){
        tempRaw = "lucBiHitOR_BunchRawInstLum";
        tempNOrb = "lucBiHitOR_NOrbPhys";
      }else{
        tempRaw = "lucBi2HitOR_BunchRawInstLum";
        tempNOrb = "lucBi2HitOR_NOrbPhys";
    }

    auto rate = tree->GetLeaf(tempRaw.c_str());

    for (size_t i = 0; i < dataHandlerDesc.scanSteps; i++) {
      tree->GetEntry(i);
      auto N = tree->GetLeaf(tempNOrb.c_str());
      for (size_t c = 0; c < collision.Error->at(0).size(); c++) {

        int BCID = collision.BCID->at(0).at(c);
        int Nt = 8;
        Float_t f = rate->GetValue(BCID)/Nt;
        Double_t lumerror = 1./(sqrt(N->GetValue(0)* Nt))* (-1)/log(1-f)*sqrt(f/(1-f));
        if (lumerror == numeric_limits<float>::infinity()) lumerror = 1;
        collision.Error->at(i).at(c) = lumerror * collision.LumData->at(i).at(c);
      }
    }
    ResetTree();

  }

  void ResetTree(){
    tree->GetEntry(0);
  };
  void printBeamDataDesc(){

    cout <<"Scan Run Number : ScanLumBlock Number : Scan Step: Plane : PlaneCoord" << endl;
    int count = beamDataDesc.size();
    for (size_t i = 0; i < count; i++) {
      /* code */
      cout <<beamDataDesc.at(i).scanRun <<" " <<beamDataDesc.at(i).scanLumBlock << " "<<beamDataDesc.at(i).scanStep << " "<< beamDataDesc.at(i).plane<< " "<< beamDataDesc.at(i).planeCoord << endl;
    }

  };


};

class BeamCollisionHandler{
  BeamDataHandler * beamA;
  BeamDataHandler * beamB;
  public:
  BeamCollisionHandler(BeamDataHandler * beamA, BeamDataHandler * beamB){
    this->beamA = beamA;
    this->beamB = beamB;

  };


};
