#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TStyle.h"
#include "TColor.h"

//#include "pngwriter.h"

// OpenCV
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"

const int NPLANES = 3;
const float BASELINE = 0;
const float ADC_MIP = 20.0;
const float ADC_MIN = -10;
const float ADC_MAX = 190;
bool write_images = true;
bool use_whole_image = false;

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

void getRGB( float value, float& r, float& g, float& b ) {
  // out of range
  if ( value<ADC_MIN ) {
    r = 0; g = 0; b = 1.0;
    return;
  }
  if ( value>ADC_MAX ) {
    r = 1.0; g = 0; b = 0;
  }

  // 0 to 1.0 MIPs: blue to green
  if ( value < ADC_MIP ) {
    float colorlen = ADC_MIP - ADC_MIN;
    g = (value-ADC_MIN)/colorlen;
    b = (1 - (value-ADC_MIN)/colorlen);
    r = 0;
  }
  // 1.0 to 2.0 MIPs green to red
  else if ( value>=ADC_MIP ) {
    float colorlen = ADC_MAX - ADC_MIP;
    b = 0;
    g = (value-ADC_MIP)/colorlen;
    r = (1.0 - (value-ADC_MIP)/colorlen);
  }
}

int main( int narg, char** argv ) {

  std::string infile = argv[1];
  std::string outdir = argv[2];
  std::string outfilelist = argv[3];

  std::vector<std::string> inputlist;
  parse_inputlist( infile, inputlist );


  std::ofstream outlist( outfilelist.c_str());

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
  int run, subrun, event;
  char label[100];
  // need to add height,width
  std::vector< int >* pImgPlane0 = 0;
  std::vector< int >* pImgPlane1 = 0;
  std::vector< int >* pImgPlane2 = 0;
  bbtree->SetBranchAddress("run", &run );
  bbtree->SetBranchAddress("subrun", &subrun );
  bbtree->SetBranchAddress("event", &event );
  bbtree->SetBranchAddress("label",label);
  bbtree->SetBranchAddress("img_plane0", &pImgPlane0 );
  bbtree->SetBranchAddress("img_plane1", &pImgPlane1 );
  bbtree->SetBranchAddress("img_plane2", &pImgPlane2 );

  // entire image tree
  int img_run, img_subrun, img_event;
  char img_label[50];
  std::vector<int>* pimg_plane0 = 0;
  std::vector<int>* pimg_plane1 = 0;
  std::vector<int>* pimg_plane2 = 0;
  imgtree->SetBranchAddress("run", &img_run );
  imgtree->SetBranchAddress("subrun", &img_subrun );
  imgtree->SetBranchAddress("event", &img_event );
  imgtree->SetBranchAddress("label",img_label);
  imgtree->SetBranchAddress("img_plane0", &pimg_plane0 );
  imgtree->SetBranchAddress("img_plane1", &pimg_plane1 );
  imgtree->SetBranchAddress("img_plane2", &pimg_plane2 );


  TChain* ttree = NULL;
  if ( use_whole_image )
    ttree = imgtree;
  else
    ttree = bbtree;
  int entry = 0;
  unsigned long bytes = ttree->GetEntry(entry);

  // Use first entry to determine image size
  int height;
  int width; 
  if ( use_whole_image ) {
    height = sqrt(pimg_plane2->size());
    width  = sqrt(pimg_plane2->size());
  }
  else {
    height = sqrt(pImgPlane2->size());
    width = sqrt(pImgPlane2->size());
  }
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
    std::string strlabel;
    if (use_whole_image) {
      strlabel = img_label;
      //strlabel = "pizero";
      strlabel = strlabel.substr(strlabel.find_last_of("_")+1,std::string::npos);
    }
    else
      strlabel = label;
    std::cout << "Entry " << entry << ": " << strlabel << std::endl;
    std::stringstream fname;
    if ( use_whole_image )
      fname << outdir << "/" << strlabel << "_" << img_run << "_" << img_subrun << "_" << img_event << ".JPEG";
    else
      fname << outdir << "/" << strlabel << "_" << run << "_" << subrun << "_" << event << ".JPEG";
    std::cout << " fname: " << fname.str() << std::endl;

    //pngwriter img( height, width, 0.0, "test.png" );
    cv::Mat img( height, width, CV_8UC3 );
    
    // color gradient
    for (int h=0; h<height; h++) {
      for (int w=0; w<width; w++) {
	float val = BASELINE;
	if ( use_whole_image )
	  val = (float)(pimg_plane2->at( w*height + h )-BASELINE);
	else
	  val = (float)(pImgPlane2->at( w*height + h )-BASELINE);

	// set scale
	float r,g,b;
	getRGB( val, r, g, b );

	// get pixel
	cv::Vec3b color = img.at<cv::Vec3b>(cv::Point(h,w));
	img.at<cv::Vec3b>(cv::Point(h,w))[0] = b*255;
	img.at<cv::Vec3b>(cv::Point(h,w))[1] = g*255;
	img.at<cv::Vec3b>(cv::Point(h,w))[2] = r*255;
      }
    }
    if ( write_images )
      cv::imwrite( fname.str(), img );
    outlist << fname.str() << std::endl;
    //img.write_png();

    entry++;
    bytes = ttree->GetEntry(entry);
    //#if ( entry>0 )
    //#  break;
  }


  return 0;
}
