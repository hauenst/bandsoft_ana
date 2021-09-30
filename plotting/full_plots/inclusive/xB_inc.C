void xB_inc(TString inDat, TString inSim, int smeared = 0){

	//Cuts for MC smeared values
	const double 	ECUT_pE_min = 3;
	const double 	ECUT_pE_max = 10.6;
	const double 	ECUT_Q2_min = 2;
	const double 	ECUT_Q2_max = 10;
	const double 	ECUT_W2_min = 2*2;
	TCut eMom_smeared	= Form("eHit_smeared->getMomentum() > %f && eHit_smeared->getMomentum() < %f",	ECUT_pE_min,		ECUT_pE_max);
	TCut eQ2_smeared	= Form("eHit_smeared->getQ2() > %f && eHit_smeared->getQ2() < %f",		ECUT_Q2_min,		ECUT_Q2_max);
	TCut eW_smeared		= Form("eHit_smeared->getW2() > %f",					ECUT_W2_min);
	TCut inclusive_smeared	= eMom_smeared && eQ2_smeared && eW_smeared;
	TCut extraxB = "eHit->getXb() < 0.45";
	TCut extraxB_smeared = "eHit_smeared->getXb() < 0.45";

	bool plotsmeared = false;
	if (smeared!=0) plotsmeared = true;

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
	TH1D ** xB_dat = new TH1D*[2];
	TH1D ** xB_sim = new TH1D*[2];
	for(int i = 0 ; i < 2 ; i++){
		xB_dat[i] = new TH1D(Form("xB_dat_%i",i),"",30,0.1,0.7);
		xB_sim[i] = new TH1D(Form("xB_sim_%i",i),"",30,0.1,0.7);
	}

	// Draw the full xB distribution
	TCanvas * c_xB = new TCanvas("c_xB","",800,600);
	double sim_scaling = 0;
	c_xB->Divide(1,2);
	for( int i = 0 ; i < 1 ; i++){

		c_xB->cd(i+1);
		inTreeDat->Draw(Form("eHit->getXb() >> xB_dat_%i",i));
		inTreeDat->Draw(Form("eHit->getXb() >> xB_dat_%i",i+1),extraxB);
		if (plotsmeared)
		{
			inTreeSim->Draw(Form("eHit_smeared->getXb() >> xB_sim_%i",i),inclusive_smeared);
			inTreeSim->Draw(Form("eHit_smeared->getXb() >> xB_sim_%i",i+1),inclusive_smeared && extraxB && extraxB_smeared);

		}
		else {
			inTreeSim->Draw(Form("eHit->getXb() >> xB_sim_%i",i));
			inTreeSim->Draw(Form("eHit->getXb() >> xB_sim_%i",i+1),extraxB);
		}


		// Simulation scaling only from no pT cut distribution (i.e. from full distribution)
		double full_simnorm = (double)xB_dat[0]->Integral() / xB_sim[0]->Integral();
		if( i == 0 ) sim_scaling = full_simnorm;
		xB_sim[i]->Scale( sim_scaling );
		xB_sim[i+1]->Scale( sim_scaling );

		xB_sim[i]->SetTitle(Form("C_{sim} = %f",sim_scaling));
		label1D(xB_dat[i],xB_sim[i],"x_{B}","Counts");

		c_xB->cd(2+i);
		label1D_ratio(xB_dat[i],xB_sim[i],"x_{B}","Data/Sim",0.8,1.2);
	}

	c_xB->SaveAs("full_xB-inc.pdf");
	xB_dat[0]->SaveAs("data_xB_inc.root");
	xB_sim[0]->SaveAs("sim_xB_inc.root");
	xB_dat[1]->SaveAs("data_xB_inc_xbcut.root");
	xB_sim[1]->SaveAs("sim_xB_inc_xbcut.root");
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
