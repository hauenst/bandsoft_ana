void xp_mconly_asbins(TString inSim1, TString inSim2){

	//Full inSim1, Asymptotic inSim2

	//Cuts for energy, top bars, and as bins
	TCut pNcut = "tag[nleadindex]->getMomentumN().Mag() < 1.0 && tag[nleadindex]->getMomentumN().Mag() > 0.25 && !(nHits[nleadindex]->getSector()==1 && nHits[nleadindex]->getComponent()==1) && nHits[nleadindex]->getEdep()>10";
	TCut pNcut_sim = "tag_smeared[nleadindex]->getMomentumN().Mag() < 1.0 && tag_smeared[nleadindex]->getMomentumN().Mag() > 0.25 && !(nHits[nleadindex]->getSector()==1 && nHits[nleadindex]->getComponent()==1) && nHits[nleadindex]->getEdep()>10";
	TCut pTcut_sim = {"tag_smeared[nleadindex]->getPt().Mag()< 0.1"};
	TCut aScut_sim[3] = {"tag_smeared[nleadindex]->getAs() >= 1.3 && tag_smeared[nleadindex]->getAs() < 1.4",
				     			"tag_smeared[nleadindex]->getAs() >= 1.4 && tag_smeared[nleadindex]->getAs() < 1.5",
							"tag_smeared[nleadindex]->getAs() >= 1.5 && tag_smeared[nleadindex]->getAs() < 1.6"};
	double norm_xpvalue[3] = {0.3,0.4,0.4};

	cerr << "Files used: " << inSim1 << " " << inSim2 << "\n";

	// Define some function used
	void label1D(TH1D* data, TH1D* sim, TString xlabel, TString ylabel);
	void label1D_ratio(TH1D* data, TH1D* sim, TH1D* ratiohist, TString xlabel, TString ylabel, double ymin , double ymax );
	void label1D_ratio_norm(TH1D* data, TH1D* sim, TH1D* ratiohist, double normbin, TString xlabel, TString ylabel, double ymin , double ymax );


	// Get TChains
	TChain * inTreeSim1 = new TChain("tagged");
	TChain * inTreeSim2 = new TChain("tagged");

	// Add Files to TChain
	inTreeSim1->Add(inSim1);
	inTreeSim2->Add(inSim2);


	// Define histograms we want to plot:
	TH1D ** xp_full = new TH1D*[3];
	TH1D ** xp_asymp = new TH1D*[3];
	TH1D ** xp_singleratio = new TH1D*[3];
	TH1D ** xp_ratio_norm = new TH1D*[3];
	for(int i = 0 ; i < 3 ; i++){
		xp_full[i] = new TH1D(Form("xp_full_%i",i),"",12,0.175,0.775);
		xp_asymp[i] = new TH1D(Form("xp_asymp_%i",i),"",12,0.175,0.775);
		xp_singleratio[i] = new TH1D(Form("xp_singleratio_%i",i),"",12,0.175,0.775);
		xp_ratio_norm[i] = new TH1D(Form("xp_ratio_norm_%i",i),"",12,0.175,0.775);
	}

	// Draw the full as distribution
	TCanvas * c_xp = new TCanvas("c_xp","",800,600);
	c_xp->Divide(3,3);
	for( int i = 0 ; i < 3 ; i++){
		TString hTtitle = "1.3 < #alpha_{S} < 1.4";
		if( i == 1 ){
			hTtitle = "1.4 < #alpha_{S} < 1.5";
		}
		if( i == 2 ){
			hTtitle = "1.5 < #alpha_{S} < 1.6";
		}
		//1-3 pads
		c_xp->cd(i+1);
		inTreeSim1->Draw(Form("tag_smeared[nleadindex]->getXp_WP() >> xp_full_%i",i),pNcut_sim && pTcut_sim && aScut_sim[i]);
		inTreeSim2->Draw(Form("tag_smeared[nleadindex]->getXp_WP() >> xp_asymp_%i",i),pNcut_sim && pTcut_sim && aScut_sim[i]);


	//	Ndata = pT_dat[i]->Integral(); 		// total data we have
	//	Nsim = pT_sim[i]->Integral(); 		// total simulation we have
	//	sim_scaling = Ndata/Nsim;		// scale of the simulation bin

		cout << "As bin: " << i << " " << hTtitle << "\n";
		xp_full[i]->SetTitle(hTtitle);
		label1D(xp_full[i],xp_asymp[i],"x^{'}","Counts");

		//4-6 pads MC ratios
		c_xp->cd(4+i);
		label1D_ratio(xp_full[i],xp_asymp[i],xp_singleratio[i],"x^{'}","Full/Asymptotic",0,2);

		//7-9 pads MC ratios normalized to x' = 0.3
		c_xp->cd(7+i);
		label1D_ratio_norm(xp_full[i],xp_asymp[i],xp_ratio_norm[i],norm_xpvalue[i],"x^{'}","Full/Asymptotic",0,2);

		xp_full[i]->SaveAs(Form("xp_dist_fullMC_asbin%i.root",i+1));
		xp_asymp[i]->SaveAs(Form("xp_dist_asypMC_asbin%i.root",i+1));
		xp_singleratio[i]->SaveAs(Form("xp_ratio_full-asypMC_asbin%i.root",i+1));
		xp_ratio_norm[i]->SaveAs(Form("xp_normratio_full-asypMC_asbin%i.root",i+1));
	}


	c_xp->SaveAs("full_xp_mcratio.pdf");


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

	for( int bin = 1 ; bin < data->GetXaxis()->GetNbins() ; ++bin ){
	//	cerr << "Simfull: " << data->GetBinCenter(bin) << " " << data->GetBinContent(bin) << " " << data->GetBinError(bin) << "\n";
	//	cerr << "Simasymp: " << sim->GetBinCenter(bin) << " " << sim->GetBinContent(bin) << " " << sim->GetBinError(bin) << "\n";
	}

	double max1 = data->GetMaximum()*1.1;
	double max2 = sim->GetMaximum()*1.1;
	sim->GetYaxis()->SetRangeUser(0,max(max1,max2));

	sim->GetXaxis()->SetTitle(xlabel);
	sim->GetYaxis()->SetTitle(ylabel);

	TLegend * legend = new TLegend(0.7,0.8,0.9,0.9);
	//legend->AddEntry(data,"Radiation On","f");
	//legend->AddEntry(sim,"Radiation Off","f");
	legend->AddEntry(data,"Sim Full","f");
	legend->AddEntry(sim,"Sim Asymptotic","f");
	legend->Draw();

	return;
}

