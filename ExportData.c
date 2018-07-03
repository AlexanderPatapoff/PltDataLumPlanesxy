enum Format{PDF,PNG,TXT};

class ExportDataHandler{
public:

  TImage *file;
  int x,y;
  int sizex,sizey;
  Format form;
  string name;
  ExportDataHandler(string fileName,Format format){
    file = TImage::Create();
    name = fileName;
    form = format;
    x = 0;
    y = 0;

  };

  void Save(TCanvas* fileC){
    file->FromPad(fileC,x,y,fileC->GetWindowWidth(),fileC->GetWindowHeight());
    y +=fileC->GetWindowHeight();
  }

  void Write(){
    const char* tempName;
    switch (form) {
      case PDF: tempName = (name + ".pdf").c_str(); cout << tempName << endl;break;
      case PNG: tempName = (name + ".png").c_str(); break;
      case TXT: tempName = (name + ".txt").c_str(); break;
    }
    file->WriteImage(tempName);
  };


  void Finish(){
    delete file;
  }

};
