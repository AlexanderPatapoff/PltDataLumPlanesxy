#include <cmath>


#define Absolute false
#define PreceedErrors true
#define zoom false

struct CollisionData{
  vector<vector<Int_t>>* BCID;
  vector<vector<Double_t>>* LumData;
  vector<vector<Double_t>>* Error;
  vector<Double_t> *Seperation;
};
struct BeamDataDesc{
  Double_t scanRun;
  Int_t scanLumBlock;
  Int_t scanStep;
  char plane;
  Double_t planeCoord;
  int beamnumber;
};
struct BeamDataHandlerDesc{
  Int_t scanSteps;
};
struct GaussianDesc{
  bool sGauss;
  vector<Double_t> *chi2 = new vector<Double_t>();
  vector<Double_t> *widthRatio = new vector<Double_t>();
  vector<Double_t> *widthA = new vector<Double_t>();
  vector<Double_t> *widthB = new vector<Double_t>();
  vector<Double_t> *widthDifference = new vector<Double_t>();
  vector<Double_t> *mean = new vector<Double_t>();
  vector<Double_t> *areaRatio = new vector<Double_t>();
  vector<Double_t> *width = new vector<Double_t>();
  vector<Double_t> *peak = new vector<Double_t>();
  vector<vector<Int_t>> *BCIDs;
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
  Double_t range;
  TF1* function;
  bool functionfitted = false;

  public:
  string name;
  Function(string name, Double_t range){

    this->name = name;
    this->range = range;
    InitializeFunction();


  };

  virtual void InitializeFunction(){};

  void virtual Fit(TGraph* plot){};

  void virtual Fit(TGraphErrors * plot){};

  vector<Double_t> virtual GetAllData(){return vector<Double_t>();};
  Double_t GetChi2(TGraph* plot,const char* opt){
    plot->Fit(function, opt);

    return plot->Chisquare(function);
  };
  Double_t GetChi2(TGraphErrors* plot,const char* opt){
    plot->Fit(function, opt);

    return plot->Chisquare(function);
  };
  Double_t GetWidth(){
    return function->GetHistogram()->GetStdDev();
  }
  Double_t GetMean(){

    return function->GetHistogram()->GetMean(1);
  }
  Double_t Integrate(Double_t x1, Double_t x2){
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
  vector<TGraphErrors*> plots;

  public:

  Frame(Vector2D<Int_t> *size,int columns){
    this->size = size;
    this->columns = columns;
  };

  void AddPlot(TGraphErrors* plot){
    plots.push_back(plot);
  };

  void Draw(TCanvasFileWriter* writer){

    bool finished = false;



    TCanvas *canvas = new TCanvas("Frame","Guassians",size->GetX(),size->GetY());
    int y = plots.size()/columns;
    canvas->Divide(columns, y,0.01,0.01);
    int temp = 0;
    for (size_t i = 0; i < y; i++) {
      for (size_t p = 0; p < columns; p++) {
        canvas->cd(temp+1);
        //Stylize();
        plots.at(temp)->Draw("AP");
        temp++;
      }
    }

    writer->Write(canvas);


    delete canvas;
  /*
      TCanvas *c1 = new TCanvas("c1","multipads",900,700);
     gStyle->SetOptStat(0);
     c1->Divide(2,2,0,0);
     TH2F *h1 = new TH2F("h1","test1",10,0,1,20,0,20);
     TH2F *h2 = new TH2F("h2","test2",10,0,1,20,0,100);
     TH2F *h3 = new TH2F("h3","test3",10,0,1,20,-1,1);
     TH2F *h4 = new TH2F("h4","test4",10,0,1,20,0,1000);

     c1->cd(1);
     gPad->SetTickx(2);
     plots.at(0)->Draw("AP*");

     c1->cd(2);
     gPad->SetTickx(2);
     gPad->SetTicky(2);
     h2->GetYaxis()->SetLabelOffset(0.01);
     h2->Draw();

     c1->cd(3);
     h3->Draw();

     c1->cd(4);
     gPad->SetTicky(2);
     h4->Draw();
     */


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
    tree = (TTree*) filein->Get("vdMScanData;1");

    dataHandlerDesc.scanSteps = tree->GetEntries();
    this->beamNumber = beamNumber;

    SortCollisionDataBCID();
    SortBeamData();
    SortCollisionDataLum();
    SortCollisionError();
    ResetTree();

    filein->Close();

  }

  Int_t GetNSteps(){
    return dataHandlerDesc.scanSteps;
  }

  Int_t GetNCollisions(){
    Int_t temp = collision.BCID->at(0).size();
    return temp;
  }

  Double_t GetLumDataAt(int step,int index){
    return collision.LumData->at(step).at(index);
  }

  Int_t GetBCIDAt(int step, int index){
    return collision.BCID->at(step).at(index);
  }

  Double_t GetErrorAt(int step, int index){
    return collision.Error->at(step).at(index);
  }

  Double_t GetSeperation(int step){
    return beamDataDesc.at(step).planeCoord;
  }
  private:
  void SortBeamData(){


    string number;
    if(beamNumber ==0){number = "1";}else{ number = "2";}

    cout<< "sorting Beam data: " << beamNumber <<endl;
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



    }
    ResetTree();
  };

