void xy_tofbins(TString inDat, TString inBac, TString inSim){

	// Define some function used
	void label1D(TH1D* data, TH1D* sim, TString xlabel, TString ylabel);

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
	TH2D * xy_tof_below_dat 	= new TH2D("xy_tof_below_dat","xy_tof 26.5-30.5 (data) "	,40,-50,110,105,-210,210);
	TH2D * xy_tof_below2_dat 	= new TH2D("xy_tof_below2_dat","xy_tof 22.5-30.5 (data) "	,40,-50,110,105,-210,210);
	TH2D * xy_tof_peak_dat 	= new TH2D("xy_tof_peak_dat","xy_tof 30.5-34.5 (data) "	,40,-50,110,105,-210,210);
	TH2D * xy_tof_above_dat 	= new TH2D("xy_tof_above_dat","xy_tof 34.5-38.5 (data) "	,40,-50,110,105,-210,210);
	TH2D * xy_tof_above2_dat 	= new TH2D("xy_tof_above2_dat","xy_tof 34.5-42.5 (data) "	,40,-50,110,105,-210,210);
	TH1D * tof_data	= new TH1D("tof_data","tof_data",60,0,60);
	TH1D * tof_bac 	= new TH1D("tof_bac","tof_bac"	,60,0,60);
	TH1D * tof_sim 	= new TH1D("tof_sim","tof_sim"	,60,0,60);


	// Draw the full tof distribution
	TCanvas * c_tof_plot = new TCanvas("c_tof_plot","",800,600);
	c_tof_plot->Divide(2,2);
	c_tof_plot->cd(1);
	inTreeDat->Draw("nHits[nleadindex]->getTof() >> tofpm_dat");
	c_tof_plot->cd(2);
	inTreeBac->Draw("nHits[nleadindex]->getTof() >> tofpm_bac");
	c_tof_plot->cd(3);
	inTreeSim->Draw("nHits[nleadindex]->getTof() >> tofpm_sim",	"nHits[nleadindex]->getEdep()/1E4 > 5");

	//label1D(tofpm_dat,tofpm_bac,"ToF [ns]","Counts");
	//c_tof_plot->SaveAs("full_tof_prebs.pdf");

	// Draw the full tofpm distribution
	TCanvas * c_xy_plot = new TCanvas("c_xy_plot","",800,600);
	c_xy_plot->Divide(2,3);

	c_xy_plot->cd(1);
	inTreeDat->Draw("nHits[nleadindex]->getX():nHits[nleadindex]->getY() >> xy_tof_below_dat","nHits[nleadindex]->getTof() > 26.5 && nHits[nleadindex]->getTof() <= 30.5");
	c_xy_plot->cd(2);
	inTreeDat->Draw("nHits[nleadindex]->getX():nHits[nleadindex]->getY() >> xy_tof_below2_dat","nHits[nleadindex]->getTof() > 22.5 && nHits[nleadindex]->getTof() <= 30.5");
	c_xy_plot->cd(3);
	inTreeDat->Draw("nHits[nleadindex]->getX():nHits[nleadindex]->getY() >> xy_tof_peak_dat","nHits[nleadindex]->getTof() > 30.5 && nHits[nleadindex]->getTof() <= 34.5");
	c_xy_plot->cd(4);
	inTreeDat->Draw("nHits[nleadindex]->getX():nHits[nleadindex]->getY() >> xy_tof_above_dat","nHits[nleadindex]->getTof() > 34.5 && nHits[nleadindex]->getTof() <= 38.5");
	c_xy_plot->cd(5);
	inTreeDat->Draw("nHits[nleadindex]->getX():nHits[nleadindex]->getY() >> xy_tof_above2_dat","nHits[nleadindex]->getTof() > 34.5 && nHits[nleadindex]->getTof() <= 42.5");


	//c_xy_plot->SaveAs("xy_distribution_data_tofcuts.pdf");




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
	legend->AddEntry(sim,"Background","f");
	legend->Draw();

	return;
}
