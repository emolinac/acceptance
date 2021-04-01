void MACRO_accfactors_parallel_Q2NuZh(Int_t N_Q2, Int_t N_Nu, Int_t N_Zh, Int_t N_Pt2, Int_t N_Phi, Int_t Q2_bin_selected, Int_t Nu_bin_selected, TString material, Int_t nSimuFiles, TString fsimul_loc, TString facc_target_loc, TString fbin_source_loc)
{
  /*FILES CUTS*/
  TCut Target_cut2;
  if(material=="D"){Target_cut2 = "TargType==1";}
  else{Target_cut2 = "TargType==2";}

  std::cout<<"Running for "<<material<<" so TargetType="<<Target_cut2<<std::endl; 

  //FILES TO USE
  TFile* fbin_source = new TFile(fbin_source_loc + Form("binning_"+material+"_%i%i%i%i%i.root",N_Q2,N_Nu,N_Zh,N_Pt2,N_Phi),"READ");
  TFile* facc_target = new TFile(Form("./accfactors_"+material+"_%i%i_%i%i%i%i%i.root",Q2_bin_selected,Nu_bin_selected,N_Q2,N_Nu,N_Zh,N_Pt2,N_Phi),"RECREATE");
  TFile* fsimul      = new TFile(Form(fsimul_loc + material + "%i_simul.root", nSimuFiles),"READ");

  //GETTING SIMUL TUPLE
  TNtuple* reconstructed = (TNtuple*) fsimul->Get("ntuple_accept");
  TNtuple* thrown        = (TNtuple*) fsimul->Get("ntuple_thrown");

  /*GETTING THE BIN LIMITS TUPLE*/
  TNtuple* limits_tuple = (TNtuple*) fbin_source->Get("limits_tuple");
  Float_t Q2_bin_min, Q2_bin_max,Nu_bin_min, Nu_bin_max,Zh_bin_min, Zh_bin_max,Pt2_bin_min, Pt2_bin_max,Phi_bin_min, Phi_bin_max;
  Float_t Q2_bin, Nu_bin, Zh_bin, Pt2_bin, Phi_bin;
  limits_tuple->SetBranchAddress("Q2_min",&Q2_bin_min);
  limits_tuple->SetBranchAddress("Q2_max",&Q2_bin_max);
  limits_tuple->SetBranchAddress("Nu_min",&Nu_bin_min);
  limits_tuple->SetBranchAddress("Nu_max",&Nu_bin_max);
  limits_tuple->SetBranchAddress("Zh_min",&Zh_bin_min);
  limits_tuple->SetBranchAddress("Zh_max",&Zh_bin_max);
  limits_tuple->SetBranchAddress("Pt2_min",&Pt2_bin_min);
  limits_tuple->SetBranchAddress("Pt2_max",&Pt2_bin_max);
  limits_tuple->SetBranchAddress("Phi_min",&Phi_bin_min);
  limits_tuple->SetBranchAddress("Phi_max",&Phi_bin_max);
  limits_tuple->SetBranchAddress("Q2_bin",&Q2_bin);
  limits_tuple->SetBranchAddress("Nu_bin",&Nu_bin);
  limits_tuple->SetBranchAddress("Zh_bin",&Zh_bin);
  limits_tuple->SetBranchAddress("Pt2_bin",&Pt2_bin);
  limits_tuple->SetBranchAddress("Phi_bin",&Phi_bin);

  Double_t limits_entries = limits_tuple->GetEntries();

  //SETTING ENTRY LISTS

  std::cout<<"Q2 bin selected "<<Q2_bin_selected<<" Nu bin selected "<<Nu_bin_selected<<std::endl;

  Float_t Q2_min_event = 0;
  Float_t Q2_max_event = 0;
  Float_t Nu_min_event = 0;
  Float_t Nu_max_event = 0;

  for(Int_t acc_entry = 0 ; acc_entry < limits_entries ; acc_entry++)
    {
      limits_tuple->GetEntry(acc_entry);
      if(Q2_bin==Q2_bin_selected&&Nu_bin==Nu_bin_selected)
	{
	  Q2_min_event = Q2_bin_min;
	  Q2_max_event = Q2_bin_max;
	  Nu_min_event = Nu_bin_min;
	  Nu_max_event = Nu_bin_max;
	  break;
	}
    } 

  std::cout<<Q2_min_event<<" < Q2 < "<<Q2_max_event<<std::endl;
  std::cout<<Nu_min_event<<" < Nu < "<<Nu_max_event<<std::endl;


  TCut Q2_cut_S     = Form("Q2>%f && Q2<%f", Q2_min_event, Q2_max_event);                 
  TCut Nu_cut_S     = Form("Nu>%f && Nu<%f", Nu_min_event, Nu_max_event);
  TCut YC_cut_S     = "YC>0.1||YC<-0.1";
  TCut deltaZ_cut_S = "deltaZ<3&&deltaZ>-3";
  TCut Xf_cut_S     = "Xf>0";
  TCut cuts_simul   = Q2_cut_S&&Nu_cut_S&&deltaZ_cut_S;

  reconstructed->Draw(">>list_acc",cuts_simul&&YC_cut_S&&Target_cut2);
  thrown->Draw(">>list_thr",cuts_simul);
  TEventList* evnt_acc = (TEventList*) gDirectory->Get("list_acc");
  TEventList* evnt_thr = (TEventList*) gDirectory->Get("list_thr");

  reconstructed->Draw(">>list_acc_Xf",cuts_simul&&YC_cut_S&&Target_cut2&&Xf_cut_S);
  thrown->Draw(">>list_thr_Xf",cuts_simul&&Xf_cut_S);
  TEventList* evnt_acc_Xf = (TEventList*) gDirectory->Get("list_acc_Xf");
  TEventList* evnt_thr_Xf = (TEventList*) gDirectory->Get("list_thr_Xf");

  // ACC TNTUPLES
  TNtuple *accfactors_tuple = new TNtuple("accfactors_tuple","accfactors_tuple","Q2_bin:Nu_bin:Zh_bin:Pt2_bin:Phi_bin:acc_factor:acc_error:N_accept:N_thrown");
  TNtuple *accfactors_Xf_tuple = new TNtuple("accfactors_Xf_tuple","accfactors_Xf_tuple","Q2_bin:Nu_bin:Zh_bin:Pt2_bin:Phi_bin:acc_factor:acc_error:N_accept:N_thrown");

  TStopwatch t;
  t.Start();
  TStopwatch t_loop;

  reconstructed->SetEventList(evnt_acc);
  thrown->SetEventList(evnt_thr);
  TCut Zh_cut, Pt2_cut, Phi_cut, cuts_loop, cuts_Xf_loop;
  for(Int_t acc_entry = 0 ; acc_entry < limits_entries ; acc_entry++)
    {
      limits_tuple->GetEntry(acc_entry);
      if(Q2_bin==Q2_bin_selected&&Nu_bin==Nu_bin_selected)
	{
	  t_loop.Start();
	  /*SETTING THE CUTS*/
	  Zh_cut     = Form("Zh>%f && Zh<%f", Zh_bin_min , Zh_bin_max);
	  Pt2_cut    = Form("Pt2>%f && Pt2<%f", Pt2_bin_min , Pt2_bin_max);
	  Phi_cut    = Form("PhiPQ>%f && PhiPQ<%f", Phi_bin_min , Phi_bin_max);
	  cuts_loop  = Zh_cut&&Pt2_cut&&Phi_cut;

	  std::cout<<"The cuts are: "<<cuts_loop<<std::endl;

	  /*CALCULATING THE EVENTS AND ACCEPTANCE*/
	  Double_t N_THR = thrown->GetEntries(cuts_loop);
	  if(N_THR == 0){
	    std::cout<<"Null thr found! NEXT!"<<std::endl;
	    t_loop.Stop();
	    t_loop.Print();
	    continue;}
	  Double_t N_REC = reconstructed->GetEntries(cuts_loop);

	  Double_t acc_factor = N_REC/N_THR;

	  /*CALCULATING THE ERRORS*/
	  //REC ERROR
	  Double_t rec_error = TMath::Sqrt(N_REC);
	  //THR ERROR
	  Double_t thr_error = TMath::Sqrt(N_THR);
	  //ACC ERROR
	  Double_t acc_error = acc_factor*TMath::Sqrt(TMath::Power(rec_error/N_REC,2) + TMath::Power(thr_error/N_THR,2));

	  /*WRITE THE FACTORS ON THE TUPLE*/
	  accfactors_tuple->Fill(Q2_bin,Nu_bin,Zh_bin,Pt2_bin,Phi_bin,acc_factor,acc_error,N_REC,N_THR);
	  t_loop.Stop();
	  t_loop.Print();
	  std::cout<<"Acc = "<<acc_factor<<std::endl;
	}
    }	      

  reconstructed->SetEventList(evnt_acc_Xf);
  thrown->SetEventList(evnt_thr_Xf);
  for(Int_t acc_entry = 0 ; acc_entry < limits_entries ; acc_entry++)
    {
      limits_tuple->GetEntry(acc_entry);
      if(Q2_bin==Q2_bin_selected&&Nu_bin==Nu_bin_selected)
	{
	  t_loop.Start();
	  /*SETTING THE CUTS*/
	  Zh_cut     = Form("Zh>%f && Zh<%f", Zh_bin_min , Zh_bin_max);
	  Pt2_cut    = Form("Pt2>%f && Pt2<%f", Pt2_bin_min , Pt2_bin_max);
	  Phi_cut    = Form("PhiPQ>%f && PhiPQ<%f", Phi_bin_min , Phi_bin_max);

	  cuts_Xf_loop = Zh_cut&&Pt2_cut&&Phi_cut;

	  std::cout<<"The cuts are: "<<cuts_Xf_loop<<std::endl;

	  /*CALCULATING THE EVENTS AND ACCEPTANCE*/
	  Double_t N_THR_Xf = thrown->GetEntries(cuts_Xf_loop);
	  if(N_THR_Xf == 0){
	    std::cout<<"Null thr found! NEXT!"<<std::endl;
	    t_loop.Stop();
	    t_loop.Print();
	    continue;}

	  Double_t N_REC_Xf = reconstructed->GetEntries(cuts_Xf_loop);

	  Double_t acc_factor_Xf = N_REC_Xf/N_THR_Xf;

	  /*CALCULATING THE ERRORS*/
	  //REC ERROR XF
	  Double_t rec_error_Xf = TMath::Sqrt(N_REC_Xf);
	  //THR ERROR XF
	  Double_t thr_error_Xf = TMath::Sqrt(N_THR_Xf);
	  //ACC ERROR XF
	  Double_t acc_error_Xf = acc_factor_Xf*TMath::Sqrt(TMath::Power(rec_error_Xf/N_REC_Xf,2) + TMath::Power(thr_error_Xf/N_THR_Xf,2));

	  /*WRITE THE FACTORS ON THE TUPLE*/
	  accfactors_Xf_tuple->Fill(Q2_bin,Nu_bin,Zh_bin,Pt2_bin,Phi_bin,acc_factor_Xf,acc_error_Xf,N_REC_Xf,N_THR_Xf);
	  t_loop.Stop();
	  t_loop.Print();
	  std::cout<<"Acc_Xf = "<<acc_factor_Xf<<std::endl;
	}
    }	      

  t.Stop();
  t.Print();

  //SAVING ACC TNTUPLES IN FILE

  facc_target->cd();
  accfactors_tuple->Write();
  accfactors_Xf_tuple->Write();

  facc_target->Close();

  std::cout<<"All done!"<<std::endl;
}
