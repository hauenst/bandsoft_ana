void phie_inc(TString inDat, TString inSim){

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

	inTreeDat->Add(inDat);

	// Define histograms we want to plot:
	TH1D * phie_dat;
	TH1D * phie_sim;
	phie_dat = new TH1D("phie_dat","",180,-180,180);
	phie_sim = new TH1D("phie_sim","",180,-180,180);


	// Draw the full electron phi distribution
	TCanvas * c_phie = new TCanvas("c_phie","",800,600);
	double sim_scaling = 0;
	c_phie->Divide(1,2);
	for( int i = 0 ; i < 1 ; i++){

		c_phie->cd(i+1);
		inTreeDat->Draw("eHit->getPhi()*180./TMath::Pi() >> phie_dat");
		inTreeSim->Draw("eHit->getPhi()*180./TMath::Pi() >> phie_sim");


		// Simulation scaling only from no pT cut distribution (i.e. from full distribution)
		double full_simnorm = (double)phie_dat->Integral() / phie_sim->Integral();
		if( i == 0 ) sim_scaling = full_simnorm;
		phie_sim->Scale( sim_scaling );


		phie_sim->SetTitle(Form("C_{sim} = %f",sim_scaling));
		label1D(phie_dat,phie_sim,"#phi_{e}","Counts");

		c_phie->cd(2+i);
		label1D_ratio(phie_dat,phie_sim,"#phi_{e}","Data/Sim",0.8,1.2);
	}


	c_phie->SaveAs("full_phie_inc.pdf");
	phie_dat->SaveAs("data_phie_inc.root");
	phie_sim->SaveAs("sim_phie_inc.root");
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
