#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TStyle.h"
#include "TColor.h"

const int NPLANES = 3;
const float BASELINE = 400;
const float ADC_MIP = 20.0;
const float min_adc = -10;
const float max_adc = 190;

void parse_inputlist( std::string filename, std::vector< std::string >& inputlist ) {

  std::ifstream infile( filename.c_str() );
  char buffer[5120];
  std::string fname;
  std::string lastname = "";
  while ( !infile.eof() ) {
    infile >> buffer;
    fname = buffer;
    if ( fname!="" && fname!=lastname ) {
      inputlist.push_back( fname );
      lastname = fname;
    }
  }
  
}

void rescale_image( const std::vector<int>& img_v, std::vector<float>& out ) {
  
}

int main( int narg, char** argv ) {

  std::string infile = argv[1];
  std::string outdb = argv[2];

  std::vector<std::string> inputlist;
  parse_inputlist( infile, inputlist );


  // For each event we do a few things: 
  // (1) Calculate mean of all images
  // (2) normalize image scale (so greyscale) or fix RGB scale
  // (3) put data into datum object
  // (4) store that object into an lmdb database
  // (5) make the training list with labels

  // Load ROOT Trees
  std::cout << "[LOAD TREE]" << std::endl;
  TChain* bbtree = new TChain("yolo/bbtree");
  TChain* imgtree = new TChain("yolo/imgtree");
  int nfiles = 0;
  for ( std::vector<std::string>::iterator it=inputlist.begin(); it!=inputlist.end(); it++ ) {
    bbtree->Add( (*it).c_str() );
    imgtree->Add( (*it).c_str() );
    std::cout << " " << nfiles << ": " << (*it) << std::endl;
    nfiles++;
  }
  
  // [ Branches ]
  // bounding box tree
  char label[100];
  // need to add height,width
  std::vector< int >* pImgPlane0 = 0;
  std::vector< int >* pImgPlane1 = 0;
  std::vector< int >* pImgPlane2 = 0;
  bbtree->SetBranchAddress("label",label);
  bbtree->SetBranchAddress("img_plane0", &pImgPlane0 );
  bbtree->SetBranchAddress("img_plane1", &pImgPlane1 );
  bbtree->SetBranchAddress("img_plane2", &pImgPlane2 );

  // entire image tree
  //...

  int entry = 0;
  unsigned long bytes = bbtree->GetEntry(entry);

  // Use first entry to determine image size
  int height= sqrt(pImgPlane2->size());
  int width = sqrt(pImgPlane2->size());
  std::cout << "IMAGE SIZE: " << width << " x " << height << std::endl;

  // Set the color scale
  Int_t  colors[50];
  Double_t Red[3]    = { 1.00, 0.00, 0.00};
  Double_t Green[3]  = { 0.00, 1.00, 0.00};
  Double_t Blue[3]   = { 1.00, 0.00, 1.00};
  Double_t Length[3] = { 1.00, 0.50, 0.00 }; // 0 (blue) -> 1.0 (red)
  Int_t FI = TColor::CreateGradientColorTable(3,Length,Red,Green,Blue,50);
  for (int i=0; i<50; i++) {
    colors[i] = FI+i;
    std::cout << "color [" << i << "] " << colors[i] << std::endl;
  }
  gStyle->SetPalette(50,colors);

  std::vector< float > img_rescale_plane2;
  img_rescale_plane2.resize(height*width);

  while ( bytes!=0 ) {
    std::cout << "Entry " << entry << ": " << label << std::endl;

    // color gradient
    for (int h=0; h<height; h++) {
      for (int w=0; w<width; w++) {
	float val = (float)pImgPlane2->at( w*height + h );
	// set scale
	
      }
    }

    entry++;
    bytes = bbtree->GetEntry(entry);
    if ( entry>100 )
      break;
  }


  return 0;
}