void label1D_ratio(TH1D* data, TH1D* sim, TH1D* ratiohist, TString xlabel, TString ylabel, double ymin , double ymax ){
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

	for( int bin = 1 ; bin < data_copy->GetXaxis()->GetNbins() ; ++bin ){
		double xval = data_copy->GetBinCenter(bin);

		double D = data_copy->GetBinContent(bin);
		double S = sim_copy->GetBinContent(bin);

		double Derr = data_copy->GetBinError(bin);
		double Serr = data_copy->GetBinError(bin);

		double ratio = D/S;
		double error = sqrt( pow(Derr/S,2) + pow(D/(S*S)*Serr,2) );

		if( S == 0 ){
			data_copy->SetBinContent(bin,-1);
			data_copy->SetBinError(bin,0);
		//	cerr << "Full/Asymp: " << xval << " " << -1 << " " << 0 << "\n";
		}
		else{
			data_copy->SetBinContent(bin,ratio);
			data_copy->SetBinError(bin,error);
			ratiohist->SetBinContent(bin,ratio);
			ratiohist->SetBinError(bin,error);
			cerr << "Full/Asymp: " << xval << " " << ratio << " " << error << "\n";
		}
	}

	//data_copy->Divide(sim_copy);
	//data_copy->SetTitle( sim_copy->GetTitle() );
	//data_copy->Draw("ep");
	ratiohist->SetTitle( sim_copy->GetTitle() );
	ratiohist->Draw("ep");
	TLine* line = new TLine(data_copy->GetXaxis()->GetBinCenter(1), 1., data_copy->GetXaxis()->GetBinCenter(data_copy->GetXaxis()->GetNbins()), 1.);
	line->SetLineWidth(3);
	line->SetLineColor(2);
	line->Draw("same");

	ratiohist->GetYaxis()->SetRangeUser(ymin,ymax);

	ratiohist->GetXaxis()->SetTitle(xlabel);
	ratiohist->GetYaxis()->SetTitle(ylabel);

	return;
}


