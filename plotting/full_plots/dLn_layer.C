void dLn_layer(TString inDat, TString inBac, TString inSim, int layer){

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
	inTreeBac->SetWeight( datnorm->X() / bacnorm->X() );
	

	// Define histograms we want to plot:
	TH1D ** dLn_dat = new TH1D*[3];
	TH1D ** dLn_bac = new TH1D*[3];
	TH1D ** dLn_sim = new TH1D*[3];
	for(int i = 0 ; i < 3 ; i++){
		dLn_dat[i] = new TH1D(Form("dLn_dat_%i",i),"",40,250,330);
		dLn_bac[i] = new TH1D(Form("dLn_bac_%i",i),"",40,250,330);
		dLn_sim[i] = new TH1D(Form("dLn_sim_%i",i),"",40,250,330);
	}

	// Draw the full dLn distribution
	TCanvas * c_dLn = new TCanvas("c_dLn","",1200,900);
	//Define the cut on layer number
	TCut layer_cut = Form("nHits[nleadindex]->getLayer() == %d", layer);
	
	double sim_scaling = 0;
	c_dLn->Divide(3,2);
	for( int i = 0 ; i < 3 ; i++){
		TCut pTcut = "";
		TString pTtitle = "Full pT";
		if( i == 1 ){
			pTcut = "tag[nleadindex]->getPt().Mag() < 0.1";
			pTtitle = "Low pT";
		}
		if( i == 2 ){
			pTcut = "tag[nleadindex]->getPt().Mag() >= 0.1";
			pTtitle = "High pT";
		}

		c_dLn->cd(i+1);
		inTreeDat->Draw(Form("sqrt( pow(nHits[nleadindex]->getX(),2) + pow(nHits[nleadindex]->getY(),2) + pow(nHits[nleadindex]->getZ(),2) )  >> dLn_dat_%i",i),"tag[nleadindex]->getMomentumN().Mag() > 0." && pTcut && layer_cut);
		inTreeBac->Draw(Form("sqrt( pow(nHits[nleadindex]->getX(),2) + pow(nHits[nleadindex]->getY(),2) + pow(nHits[nleadindex]->getZ(),2) )  >> dLn_bac_%i",i),"tag[nleadindex]->getMomentumN().Mag() > 0." && pTcut && layer_cut);
		inTreeSim->Draw(Form("nHits[nleadindex]->getDL().Mag() >> dLn_sim_%i",i),"tag[nleadindex]->getMomentumN().Mag() > 0." && pTcut && layer_cut);

		// Background subraction
		dLn_dat[i]->Add(dLn_bac[i],-1);

		// Simulation scaling only from no pT cut distribution (i.e. from full distribution)
		double full_simnorm = (double)dLn_dat[0]->Integral() / dLn_sim[0]->Integral();
	
		if( i == 0 ) sim_scaling = full_simnorm;
		dLn_sim[i]->Scale( sim_scaling );
		
		double sec_simnorm = (double)dLn_dat[i]->Integral() / dLn_sim[i]->Integral();
		
		dLn_sim[i]->SetTitle(pTtitle+Form(", C_{full} = %1.3f, layer=%d, C=%1.3f  ",sim_scaling, layer, sec_simnorm));
		label1D(dLn_dat[i],dLn_sim[i],"|dL_{n}| [cm]","Counts");

		c_dLn->cd(4+i);
		label1D_ratio(dLn_dat[i],dLn_sim[i],"|dL_{n}| [cm]","Data/Sim",0,2);
	}


	c_dLn->SaveAs(Form("full_dLn_layer_%d.pdf", layer));

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