  void SortCollisionDataLum(){
    cout << "Retrieving Lum Data: " <<beamNumber << endl;
    string temp;
    if(beamNumber ==0){temp = "lucBiHitOR_BunchInstLumi";}else{temp = "lucBi2HitOR_BunchInstLumi";}

    int y = dataHandlerDesc.scanSteps;
    int z = collision.BCID->at(0).size();
    collision.LumData = new vector<vector<Double_t>> (y,vector<Double_t>(z));


    for (size_t i = 0; i < y; i++) {
      tree->GetEntry(i);
      for (size_t p = 0; p < z; p++) {
        collision.LumData->at(i).at(p)= tree->GetLeaf(temp.c_str())->GetValue(collision.BCID->at(i).at(p));

      }
    }
    ResetTree();


  };

  void SortCollisionDataBCID(){

    cout << "Retrieving BCIDs: " <<beamNumber<< endl;
    ResetTree();
    Int_t nBaskets = tree->GetLeaf("LuminousBCIDs")->GetLen();
    collision.BCID = new vector<vector<Int_t>>(dataHandlerDesc.scanSteps, vector<Int_t>(nBaskets));

    for (size_t p = 0; p < dataHandlerDesc.scanSteps; p++) {
      tree->GetEntry(p);
      for (size_t i = 0; i < nBaskets; i++) {
         collision.BCID->at(p).at(i) = tree->GetLeaf("LuminousBCIDs")->GetValue(i);
      }
    }
    ResetTree();

  };

  void SortCollisionError(){
    cout << "Retrieving Errors: " << beamNumber <<endl;
    collision.Error = new vector<vector<Double_t>> (dataHandlerDesc.scanSteps, vector<Double_t>(collision.BCID->at(0).size()));



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
        Double_t f = rate->GetValue(BCID)/Nt;
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
    cout <<"Initializing Collision"<<endl;
    this->beamA = beamA;
    this->beamB = beamB;
    CollideBeams();

  };

  void ReCalculateErrors(Double_t(*ErrorFunction)(Double_t,Double_t)){

    Int_t steps = beamA->GetNSteps();
    Int_t nCollisions = beamA->GetNCollisions();


    collision.Error = new vector<vector<Double_t>>(steps,vector<Double_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {

        Double_t tempError = ErrorFunction(beamA->GetLumDataAt(i,p),beamB->GetLumDataAt(i,p));
        collision.Error->at(i).at(p) = tempError;

      }
    }

    (*ErrorFunction);


  }

  void ReCalculateLums(Double_t(*LumFunction)(Double_t,Double_t)){

    Int_t steps = beamA->GetNSteps();
    Int_t nCollisions = beamA->GetNCollisions();


    collision.LumData = new vector<vector<Double_t>>(steps,vector<Double_t>(nCollisions));


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {

        Double_t tempLumData = LumFunction(beamA->GetLumDataAt(i,p),beamB->GetLumDataAt(i,p));
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

    x.LumData = new vector<vector<Double_t>>(steps, vector<Double_t>(nCollisions));
    x.BCID = new vector<vector<Int_t>>(steps, vector<Int_t>(nCollisions));
    x.Error = new vector<vector<Double_t>>(steps,vector<Double_t>(nCollisions));
    x.Seperation = new vector<Double_t>(steps);

    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {
        x.LumData->at(i).at(p) = collision.LumData->at(steps + i).at(p);
        x.BCID->at(i).at(p) = collision.BCID->at(steps + i).at(p);
        x.Error->at(i).at(p) = collision.Error->at(steps + i).at(p);
      }
      x.Seperation->at(i) = collision.Seperation->at(steps + i);
    }

    return x;
  }

