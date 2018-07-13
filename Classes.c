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
