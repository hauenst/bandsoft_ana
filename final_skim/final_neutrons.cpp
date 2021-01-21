#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include "TClonesArray.h"
#include "TEventList.h"
#include "TCut.h"
#include "TChain.h"
#include "TVector3.h"
#include "TH1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

#include "constants.h"
#include "kinematic_cuts.h"
#include "bandhit.h"
#include "clashit.h"
#include "taghit.h"

// For processing data

using namespace std;

int slc[6][5] = {{3,7,6,6,2},{3,7,6,6,2},{3,7,6,6,2},{3,7,6,6,2},{3,7,5,5,0},{3,7,6,6,2}};

int main(int argc, char ** argv){
	if (argc < 3){
		cerr << "Incorrect number of arugments. Instead use:\n\t./code [outputFile] [MC/DATA] [inputFiles] \n\n";
		cerr << "\t\t[outputFile] = ____.root\n";
		cerr << "\t\t[<MC,DATA> = <0, 1> \n";
		cerr << "\t\t[inputFile] = ____.root ____.root ____.root ...\n\n";
		return -1;
	}
	int MC_DATA_OPT = atoi(argv[2]);

	// Conditions for a final accepted event neutron - signal or background
	TCut nGood	= Form("goodneutron == %i",					NCUT_goodneutron);
	TCut nLeadIdx	= Form("nleadindex != %i",					NCUT_leadindex);
	TCut nStatus	= Form("nHits[nleadindex]->getStatus() == %i",			NCUT_status);
	TCut nEdep;
	if( MC_DATA_OPT == 0 )
		nEdep	= Form("nHits[nleadindex]->getPmtLadc() > %f",			NCUT_Edep*SimAdcToMeVee);
	else if( MC_DATA_OPT == 1)
		nEdep	= Form("nHits[nleadindex]->getEdep() > %f",			NCUT_Edep*DataAdcToMeVee);
	TCut neutron 	= nGood && nLeadIdx && nStatus && nEdep;

	// Final TCut:
	TString cut = Form("%s",neutron.GetTitle());

	// Load input files
	TChain* inTree = new TChain("neutrons");
	for( int i = 3 ; i < argc; i++ ){
		cout << "Adding file " << argv[i] << endl;
		inTree->Add(argv[i]);
	}

	// Create an output rootfile
	TFile * outFile = new TFile(argv[1],"RECREATE");
	inTree->LoadTree(0);
	TTree * outTree = inTree->CloneTree(0);


	// get background normalization level for this neutron PID
	TH1D * hToF_bac = new TH1D("hToF_bac","hToF_bac",1000,-25,75);
	inTree->Draw("nHits[nleadindex]->getTofFadc() / (nHits[nleadindex]->getDL().Mag()/100.) >> hToF_bac",cut);
	TVector3 bacnorm;
	if( MC_DATA_OPT == 1 ){
		TFitResultPtr fit = (TFitResultPtr)hToF_bac->Fit("pol0","QESR","",-20,0);
		double norm_per_bin = fit->Parameter(0);
			// Given our momentum max and min, solve for bins in ToF/m
		double beta_min = 1./sqrt(1.+ pow(mN/NCUT_Pn_min,2));
		double beta_max = 1./sqrt(1.+ pow(mN/NCUT_Pn_max,2));
			// max beta = min ToF and vice versa
		double TofpM_max = 1./(cAir*beta_min)*100;
		double TofpM_min = 1./(cAir*beta_max)*100;
		int TofpM_min_bin = hToF_bac->FindBin( TofpM_min );
		int TofpM_max_bin = hToF_bac->FindBin( TofpM_max );
		int nBins = (TofpM_max_bin - TofpM_min_bin); 	
		double background_counts = norm_per_bin * nBins;
	
		bacnorm.SetXYZ(background_counts,0,0);
	}


	// Create the good event list from our cuts defined above
	inTree->Draw(">>goodEvents",cut);
	TEventList * goodEvents = (TEventList*) gDirectory->Get("goodEvents");
	int nEvents = goodEvents->GetN();

	// Loop over all the good events and write the output tree
	for( int ev = 0 ; ev < nEvents ; ev++ ){
		if( ev % 100000 == 0 ) cout << "\ton event " << ev << "\n";

		int entry = goodEvents->GetEntry(ev);
		inTree->GetEntry(entry);

		outTree->Fill();

	} // end loop over events


	// Write and close
	outFile->cd();
	outTree->Write();
	bacnorm.Write("bacnorm");	
	hToF_bac->Write();
	outFile->Close();
	return 0;
}
