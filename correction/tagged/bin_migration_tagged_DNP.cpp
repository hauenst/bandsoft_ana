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
#include "bandhit.h"
#include "taghit.h"

#include "kinematic_cuts.h"
#include "bin_edges.h"

using std::cerr;
using std::cout;
void calculate_AsPt( double & As, double & Pt , double & Xp, const double Ebeam, genpart * const electron, genpart * const neutron );
void setError( double * err , TH1D * rec , TH1D * gen );

int main( int argc, char** argv){

	if( argc != 2 ){
		cerr << "Incorrect number of arguments. Please use:\n";
		cerr << "./code [Sim File]\n";
		return -1;
	}

	TChain * inTree  = new TChain("tagged");
	inTree->Add(argv[1]);

//	TFile inFile(argv[1]);
//	TTree * inTree = (TTree*) inFile.Get("tagged");

	TClonesArray* gen_particles 	= new TClonesArray("genpart");
	clashit * rec_electron 		= new clashit;
	TClonesArray* rec_tagged 	= new TClonesArray("taghit");
	TClonesArray* rec_neutrons 	= new TClonesArray("bandhit");
	int 	genMult 	= 0;
	int	nleadindex 	= 0;
	double	Ebeam		= 0;
	inTree->SetBranchAddress("mcParts"		, &gen_particles		);
	inTree->SetBranchAddress("genMult"		, &genMult			);
	inTree->SetBranchAddress("eHit_smeared"			, &rec_electron			);
	inTree->SetBranchAddress("tag_smeared"			, &rec_tagged			);
	inTree->SetBranchAddress("nHits"		, &rec_neutrons			);
	inTree->SetBranchAddress("nleadindex"		, &nleadindex			);
	inTree->SetBranchAddress("Ebeam"		, &Ebeam			);

	TFile * outFile = new TFile("migcorrection_tagged.root","RECREATE");

	TH1D *h1_neutron_mom_rec = new TH1D("h1_neutron_mom_rec","Reco Neutron Momentum",50,0,1);

	TH1D *xp_gen_cutongen = new TH1D("xp_gen_cutongen","xp_gen_cutongen",bins_Xp,Xp_min,Xp_max);
	TH1D *xp_gen_cutonrec	= new TH1D("xp_gen_cutonrec","xp_gen_cutonrec",bins_Xp,Xp_min,Xp_max);
	TH1D *xp_rec_cutongen = new TH1D("xp_rec_cutongen","xp_rec_cutongen",bins_Xp,Xp_min,Xp_max);
	TH1D *xp_rec_cutonrec	= new TH1D("xp_rec_cutonrec","xp_rec_cutonrec",bins_Xp,Xp_min,Xp_max);

	//Event loop
	for( int event = 0 ; event < inTree->GetEntries() ; ++event ){
		gen_particles	->Clear();
		rec_neutrons	->Clear();
		rec_electron	->Clear();
		rec_tagged	->Clear();
		genMult 	= 0;
		nleadindex	= 0;
		Ebeam 		= 0;
		//if(event > 1000) break;
		inTree->GetEntry(event);

		//Get neutron hit for extra cuts
		bandhit * neutron_event = (bandhit*)rec_neutrons->At(nleadindex);
		//Skip if top row
		if ( neutron_event->getSector()==1 && neutron_event->getComponent()==1 ) continue;
		//skip if edep below 10
		if ( neutron_event->getEdep()<10) continue;

		// Get the reconstructed variables
		taghit * rec_tag	= (taghit*)  rec_tagged->At(nleadindex);
		if( rec_tag->getMomentumN().Mag() < 0.25 ) continue;
		h1_neutron_mom_rec->Fill(rec_tag->getMomentumN().Mag());


		double rec_As		= rec_tag->getAs();
		double rec_Pt 	= rec_tag->getPt().Mag();
		double rec_Q2		= rec_electron->getQ2();
		double rec_Xb		= rec_electron->getXb();
		double rec_Xp		= rec_tag->getXp_WP();

		// Form As, ThetaNQ, W, X with generated:
		genpart * gen_electron 	= (genpart*) gen_particles->At(0);
		genpart * gen_neutron 	= (genpart*) gen_particles->At(1);
		double	gen_Q2		= gen_electron->getQ2();
		double 	gen_Xb		= gen_electron->getXb();

		double gen_As, gen_Pt, gen_Xp;
		calculate_AsPt( gen_As, gen_Pt, gen_Xp, Ebeam, gen_electron, gen_neutron );

		//Cut on generated values for Q2, pT and alphaS
		if (gen_Q2 > ECUT_Q2_min && gen_Q2 < ECUT_Q2_max &&
			gen_Pt < 0.1 && gen_As > 1.3 && gen_As < 1.4)  {

				xp_gen_cutongen->Fill(gen_Xp);
				xp_rec_cutongen->Fill(rec_Xp);
			}
		//Cut on generated values for Q2, pT and alphaS
		if (rec_Q2 > ECUT_Q2_min && rec_Q2 < ECUT_Q2_max &&
				rec_Pt < 0.1 && rec_As > 1.3 && rec_As < 1.4)  {

					xp_gen_cutonrec->Fill(gen_Xp);
					xp_rec_cutonrec->Fill(rec_Xp);
				}


	}//end event loop

	outFile->Close();

	inFile.Close();
	return 1;
}


