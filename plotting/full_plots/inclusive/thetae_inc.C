void thetae_inc(TString inDat, TString inSim){

	// Define some function used
	void label1D(TH1D* data, TH1D* sim, TString xlabel, TString ylabel);
	void label1D_ratio(TH1D* data, TH1D* sim, TString xlabel, TString ylabel, double ymin , double ymax );

	// Load TFiles
	//TFile * inFileDat = new TFile(inDat);
	TFile * inFileSim = new TFile(inSim);

	// Get the TTrees
	//TTree * inTreeDat = (TTree*) inFileDat->Get("electrons");
	TTree * inTreeSim = (TTree*) inFileSim->Get("electrons");
	TChain * inTreeDat = new TChain("electrons");

	inTreeDat->AddFile(inDat);

	// Define histograms we want to plot:
	TH1D ** thetae_dat = new TH1D*[1];
	TH1D ** thetae_sim = new TH1D*[1];
	for(int i = 0 ; i < 1 ; i++){
		thetae_dat[i] = new TH1D(Form("thetae_dat_%i",i),"",23,7,30);
		thetae_sim[i] = new TH1D(Form("thetae_sim_%i",i),"",23,7,30);
	}

	// Draw the full thetae distribution
	TCanvas * c_thetae = new TCanvas("c_thetae","",800,600);
	double sim_scaling = 0;
	c_thetae->Divide(1,2);
	for( int i = 0 ; i < 1 ; i++){

		c_thetae->cd(i+1);
		inTreeDat->Draw(Form("eHit->getTheta()*180./TMath::Pi() >> thetae_dat_%i",i));
		inTreeSim->Draw(Form("eHit->getTheta()*180./TMath::Pi() >> thetae_sim_%i",i));


		// Simulation scaling only from no pT cut distribution (i.e. from full distribution)
		double full_simnorm = (double)thetae_dat[0]->Integral() / thetae_sim[0]->Integral();
		if( i == 0 ) sim_scaling = full_simnorm;
		thetae_sim[i]->Scale( sim_scaling );


		thetae_sim[i]->SetTitle(Form("C_{sim} = %f",sim_scaling));
		label1D(thetae_dat[i],thetae_sim[i],"#theta_{e}","Counts");

		c_thetae->cd(2+i);
		label1D_ratio(thetae_dat[i],thetae_sim[i],"#theta_{e}","Data/Sim",0.8,1.2);
	}


	c_thetae->SaveAs("full_thetae-inc.pdf");

	return;
}

void label1D(TH1D* data, TH1D* sim, TString xlabel, TString ylabel){
	data->SetLineColor(4);
	data->SetMarkerColor(4);
	data->SetMarkerStyle(8);
	data->SetMarkerSize(1);
	data->SetStats(0);
	data->GetYaxis()->SetTitleSize(0.05);
  data->GetYaxis()->SetTitleOffset(0.95);
  data->GetYaxis()->SetLabelSize(0.05);
  data->GetXaxis()->SetTitleSize(0.05);
  data->GetXaxis()->SetLabelSize(0.05);

	sim->SetLineColor(2);
	sim->SetLineWidth(1);
	sim->SetStats(0);
	sim->GetYaxis()->SetTitleSize(0.05);
	sim->GetYaxis()->SetTitleOffset(0.95);
	sim->GetYaxis()->SetLabelSize(0.05);
	sim->GetXaxis()->SetTitleSize(0.05);
	sim->GetXaxis()->SetLabelSize(0.05);

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
	data_copy->GetYaxis()->SetTitleSize(0.05);
	data_copy->GetYaxis()->SetTitleOffset(0.95);
	data_copy->GetYaxis()->SetLabelSize(0.05);
	data_copy->GetXaxis()->SetTitleSize(0.05);
	data_copy->GetXaxis()->SetLabelSize(0.05);

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
