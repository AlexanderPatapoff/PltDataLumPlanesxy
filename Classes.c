#include <cmath>
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
  protected:
  string name;
  Float_t range;
  TF1* function;
  bool functionfitted = false;

  public:
  Function(string name, Float_t range){

    this->name = name;
    this->range = range;
    InitializeFunction();


  };
  virtual void InitializeFunction(){};

  void virtual Fit(TGraph* plot){};

  void virtual Fit(TGraphErrors * plot){};

  Float_t GetChi2(TGraph* plot,const char* opt){
    plot->Fit(function, opt);

    return plot->Chisquare(function);
  };
  Float_t GetChi2(TGraphErrors* plot,const char* opt){
    plot->Fit(function, opt);

    return plot->Chisquare(function);
  };
  Float_t GetWidth(){
    return function->GetHistogram()->GetStdDev();
  }
  Float_t GetMean(){

    return function->GetHistogram()->GetMean(1);
  }
  Float_t Integrate(Float_t x1, Float_t x2){
    return -1;
  }
  TF1* GetFunction(){
    return function;
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

  Int_t GetNSteps(){
    return dataHandlerDesc.scanSteps;
  }

  Int_t GetNCollisions(){
    Int_t temp = collision.BCID->size();
    return 10;
  }

  Float_t GetLumDataAt(int step,int index){
    return collision.LumData->at(step).at(index);
  }

  Int_t GetBCIDAt(int step, int index){
    return collision.BCID->at(step).at(index);
  }

  Float_t GetErrorAt(int step, int index){
    return collision.Error->at(step).at(index);
  }
  private:
  void SortBeamData(){


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

  void SortCollisionDataLum(){

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

  void SortCollisionDataBCID(){

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

  void SortCollisionError(){

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
  CollisionData collision;


  BeamCollisionHandler(BeamDataHandler * beamA, BeamDataHandler * beamB){
    this->beamA = beamA;
    this->beamB = beamB;
    CollideBeams();

  };

  void ReCalculateErrors(Float_t(*ErrorFunction)(Float_t,Float_t)){

    Int_t steps = beamA->GetNSteps();
    Int_t nCollisions = beamA->GetNCollisions();


    collision.Error = new vector<vector<Float_t>>(steps,vector<Float_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {

        Float_t tempError = ErrorFunction(beamA->GetLumDataAt(i,p),beamB->GetLumDataAt(i,p));
        collision.Error->at(i).at(p) = tempError;

      }
    }

    (*ErrorFunction);


  }

  void ReCalculateLums(Float_t(*LumFunction)(Float_t,Float_t)){

    Int_t steps = beamA->GetNSteps();
    Int_t nCollisions = beamA->GetNCollisions();


    collision.LumData = new vector<vector<Float_t>>(steps,vector<Float_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {

        Float_t tempLumData = LumFunction(beamA->GetLumDataAt(i,p),beamB->GetLumDataAt(i,p));
        collision.Error->at(i).at(p) = tempLumData;

      }
    }

    (*LumFunction);

  }

  CollisionData * GetCollision(){
    return &collision;
  }

  CollisionData GetCollisionX(){
    CollisionData x;
    int steps = collision.BCID->size()/2;
    int nCollisions = collision.BCID->at(0).size();

    x.LumData = new vector<vector<Float_t>>(steps, vector<Float_t>(nCollisions));
    x.BCID = new vector<vector<Int_t>>(steps, vector<Int_t>(nCollisions));
    x.Error = new vector<vector<Float_t>>(steps,vector<Float_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {
        x.LumData->at(i).at(p) = collision.LumData->at(steps + i).at(p);
        x.BCID->at(i).at(p) = collision.BCID->at(steps + i).at(p);
        x.Error->at(i).at(p) = collision.Error->at(steps + i).at(p);
      }
    }

    return x;
  }

  CollisionData GetCollisionY(){
    CollisionData x;
    int steps = collision.BCID->size()/2;
    int nCollisions = collision.BCID->at(0).size();

    x.LumData = new vector<vector<Float_t>>(steps, vector<Float_t>(nCollisions));
    x.BCID = new vector<vector<Int_t>>(steps, vector<Int_t>(nCollisions));
    x.Error = new vector<vector<Float_t>>(steps,vector<Float_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {
        x.LumData->at(i).at(p) = collision.LumData->at(i).at(p);
        x.BCID->at(i).at(p) = collision.BCID->at(i).at(p);
        x.Error->at(i).at(p) = collision.Error->at(i).at(p);
      }
    }

    return x;
  }

  private:

  void CollideBeams(){
    cout << "Retrieving steps"<<endl;
    Int_t steps = beamA->GetNSteps();
    cout <<"Retrieving nCollisions"<<endl;
    Int_t nCollisions = beamA->GetNCollisions();

    cout <<"Initializing vectors"<<endl;
    collision.LumData = new vector<vector<Float_t>>(steps, vector<Float_t>(nCollisions));
    collision.BCID = new vector<vector<Int_t>>(steps, vector<Int_t>(nCollisions));
    collision.Error = new vector<vector<Float_t>>(steps,vector<Float_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {

        Float_t tempLumData = (beamA->GetLumDataAt(i,p) + beamB->GetLumDataAt(i,p))/2;
        //collision.LumData->at(i).at(p) = tempLumData;

        collision.BCID->at(i).at(p) = beamA->GetBCIDAt(i,p);

        Float_t tempErrorData = sqrt(pow(beamB->GetErrorAt(i,p),2) + pow(beamA->GetErrorAt(i,p),2));
        //collision.Error->at(i).at(p) = tempErrorData;

      }
    }
  }

};

class SingleGaussFunction:Function{
  SingleGaussFunction(string name,Float_t range) : Function(name,range){

  }

  void Fit(TGraphErrors* plot){
    plot->Fit(function, "MESQ");
  }

  void InitializeFunction(){
    function = new TF1(this->name.c_str(), "gaus", (-1*range), range);
  }
};

class DoubleGaussFunction:Function{
  Float_t widthA,widthB;

  TF1* gaussA, *gaussB;

  DoubleGaussFunction(string name,Float_t range) :Function(name,range){

  }

  void Fit(TGraphErrors* plot){
    TF1 * fit = new TF1("h1","gaus",-1,1);

    plot->Fit("h1","MESQ");

    float p0,p1,p2,p3,p4,cdof2;
    p0 = fit->GetParameter(0);
    p1 = fit->GetParameter(1);
    p2 = fit->GetParameter(2);


    function->SetParameter(0,p0);
    function->SetParameter(1,p1);
    function->SetParameter(2,p2);
    function->SetParameter(3,0.5);
    function->SetParameter(4,5.0);
    function->SetParLimits(4,1,1000);


    plot->Fit("doubleGuass","MESQ");


    p0 = function->GetParameter(0);
    p1 = function->GetParameter(1);
    p2 = function->GetParameter(2);
    p3 = function->GetParameter(3);
    p4 = function->GetParameter(4);
    //cdof2 = (doubleGuass->chi2()/max(f))
    //
    widthA = (p2/(1-p3+p3*p4));
    widthB = (p2*p4/(1-p3+p3*p4));



    gaussA = new TF1("g1","[0]*( (1-[3])*exp(-0.5*((x-[1])/([2]/(1-[3]+[3]*[4])))**2))",-1,1);
    gaussA->SetParameter(0,p0);
    gaussA->SetParameter(1,p1);
    gaussA->SetParameter(2,p2);
    gaussA->SetParameter(3,p3);
    gaussA->SetParameter(4,p4);

    gaussB = new TF1("g2","[0]*([3]*exp(-0.5*((x-[1])/([2]*[4]/(1-[3]+[3]*[4])))**2))",-1,1);
    gaussB->SetParameter(0,p0);
    gaussB->SetParameter(1,p1);
    gaussB->SetParameter(2,p2);
    gaussB->SetParameter(3,p3);
    gaussB->SetParameter(4,p4);


  }

  void InitializeFunction(){
    function = new TF1(this->name.c_str(), "[0]*( (1-[3])*exp(-0.5*((x-[1])/([2]/(1-[3]+[3]*[4])))**2) + [3]*exp(-0.5*((x-[1])/([2]*[4]/(1-[3]+[3]*[4])))**2))", -1*this->range, this->range);

  }

  public:
  Float_t GetWidthA(){
    return widthA;
  }

  Float_t GetWidthB(){
    return widthB;
  }

  Float_t GetABsDifferenceWidths(){
    return abs(widthA-widthB);
  }

  Float_t GetAreaRatio(Float_t x1, Float_t x2){


    Float_t Area1 = gaussA->Integral(x1,x2);




    Float_t Area2 = gaussB->Integral(x1,x2);

    return Area2/Area1;
  }
};







//