  CollisionData GetCollisionY(){
    CollisionData x;
    int steps = collision.BCID->size()/2;
    int nCollisions = collision.BCID->at(0).size();

    x.LumData = new vector<vector<Double_t>>(steps, vector<Double_t>(nCollisions));
    x.BCID = new vector<vector<Int_t>>(steps, vector<Int_t>(nCollisions));
    x.Error = new vector<vector<Double_t>>(steps,vector<Double_t>(nCollisions));
    x.Seperation = new vector<Double_t>(steps);

    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {
        x.LumData->at(i).at(p) = collision.LumData->at(i).at(p);
        x.BCID->at(i).at(p) = collision.BCID->at(i).at(p);
        x.Error->at(i).at(p) = collision.Error->at(i).at(p);
      }

      x.Seperation->at(i) = collision.Seperation->at(steps + i);
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

    collision.LumData = new vector<vector<Double_t>>(steps, vector<Double_t>(nCollisions));
    collision.BCID = new vector<vector<Int_t>>(steps, vector<Int_t>(nCollisions));
    collision.Error = new vector<vector<Double_t>>(steps,vector<Double_t>(nCollisions));
    collision.Seperation = new vector<Double_t>(steps);
    cout <<"Calculating Collision Data"<<endl;


    for (size_t i = 0; i < steps; i++) {
      for (size_t p = 0; p < nCollisions; p++) {

        Double_t tempLumData = (beamA->GetLumDataAt(i,p) + beamB->GetLumDataAt(i,p))/2;
        collision.LumData->at(i).at(p) = tempLumData;

        collision.BCID->at(i).at(p) = beamA->GetBCIDAt(i,p);

        Double_t tempErrorData = sqrt(pow(beamB->GetErrorAt(i,p),2) + pow(beamA->GetErrorAt(i,p),2));
        collision.Error->at(i).at(p) = tempErrorData;

      }
      Double_t temp = beamA->GetSeperation(i) - beamB->GetSeperation(i);
      collision.Seperation->at(i) = temp;
    }
  }

};

class SingleGaussFunction: public Function{

  void InitializeFunction(){
    function = new TF1(this->name.c_str(), "gaus", (-1*range), range);
    cout<< "init Called"<<endl;
  }

  public:
  SingleGaussFunction(string name,Double_t range) : Function(name,range){
    InitializeFunction();
  }

  void Fit(TGraphErrors* plot){
    plot->Fit(function, "MESQ");
  }

  vector<Double_t> GetAllData(){
    vector<Double_t> data;
    data.push_back(GetMean());
    data.push_back(GetWidth());
    return data;
  }
};



class DoubleGaussFunction: public Function{
  Double_t widthA,widthB;

  TF1* gaussA, *gaussB;


  void InitializeFunction(){
    function = new TF1(this->name.c_str(), "[0]*( (1-[3])*exp(-0.5*((x-[1])/([2]/(1-[3]+[3]*[4])))**2) + [3]*exp(-0.5*((x-[1])/([2]*[4]/(1-[3]+[3]*[4])))**2))", -1*this->range, this->range);

  }


  public:
  DoubleGaussFunction(string name,Double_t range) :Function(name,range){
    InitializeFunction();
  }

