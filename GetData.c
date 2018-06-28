#include "Engine.c"


//structure meant to hold data on current scan info TODO: make lumData non static array size
struct ID{
  Float_t scanRun;
  Int_t scanLumBlock;
  Int_t scanStep;
  char plane;
  Float_t planeCoord;
  int beamNumber;
  Int_t lumData [3564];

};

//Graph the IDS   ****NOT MODULABLE**** TODO: ADD FLEXIBILITY
void GraphIDs(Int_t data[]){
  TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,500,300);
  Double_t x[5], y[5];
  for (Int_t i=0;i<64000;i++) {
    if(data[i] !=0){
      y[data[i]] +=1;

    }

  }
  for (size_t i = 0; i < 5; i++) {
    x[i] = i;
  }
  TGraph* gr = new TGraph(5,x,y);
  gr->SetFillColor(40);
  gr->Draw("AB");
}

//Writes the ID sturct into console
void printID(ID id){

  cout <<id.scanRun <<" " <<id.scanLumBlock << " "<<id.scanStep << " "<< id.plane<< " "<< id.planeCoord << endl;

}
//NULL
void Test(){
  cout <<"test"<< endl;
  Engine();
}
//Entry function of the
void GetData(){

   TFile *filein = new TFile("file:~/Downloads/scan.root","READ");
   TTree *tree = (TTree*) filein->Get("vdMScanData;1");
   Int_t nentries = tree->GetEntries();

   ID idEntry [18];
  // List<Int_t[]> totalData;
   cout <<"scanRun scanLB scanStep plane planeCoord" << endl;
   for (size_t i = 0; i < nentries; i++) {

     tree->GetEntry(i);
     idEntry[i].scanRun = tree->GetLeaf("ScanRun")->GetValue(0);
     idEntry[i].scanLumBlock = tree->GetLeaf("ScanLB")->GetValue(0);
     idEntry[i].scanStep = tree->GetLeaf("ScanStep")->GetValue(0);
     if( i >= 8)
     {
       idEntry[i].plane = 'x';
       idEntry[i].planeCoord = tree->GetLeaf("B1DeltaXSet")->GetValue(0);

     }else{
       idEntry[i].plane = 'y';
       idEntry[i].planeCoord = tree->GetLeaf("B1DeltaYSet")->GetValue(0);
     }

     Int_t nBaskets = tree->GetLeaf("lucBiHitOR_BunchInstLumi")->GetLen();


     for (size_t s = 0; s < nBaskets; s++) {
       idEntry[i].lumData[s] = tree->GetLeaf("lucBiHitOR_BunchInstLumi")->GetValue(s);
     }



     printID(idEntry[i]);
   }
   //GraphIDs(idEntry[4].lumData);
   Int_t total [64152];
   Int_t x =0;
   for(int i =0; i < 18; i++){
     for (size_t p = 0; p < 3564; p++) {
       total[x] = idEntry[i].lumData[p];
       x = x+1;
     }
   }
   GraphIDs(total);



/* attempt
t1->SetBranchAddress("px",&px);
   t1->SetBranchAddress("py",&py);
   t1->SetBranchAddress("pz",&pz);
   t1->SetBranchAddress("random",&random);
   t1->SetBranchAddress("ev",&ev);
*/

   filein->Close();

}
