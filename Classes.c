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
  Plot(TGraph* plot){
    this.plot->plot;
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
    int xadjust = size->GetX() - size->GetX()/columns

    int temp = 0;
    while(!(temp == plots->size())){

      for (size_t i = 0; i < columns; i++) {
        TPad *pad = new TPad("Graph","",0,0,1,1);
        pad->setBBoxX2(xadjust);
        pad->setBBoxY2(yadjust);
        pad->SetBBoxCenterX((size->GetX()/columns + size->GetX()/columns*i)/2);
        pad->SetBBoxCenterY((ySize + row * ySize)/2)
        pad->Draw();
        pad->cd();

        plots->at(i)->Draw();



        temp++;


      }



    }

  }





};
