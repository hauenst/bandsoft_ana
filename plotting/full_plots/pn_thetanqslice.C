void pn_thetanqslice(TString inDat, TString inBac, TString inSim){

	TCut pNcut = "tag[nleadindex]->getMomentumN().Mag() < 1 && tag[nleadindex]->getMomentumN().Mag() > 0.25";
	TString pTtitle = "Full pT";
	TCut pTcut = "tag[nleadindex]->getPt().Mag() > 0.0 && tag[nleadindex]->getPt().Mag() < 0.2";

	// Define some function used
	void label1D(TH1D* data, TH1D* sim, TString xlabel, TString ylabel);
	void label1D_ratio(TH1D* data, TH1D* sim, TString xlabel, TString ylabel, double ymin , double ymax );

	// Load TFiles
	TFile * inFileDat = new TFile(inDat);
	TFile * inFileBac = new TFile(inBac);
	TFile * inFileSim = new TFile(inSim);

	// Get the TTrees
	TTree * inTreeDat = (TTree*) inFileDat->Get("tagged");
	TTree * inTreeBac = (TTree*) inFileBac->Get("tagged");
	TTree * inTreeSim = (TTree*) inFileSim->Get("tagged");

	// Get and set the background normalization
	TVector3 * datnorm = (TVector3*)inFileDat->Get("bacnorm");
	TVector3 * bacnorm = (TVector3*)inFileBac->Get("bacnorm");
	inTreeBac->SetWeight( datnorm->Z() / bacnorm->X() );

	// Define histograms we want to plot:
	TH1D ** pn_dat = new TH1D*[3];
	TH1D ** pn_bac = new TH1D*[3];
	TH1D ** pn_sim = new TH1D*[3];
	for(int i = 0 ; i < 3 ; i++){
		pn_dat[i] = new TH1D(Form("pn_dat_%i",i),"",40,0.2,0.6);
		pn_bac[i] = new TH1D(Form("pn_bac_%i",i),"",40,0.2,0.6);
		pn_sim[i] = new TH1D(Form("pn_sim_%i",i),"",40,0.2,0.6);
	}

	// Draw the full pn distribution
	TCanvas * c_pn = new TCanvas("c_pn","",800,600);
	double sim_scaling = 0;
	c_pn->Divide(3,2);
	for( int i = 0 ; i < 3 ; i++){
		TCut thetaNQcut = "";
		TString thetaNQtitle = "";
		if( i == 0 ){
			thetaNQcut = "tag[nleadindex]->getThetaNQ() * 180./TMath::Pi() < 155 && tag[nleadindex]->getThetaNQ() * 180./TMath::Pi() > 145";
			thetaNQtitle = "145-155";
		}
		if( i == 1 ){
			thetaNQcut = "tag[nleadindex]->getThetaNQ() * 180./TMath::Pi() < 165 && tag[nleadindex]->getThetaNQ() * 180./TMath::Pi() > 155";
			thetaNQtitle = "155-165";
		}
		if( i == 2 ){
			thetaNQcut = "tag[nleadindex]->getThetaNQ() * 180./TMath::Pi() < 175 && tag[nleadindex]->getThetaNQ() * 180./TMath::Pi() > 165";
			thetaNQtitle = "165-175";
		}

		c_pn->cd(i+1);
		inTreeDat->Draw(Form("tag[nleadindex]->getMomentumN().Mag() >> pn_dat_%i",i),pNcut && thetaNQcut && pTcut);
		inTreeBac->Draw(Form("tag[nleadindex]->getMomentumN().Mag() >> pn_bac_%i",i),pNcut && thetaNQcut && pTcut);
		inTreeSim->Draw(Form("tag[nleadindex]->getMomentumN().Mag() >> pn_sim_%i",i),pNcut && thetaNQcut && pTcut);

		if( pn_dat[i]->Integral() < 1 || pn_bac[i]->Integral() < 1 || pn_sim[i]->Integral() < 1 ) continue;

		// Background subraction
		pn_dat[i]->Add(pn_bac[i],-1);

		// Simulation scaling only from no pT cut distribution (i.e. from full distribution)
		double full_simnorm = (double)pn_dat[i]->Integral() / pn_sim[i]->Integral();
		pn_sim[i]->Scale( full_simnorm );
		
		
		pn_sim[i]->SetTitle(pTtitle+" "+thetaNQtitle+Form(", C_{sim} = %f, ",full_simnorm));
		label1D(pn_dat[i],pn_sim[i],"|p_{n}| [GeV/c]","Counts");

		c_pn->cd(4+i);
		label1D_ratio(pn_dat[i],pn_sim[i],"|p_{n}| [GeV/c]","Data/Sim",0,2);
	}


	c_pn->SaveAs("full_pn_thetanqslice_fullpT.pdf");

	return;
}

void label1D(TH1D* data, TH1D* sim, TString xlabel, TString ylabel){
	data->SetLineColor(4);
	data->SetMarkerColor(4);
	data->SetMarkerStyle(8);
	data->SetMarkerSize(1);
	data->SetStats(0);

	sim->SetLineColor(2);
	sim->SetLineWidth(1);
	sim->SetStats(0);

	sim->Draw("hist");
	data->Draw("p,same");

	double max1 = data->GetMaximum()*1.1;
	double max2 = sim->GetMaximum()*1.1;
	sim->GetYaxis()->SetRangeUser(0,max(max1,max2));
	
	sim->GetXaxis()->SetTitle(xlabel);
	sim->GetYaxis()->SetTitle(ylabel);

	TLegend * legend = new TLegend(0.7,0.8,0.9,0.9);
	//legend->AddEntry(data,"Radiation On","f");
	//legend->AddEntry(sim,"Radiation Off","f");
	legend->AddEntry(data,"Data","f");
	legend->AddEntry(sim,"Sim","f");
	legend->Draw();

	return;
}

void label1D_ratio(TH1D* data, TH1D* sim, TString xlabel, TString ylabel, double ymin , double ymax ){
	gStyle->SetOptFit(1);
	
	TH1D * data_copy = (TH1D*) data->Clone();
	TH1D * sim_copy = (TH1D*) sim->Clone();
	
	data_copy->SetLineColor(1);
	data_copy->SetLineWidth(3);
	//data_copy->SetStats(0);

	sim_copy->SetLineColor(9);
	sim_copy->SetLineWidth(3);
	//sim_copy->SetStats(0);
	//sim_copy->Scale(data_copy->Integral() / sim_copy->Integral() );

	data_copy->Divide(sim_copy);
	data_copy->SetTitle( sim_copy->GetTitle() );
	data_copy->Draw("ep");
	TLine* line = new TLine(data_copy->GetXaxis()->GetBinCenter(1), 1., data_copy->GetXaxis()->GetBinCenter(data_copy->GetXaxis()->GetNbins()), 1.);
	line->SetLineWidth(3);
	line->SetLineColor(2);
	line->Draw("same");

	data_copy->GetYaxis()->SetRangeUser(ymin,ymax);
	
	data_copy->GetXaxis()->SetTitle(xlabel);
	data_copy->GetYaxis()->SetTitle(ylabel);

	return;
}