void label1D_ratio_norm(TH1D* data, TH1D* sim, TH1D* ratiohist, double normbin, TString xlabel, TString ylabel, double ymin , double ymax ){
	gStyle->SetOptFit(1);

	TH1D * data_copy = (TH1D*) data->Clone();
	TH1D * sim_copy = (TH1D*) sim->Clone();

	data_copy->SetLineColor(1);
	data_copy->SetLineWidth(3);
	//data_copy->SetStats(0);
	cerr << "Sim 1 normbin " << data_copy->FindBin(normbin) << " with content " << data_copy->GetBinContent(data_copy->FindBin(normbin)) << endl;
	data_copy->Scale(1./data_copy->GetBinContent(data_copy->FindBin(normbin)) );

	sim_copy->SetLineColor(9);
	sim_copy->SetLineWidth(3);
	cerr << "Sim 2 normbin " << sim_copy->FindBin(normbin) << " with content " << sim_copy->GetBinContent(sim_copy->FindBin(normbin)) << endl;
	sim_copy->Scale(1./sim_copy->GetBinContent(sim_copy->FindBin(normbin)) );
	//sim_copy->SetStats(0);
	//sim_copy->Scale(data_copy->Integral() / sim_copy->Integral() );

	for( int bin = 1 ; bin < data_copy->GetXaxis()->GetNbins() ; ++bin ){
		double xval = data_copy->GetBinCenter(bin);

		double D = data_copy->GetBinContent(bin);
		double S = sim_copy->GetBinContent(bin);

		double Derr = data_copy->GetBinError(bin);
		double Serr = data_copy->GetBinError(bin);

		double ratio = D/S;
		double error = sqrt( pow(Derr/S,2) + pow(D/(S*S)*Serr,2) );

		if( S == 0 ){
			data_copy->SetBinContent(bin,-1);
			data_copy->SetBinError(bin,0);
		//	cerr << "Full/Asymp: " << xval << " " << -1 << " " << 0 << "\n";
		}
		else{
			data_copy->SetBinContent(bin,ratio);
			data_copy->SetBinError(bin,error);
			ratiohist->SetBinContent(bin,ratio);
			ratiohist->SetBinError(bin,error);
			cerr << "Full/Asymp(norm): " << xval << " " << ratio << " " << error << "\n";
		}
	}

	//data_copy->Divide(sim_copy);
	//data_copy->SetTitle( sim_copy->GetTitle() );
	//data_copy->Draw("ep");
	ratiohist->SetTitle( sim_copy->GetTitle() );
	ratiohist->Draw("ep");
	TLine* line = new TLine(data_copy->GetXaxis()->GetBinCenter(1), 1., data_copy->GetXaxis()->GetBinCenter(data_copy->GetXaxis()->GetNbins()), 1.);
	line->SetLineWidth(3);
	line->SetLineColor(2);
	line->Draw("same");

	ratiohist->GetYaxis()->SetRangeUser(ymin,ymax);

	ratiohist->GetXaxis()->SetTitle(xlabel);
	ratiohist->GetYaxis()->SetTitle(ylabel);

	return;
}
