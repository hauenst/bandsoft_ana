#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLine.h"

#include "genpart.h"
#include "clashit.h"

#include "bin_edges.h"

using std::cerr;
using std::cout;
void fillHist( double Q2, double Xb, TH1D** hist );
void setError( double * err , TH1D * rec , TH1D * gen );

int main( int argc, char** argv){

	if( argc != 2 ){
		cerr << "Incorrect number of arguments. Please use:\n";
		cerr << "./code [Sim File]\n";
		return -1;
	}

	TFile inFile(argv[1]);
	TTree * inTree = (TTree*) inFile.Get("electrons");

	TClonesArray* gen_particles 	= new TClonesArray("genpart");
	clashit * rec_electron 		= new clashit;
	int 	genMult 	= 0;
	double	Ebeam		= 0;
	inTree->SetBranchAddress("mcParts"		, &gen_particles		);
	inTree->SetBranchAddress("genMult"		, &genMult			);
	inTree->SetBranchAddress("eHit_smeared"			, &rec_electron			);
	inTree->SetBranchAddress("Ebeam"		, &Ebeam			);

	TFile * outFile = new TFile("migcorrection_inclusive.root","RECREATE");
	TH1D ** h2_gen_xb 	= new TH1D*[bins_Q2];
	TH1D ** h2_rec_xb	= new TH1D*[bins_Q2];
	for( int i = 0 ; i < bins_Q2 ; ++i ){ // bins in Q2
		double minxb = Xb_min[i];
		double maxxb = minxb + bins_Xb*Xb_step;
		h2_gen_xb[i] = new TH1D(Form("gen_Xb_Q2_%i",i),Form("gen_Xb_Q2_%i",i),bins_Xb,minxb,maxxb); 
		h2_rec_xb[i] = new TH1D(Form("migcorr_Xb_Q2_%i",i),Form("migcorr_Xb_Q2_%i",i),bins_Xb,minxb,maxxb); 
	}

	for( int event = 0 ; event < inTree->GetEntries() ; ++event ){
		gen_particles	->Clear();
		rec_electron	->Clear();
		genMult 	= 0;
		Ebeam 		= 0;
		inTree->GetEntry(event);

		// Get the reconstructed variables
		double rec_Q2		= rec_electron->getQ2();
		double rec_Xb		= rec_electron->getXb();

		// Form As, ThetaNQ, W, X with generated:
		genpart * gen_electron 	= (genpart*) gen_particles->At(0);
		double	gen_Q2		= gen_electron->getQ2();
		double 	gen_Xb		= gen_electron->getXb();

		// sort the generated values:
		fillHist( gen_Q2, gen_Xb, h2_gen_xb );

		// sort the reconstructed values:
		fillHist( rec_Q2, rec_Xb, h2_rec_xb );

	}

	TCanvas ** c_Q2 = new TCanvas*[bins_Q2];
	for( int i = 0 ; i < bins_Q2 ; ++i ){ // loop over Q2 bins
		c_Q2[i] = new TCanvas(Form("c_Q2_%i",i),"",1200,1200);
		
		if( h2_gen_xb[i]->Integral() < 1 || h2_rec_xb[i]->Integral() < 1 ) continue;
		double * errors = new double[bins_Xb];
		setError( errors , h2_rec_xb[i] , h2_gen_xb[i] );
		h2_rec_xb[i]->Divide( h2_gen_xb[i] );

		for( int bin = 1 ; bin < h2_rec_xb[i]->GetXaxis()->GetNbins(); ++bin ){
			h2_rec_xb[i]->SetBinError( bin , errors[bin-1] );
		}

		//c_Q2[i]->cd( (k*2)+1 + j );

		h2_rec_xb[i]->SetLineWidth(3);
		h2_rec_xb[i]->SetMarkerColor(4);
		h2_rec_xb[i]->SetMaximum(2);
		h2_rec_xb[i]->SetMinimum(0);
		h2_rec_xb[i]->Draw("*P");
		TLine * hline = new TLine(Xb_min[i],1,Xb_min[i]+bins_Xb*Xb_step,1);
		hline->SetLineWidth(2);
		hline->SetLineColor(1);
		hline->SetLineStyle(2);
		hline->Draw("SAME");

		//h2_gen_xb[i]->Write();
		h2_rec_xb[i]->Write();

		delete[] errors;

		c_Q2[i]->Print(Form("binmigration_inclusive_Q2_%i.pdf",i));
	}
	outFile->Close();

	inFile.Close();
	return 1;
}



void fillHist( double Q2, double Xb, TH1D** hist ){

	// If it's larger than max Q2 or smaller than min Q2, return
	if( Q2 > Q2Bins[bins_Q2] 	) return;
	if( Q2 < Q2Bins[0] 		) return;

	// Need to figure out which bin of Q2, Pt, Xb (then fill as As):
	int this_bin_q2 = -1;
	
	for( int q2_bin = bins_Q2-1 ; q2_bin >= 0; --q2_bin ){
		if( Q2 > Q2Bins[q2_bin] ) this_bin_q2 = q2_bin;
		if( this_bin_q2 != -1 ) break;
	}

	if( Xb < Xb_min[this_bin_q2]			) return;
	if( Xb > Xb_min[this_bin_q2]+Xb_step*6		) return;

	// Safety clauses
	if( this_bin_q2 == -1 ){ cerr << "how\n"; return; }
	if( this_bin_q2 > bins_Q2-1 ){ cerr << "how\n"; return; }

	hist[this_bin_q2]->Fill( Xb );

}

void setError( double * err , TH1D * rec , TH1D * gen ){
	
	for( int bin = 1 ; bin < rec->GetXaxis()->GetNbins(); ++bin ){

		double r = rec->GetBinContent(bin);
		double g = gen->GetBinContent(bin);

		double e = sqrt( r/(g*g) + r*r/(g*g*g) );
		cout << bin << " " << r << " " << g << " " << r/g << " " << e << "\n";
		if( e!=e ) e = 0;
		err[bin-1] = e;
		cout << err[bin-1] << "\n\n";
	}


	return;
}