  void Fit(TGraphErrors* plot){
    TF1 * fit = new TF1("h1","gaus",-1,1);

    plot->Fit("h1","MESQ");

    float p0,p1,p2,p3,p4,cdof2;
    float p0e,p1e,p2e;


    p0 = fit->GetParameter(0);

    p1 = fit->GetParameter(1);

    p2 = fit->GetParameter(2);

  //  #if PreceedErrors
      p0e = fit->GetParError(0);
      p1e = fit->GetParError(1);
      p2e = fit->GetParError(2);
      function->SetParError(0,p0e);
      function->SetParError(1,p1e);
      function->SetParError(2,p2e);
  //  #endif


    function->SetParameter(0,p0);
    //function->SetParError(0,p0e);
    function->SetParameter(1,p1);
    //function->SetParError(1,p1e);
    function->SetParameter(2,p2);
    //function->SetParError(2,p2e);
    function->SetParameter(3,0.5);
    function->SetParameter(4,5.0);
    function->SetParLimits(4,0,1000);


    plot->Fit(this->name.c_str(),"MESQ");

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

  vector<Double_t> GetAllData(){
    vector<Double_t> data;
    data.push_back(GetMean());
    data.push_back(GetWidth());
    data.push_back(GetWidthA());
    data.push_back(GetWidthB());
    data.push_back(GetWidthDifference());
    data.push_back(GetAreaRatio(-1,1));

    return data;
  }

  Double_t GetWidthA(){
    return widthA;
  }

  Double_t GetWidthB(){
    return widthB;
  }

  Double_t GetWidthRation(){
    return widthA/widthB;
  }

  Double_t GetWidthDifference(){
    return (widthA-widthB);
  }

  Double_t GetAreaRatio(Double_t x1, Double_t x2){

#if Absolute
    Double_t Area1 = abs(gaussA->Integral(x1,0));
    Area1 += abs(gaussA->Integral(0,x2));



    Double_t Area2 = abs(gaussB->Integral(x1,0));
    Area2 += abs(gaussB->Integral(0,x2));
    #else
    Double_t Area1 = gaussA->Integral(x1,x2);

    Double_t Area2 = gaussB->Integral(x1,x2);


#endif

    return Area1/Area2;

  }
};

class AnalysisEngine{
  BeamCollisionHandler * collision;
  Function * function;
  CollisionData x,y;
  TCanvasFileWriter* writer;
  void (*Task)(GaussianDesc*,Function*,TGraphErrors*);

  public:

  AnalysisEngine(BeamCollisionHandler* collision,Function* function,void (*TT)(GaussianDesc*,Function*,TGraphErrors*),string fileName){
    this->collision = collision;
    this->function = function;
    this->Task = TT;
    writer = new TCanvasFileWriter(fileName);
    writer->OpenFile();
    OrganizeData();
    PlotData();
    writer->CloseFile();
  }





  void OrganizeData(){
    x = collision->GetCollisionX();
    y = collision->GetCollisionY();
  }

  Frame SortTaskData(GaussianDesc* x,int axis){
    Vector2D<Int_t> * point;
    string number;
    if(x->sGauss){
      point = new Vector2D<Int_t>(450,300);
      }else{
      point = new Vector2D<Int_t>(600,600);
    }
    if(axis ==0){number = "X";}else{number = "Y";}
    Frame frame(point,2);
    string temp;

    Double_t bcid[x->peak->size()];
    copy(x->BCIDs->at(0).begin(), x->BCIDs->at(0).end(), bcid);

    TGraphErrors* peak = new TGraphErrors(x->peak->size(),bcid,x->peak->data(),0,0);
    temp = "peak: " + number;
    peak->SetTitle(temp.c_str());

    TGraphErrors* mean = new TGraphErrors(x->mean->size(),bcid,x->mean->data(),0,0);
    temp = "mean: " + number;
    mean->SetTitle(temp.c_str());

    TGraphErrors* width = new TGraphErrors(x->width->size(),bcid,x->width->data(),0,0);
    temp = "width: " + number;
    width->SetTitle(temp.c_str());

    TGraphErrors* chi2 = new TGraphErrors(x->chi2->size(),bcid,x->chi2->data(),0,0);
    //chi2->GetYaxis()->SetRangeUser(0,500);
    temp = "chi2: " + number;
    chi2->SetTitle(temp.c_str());



    Stylize(peak);
    Stylize(mean);
    Stylize(width);
    Stylize(chi2);

    frame.AddPlot(peak);
    frame.AddPlot(mean);
    frame.AddPlot(width);
    frame.AddPlot(chi2);



    if(x->sGauss == false){
      TGraphErrors* widthA = new TGraphErrors(x->peak->size(),bcid,x->widthA->data(),0,0);
      temp = "width gaussian 1: " + number;
      widthA->SetTitle(temp.c_str());

      TGraphErrors* widthB = new TGraphErrors(x->peak->size(),bcid,x->widthB->data(),0,0);
      temp = "width gaussian 2: " + number;
      widthB->SetTitle(temp.c_str());

      TGraphErrors* widthDifference = new TGraphErrors(x->peak->size(),bcid,x->widthDifference->data(),0,0);
      temp = "width Differnce: " + number;
      widthDifference->SetTitle(temp.c_str());


      TGraphErrors* widthRatio = new TGraphErrors(x->peak->size(),bcid,x->widthRatio->data(),0,0);
      temp = "widthRatio: gaussian1/gaussian2: " + number;
      widthRatio->SetTitle(temp.c_str());

      TGraphErrors* areaRatio = new TGraphErrors(x->peak->size(),bcid,x->areaRatio->data(),0,0);
      temp = "areaRatio: gaussian1/gaussian2:" + number;
      areaRatio->SetTitle(temp.c_str());

      #if zoomed
        widthRatio->GetYaxis()->SetRangeUser(0,1);
        widthDifference->GetYaxis()->SetRangeUser(-0.02,0.02);
        widthA->GetYaxis()->SetRangeUser(-0.01,0.02);
        chi2->GetYaxis()->SetRangeUser(0,100);
        areaRatio->GetYaxis()->SetRangeUser(0,2.3);
      #endif

      Stylize(widthA);
      Stylize(widthB);
      Stylize(widthDifference);
      Stylize(widthRatio);
      Stylize(areaRatio);

      frame.AddPlot(widthA);
      frame.AddPlot(widthB);
      frame.AddPlot(widthDifference);
      frame.AddPlot(widthRatio);
      frame.AddPlot(areaRatio);
      frame.AddPlot(areaRatio);


    }


    return frame;




  }