void calculate_AsPt( double & As_in, double & Pt_in , double & Xp_in, const double Ebeam, genpart * const electron, genpart * const neutron ){

	TVector3 	beamVec(0,0,Ebeam);
	TVector3	eVec;
	eVec.SetMagThetaPhi( electron->getMomentum(), electron->getTheta(), electron->getPhi() );
	TVector3	qVec; qVec = beamVec - eVec;
	TVector3	nVec;
	nVec.SetMagThetaPhi( neutron->getMomentum(), neutron->getTheta(), neutron->getPhi() );
	nVec.Unit();


	TVector3 norm_scatter = qVec.Cross( beamVec );
	norm_scatter 	= norm_scatter.Unit();

	TVector3 norm_reaction = qVec.Cross( nVec );
	norm_reaction 	= norm_reaction.Unit();

	double phi_nq 		= norm_scatter.Angle( norm_reaction );
	double theta_nq 	= nVec.Angle( qVec );
	double gen_CosThetaNQ 	= cos(theta_nq);

	TVector3 direction = norm_scatter.Cross(norm_reaction);
	if( direction.Z() > 0 ){ // this means the phi_rq should be between 0 and pi
	}
	else if( direction.Z() < 0 ){ // this means the phi_rq should be between -pi and 0
		phi_nq *= (-1);
	}

	nVec.SetMagThetaPhi( neutron->getMomentum(), neutron->getTheta(), neutron->getPhi() );
	double p_n = neutron->getMomentum();

	double E_n 	= sqrt( mN*mN + p_n*p_n );
	double W_primeSq = mD*mD - electron->getQ2() + mN*mN + 2.*mD*(electron->getOmega()-E_n) - 2.*electron->getOmega()*E_n + 2.*electron->getQ()*p_n*cos(theta_nq);
	double Wp = sqrt(W_primeSq);
	double Xp = electron->getQ2()/(2.*( electron->getOmega()*(mD-E_n) + p_n*electron->getQ()*gen_CosThetaNQ));
	double Xp2 = electron->getQ2()/(W_primeSq - mN*mN + electron->getQ2());

	TVector3 Pt_vec;
	TVector3 pN_par_q = nVec.Dot(qVec) / (qVec.Mag2()) * qVec;

	As_in = (E_n - p_n*gen_CosThetaNQ)/mN;
	Pt_vec = nVec - pN_par_q;
	Pt_in = Pt_vec.Mag();
  Xp_in = Xp2;
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
