#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"

#include "constants.h"
#include "bandhit.h"
#include "TClonesArray.h"
#include "kinematic_cuts.h"

using namespace std;

int slc[6][5] = {{3,7,6,6,2},{3,7,6,6,2},{3,7,6,6,2},{3,7,6,6,2},{3,7,5,5,0},{3,7,6,6,2}};

int main(int argc, char ** argv){
	// Set style
	gStyle->SetOptFit(1);

	if (argc < 4){
		cerr << "Wrong number of arguments. Instead use\n"
			<< "\t./code [TDC(0) or FADC(1)] [outputTxtfile] [outputPDFfile] [inputDatafiles]\n";
		return -1;
	}

	int TDCorFADC = atoi(argv[1]);

	// Initialize histograms, fits, and canvases for output
	TH1D **** ToF_spec = new TH1D***[5];
	TF1  **** ToF_fits = new TF1 ***[5];
	TF1  **** ToF_fits_it = new TF1 ***[5];
	TCanvas **** cSLC = new TCanvas***[5];	
	// 	for every bar, we need a fit function, a histogram, and canvas to draw
	for( int sector = 0; sector < 5; sector++){
		ToF_spec[sector] = new TH1D**[5];
		ToF_fits[sector] = new TF1 **[5];
		ToF_fits_it[sector] = new TF1 **[5];
		cSLC[sector] = new TCanvas**[5];
		for( int layer = 0; layer < 5; layer++){
			ToF_spec[sector][layer] = new TH1D*[7];
			ToF_fits[sector][layer] = new TF1 *[7];
			ToF_fits_it[sector][layer] = new TF1 *[7];
			cSLC[sector][layer] = new TCanvas*[7];
			for(int component = 0; component < slc[layer][sector]; component++){
				ToF_spec[sector][layer][component] = new TH1D(Form("ToF_spec_%i_%i_%i",(sector+1),(layer+1),(component+1)),Form("ToF_spec_%i_%i_%i",(sector+1),(layer+1),(component+1)),400,-14.975,25.025);
			}
		}
	}

	// Loop over all the files that are given to me to get the best statistics per bar
	for( int i = 4 ; i < argc ; i++ ){
		TFile * inFile = new TFile(argv[i]);
		if (!(inFile->GetListOfKeys()->Contains("calib"))){
			cerr << "File has no entries\n";
			return -2;
		}
		TTree * inTree = (TTree*)inFile->Get("calib");

		//	Event info:
		int Runno		= 0;
		double Ebeam		= 0;
		double gated_charge	= 0;
		double livetime		= 0;
		double starttime	= 0;
		double current		= 0;
		bool goodneutron 	= false;
		int nleadindex	 	= -1;
		// 	Neutron info:
		int nMult		= 0;
		TClonesArray * nHits = new TClonesArray("bandhit");
		// 	Event branches:
		inTree->SetBranchAddress("Runno"		,&Runno			);
		inTree->SetBranchAddress("Ebeam"		,&Ebeam			);
		inTree->SetBranchAddress("gated_charge"		,&gated_charge		);
		inTree->SetBranchAddress("livetime"		,&livetime		);
		inTree->SetBranchAddress("starttime"		,&starttime		);
		inTree->SetBranchAddress("current"		,&current		);
		//	Neutron branches:
		inTree->SetBranchAddress("nMult"		,&nMult			);
		inTree->SetBranchAddress("nHits"		,&nHits			);
		inTree->SetBranchAddress("goodneutron"		,&goodneutron		);
		inTree->SetBranchAddress("nleadindex"		,&nleadindex		);

		// Start working on one of the files, looping over all of the events
		cout << "Working on file: " << argv[i] << "\n";
		for( int ev = 0 ; ev < inTree->GetEntries() ; ev++ ){
			if( ev % 1000000 == 0 ) cout << "\ton event " << ev << "\n";

			// Clear all branches before getting the entry from tree
			Runno		= 0;
			Ebeam		= 0;
			gated_charge	= 0;
			livetime	= 0;
			starttime	= 0;
			current		= 0;
			goodneutron 	= false;
			nleadindex	= -1;
			// 	Neutron info:
			nMult		= 0;
			nHits->Clear();
			
			inTree->GetEntry(ev);

			// Only look for "photon" events
			bandhit * this_photon = (bandhit*) nHits->At(0);
			if( nMult 			!= 1 ) continue;
			if( this_photon->getStatus() 	!= 0 ) continue;
			if( TDCorFADC == 0 )
				if( this_photon->getTof() 	== 0 ) continue;
			if( TDCorFADC == 1 )
				if( this_photon->getTofFadc() 	== 0 ) continue;
			if( this_photon->getEdep()	< 1*DataAdcToMeVee ) 	continue;

			double time 	= 0;
			if( TDCorFADC == 0 )
				time 	= this_photon->getTof();
			if( TDCorFADC == 1 )
				time 	= this_photon->getTofFadc();
			double dL 	= this_photon->getDL().Mag();

			double tof = time - dL/cAir;
			int sector 	= this_photon->getSector();
			int layer 	= this_photon->getLayer();
			int component 	= this_photon->getComponent();

			ToF_spec[sector-1][layer-1][component-1]->Fill(tof);
		} // end loop over events

		inFile->Close();
	}// end loop over files



	ofstream out_file;
	out_file.open(argv[2]);

	TCanvas * c0 = new TCanvas("c0","c0",900,900);
	TString openname = string(argv[3]) + "(";
	c0 -> Print(openname);
	for( int sector = 0; sector < 5; sector++){
		for( int layer = 0; layer < 5; layer++){
			for(int component = 0; component < slc[layer][sector]; component++){

				// Create canvas
				cSLC[sector][layer][component] = new TCanvas(Form("S%iL%iC%i",sector+1,layer+1,component+1),Form("Sector %i, Layer %i, Component %i",sector+1,layer+1,component+1),900,900);


				if( ToF_spec[sector][layer][component]->Integral() == 0 ){
					out_file << (sector+1) << " " << (layer+1) << " " << (component+1) << " " 
						<< 0 << " " << 0 << " " 
						<< 0 << " " << 0 << " "
						<< 0 << " " << -1 << "\n";
					continue;
				}

				// Get the min and max of the fit based on assuming 0.3ns resolution and the peak position
				double sig_guess = 0.2;

				double max = ToF_spec[sector][layer][component]->GetMaximum();
				if( (sector+1) == 3 || (sector+1) == 4){
					ToF_spec[sector][layer][component]->Rebin(2);
					//max = 0.8*max;
				}
				//else{ max = 0.8*max; }
				double max_pos = ToF_spec[sector][layer][component]->GetXaxis()->GetBinCenter( ToF_spec[sector][layer][component]->GetMaximumBin() );
				//max_pos = ToF_spec[sector][layer][component]->GetXaxis()->GetBinCenter( ToF_spec[sector][layer][component]->FindFirstBinAbove(max) );

				// Set parameters of the fit before fitting:
				double background_lvl = 0.;
				for( int i = 1; i < 25; i++){
					background_lvl += ToF_spec[sector][layer][component]->GetBinContent(i);
				}
				background_lvl /= 24;

				double min_fit = max_pos - 10;
				double max_fit = max_pos + 4.*sig_guess;
				ToF_fits[sector][layer][component] = new TF1(Form("ToF_fits_%i_%i_%i",sector,layer,component),"pol0+gaus(1)",min_fit,max_fit);
				// background level:
				ToF_fits[sector][layer][component]->SetParameter(0,background_lvl);
				// constant of gaus:
				ToF_fits[sector][layer][component]->SetParameter(1,max);
				// mean of gaus:
				ToF_fits[sector][layer][component]->SetParameter(2,max_pos);
				// sigma of gaus:
				ToF_fits[sector][layer][component]->SetParameter(3,sig_guess);

				ToF_spec[sector][layer][component]->Fit(ToF_fits[sector][layer][component],"QESR");

				// Now do another iteration of fits with the current parameters only if the parameters are reasonable
				if( ToF_fits[sector][layer][component]->GetParameter(3) < 0.4 && ToF_fits[sector][layer][component]->GetParameter(3) >= 0.1 ){
					sig_guess = ToF_fits[sector][layer][component]->GetParameter(3);
					max_fit = ToF_fits[sector][layer][component]->GetParameter(2) - 5;
					min_fit = ToF_fits[sector][layer][component]->GetParameter(2) + 2*sig_guess;
					ToF_fits_it[sector][layer][component] = new TF1(Form("ToF_fits_%i_%i_%i_it",sector,layer,component),"pol0+gaus(1)",min_fit,max_fit);
					ToF_fits_it[sector][layer][component]->SetLineColor(4);
					ToF_fits_it[sector][layer][component]->SetParameter(0, ToF_fits[sector][layer][component]->GetParameter(0) );
					ToF_fits_it[sector][layer][component]->SetParameter(1, ToF_fits[sector][layer][component]->GetParameter(1) );
					ToF_fits_it[sector][layer][component]->SetParameter(2, ToF_fits[sector][layer][component]->GetParameter(2) );
					ToF_fits_it[sector][layer][component]->SetParameter(3, ToF_fits[sector][layer][component]->GetParameter(3) );
					ToF_spec[sector][layer][component]->Fit(ToF_fits_it[sector][layer][component],"QESR");

					out_file << (sector+1) << " " << (layer+1) << " " << (component+1) << " " 
						<< ToF_fits_it[sector][layer][component]->GetParameter(0) << " " << ToF_fits_it[sector][layer][component]->GetParameter(1) << " " 
						<< ToF_fits_it[sector][layer][component]->GetParameter(2) << " " << ToF_fits_it[sector][layer][component]->GetParameter(3) << " "
						<< ToF_spec[sector][layer][component]->Integral() << " " << 1 << "\n";
				}
				else{
					out_file << (sector+1) << " " << (layer+1) << " " << (component+1) << " " 
						<< ToF_fits[sector][layer][component]->GetParameter(0) << " " << ToF_fits[sector][layer][component]->GetParameter(1) << " " 
						<< ToF_fits[sector][layer][component]->GetParameter(2) << " " << ToF_fits[sector][layer][component]->GetParameter(3) << " "
						<< ToF_spec[sector][layer][component]->Integral() << " " << 0 << "\n";

				}

				cSLC[sector][layer][component]->cd(1);
				ToF_spec[sector][layer][component]->Draw();
				//ToF_fits[sector][layer][component]->Draw("SAME");

				cSLC[sector][layer][component]->Update();
				cSLC[sector][layer][component]->Modified();
				cSLC[sector][layer][component] -> Print(argv[3]);
				//cSLC[sector][layer][component]->Write();
			}
		}
	}
	TString endname = string(argv[3]) + ")";
	c0 -> Print(endname);

	out_file.close();


	return 0;
}