  void PlotData(){
    cout<<"plotting Data"<<endl;
    vector<Frame> frames;

    Vector2D<Int_t> * point = new Vector2D<Int_t>(450,300);

    GaussianDesc xDesc;
    GaussianDesc yDesc;


    int steps = x.BCID->size();
    int length = x.BCID->at(0).size();

    for (size_t p = 0; p < length; p++) {
      Frame frame(point, 2);

      Double_t x1[steps],y1[steps],e1[steps],sep1[steps];
      Double_t x2[steps],y2[steps],e2[steps],sep2[steps];

      for (size_t i = 0; i < steps; i++) {
        x1[i] = x.Seperation->at(i);
        y1[i] = x.LumData->at(i).at(p);
        e1[i] = x.Error->at(i).at(p);

        x2[i] = y.Seperation->at(i);
        y2[i] = y.LumData->at(i).at(p);
        e2[i] = y.Error->at(i).at(p);
      }

      TGraphErrors* LumPlotX = new TGraphErrors(steps,x1,y1,0,e1);
      string namex = "X LumData" + to_string(x.BCID->at(0).at(p));
      LumPlotX->SetTitle(namex.c_str());
      function->Fit(LumPlotX);
      frame.AddPlot(LumPlotX);
      xDesc.BCIDs = x.BCID;

      Task(&xDesc,function,LumPlotX);




      CalculateSeperation(sep1,x1,y1,steps,function->GetFunction());

      TGraphErrors* LumPlotXSep = new TGraphErrors(steps,x1,sep1,0,0);
      //frame.AddPlot(LumPlotXSep);







      TGraphErrors* LumPlotY = new TGraphErrors(steps,x2,y2,0,e2);
      string namey = "Y LumData" + to_string(x.BCID->at(0).at(p));
      LumPlotY->SetTitle(namey.c_str());

      function->Fit(LumPlotY);

      frame.AddPlot(LumPlotY);

      yDesc.BCIDs = y.BCID;
      Task(&yDesc,function,LumPlotY);



      CalculateSeperation(sep2,x2,y2,steps,function->GetFunction());

      TGraphErrors* LumPlotYSep = new TGraphErrors(steps,x2,sep2,0,0);
      frame.AddPlot(LumPlotYSep);

      //frames.push_back(frame);





    }

    frames.push_back(SortTaskData(&xDesc,0));
    frames.push_back(SortTaskData(&yDesc,1));


    for (size_t i = 0; i < frames.size(); i++) {
      frames.at(i).Draw(writer);
    }


  }


  void CalculateSeperation(Double_t* sep,Double_t* x,Double_t* y,int size, TF1* function){


    for (size_t i = 0; i < size; i++) {
      sep[i] = -1*function->Eval(x[i]) + y[i];
    }


  }

  void Stylize(TGraphErrors * plot){
    plot->SetMarkerStyle(18);
    plot->SetMarkerSize(0.2);
    plot->SetMarkerColor(2);
  }



};

class FrameComparison{
  vector<Frame> frames;
  TCanvasFileWriter * writer;
  public:

  FrameComparison(TCanvasFileWriter * writer){
    this->writer = writer;
  }

  void AddFrame(Frame frame){
    this->frames.push_back(frame);
  }

  void Draw(){
    for (size_t i = 0; i < frames.size(); i++) {
      frames.at(i).Draw(writer);
    }
  }


};




//
