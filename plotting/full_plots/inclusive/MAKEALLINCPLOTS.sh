#/bin/bash

#option defines beam energy setting 1 = 10.2, 2 = 10.4, 3 = 10.6
option=$1
if [ $1 == 1 ]
then
  echo "Inclusive comparison plots for 10.2 GeV"
  root -l -b -q "pe_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"
  root -l -b -q "thetae_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"
  root -l -b -q "phie_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"

  root -l -b -q "xB_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"
  root -l -b -q "Q2_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"
  root -l -b -q "W2_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"
  root -l -b -q "omega_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root\")"

  gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=inclusive_compare_10pt2.pdf full_*

  if [ ! -d "inc_plots10.2" ]
  then
    echo "inc_plots10.2 does not exist. Create directory now."
    mkdir inc_plots10.2
  fi
  mv full_*.pdf inc_plots10.2
  mv inclusive_compare_10pt2.pdf inc_plots10.2

elif [ $1 == 2 ]
then
  echo "Inclusive comparison plots for 10.4 GeV"
  root -l -b -q "pe_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"
  root -l -b -q "thetae_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"
  root -l -b -q "phie_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"

  root -l -b -q "xB_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"
  root -l -b -q "Q2_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"
  root -l -b -q "W2_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"
  root -l -b -q "omega_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root\")"

  gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=inclusive_compare_10pt4.pdf full_*

  if [ ! -d "inc_plots10.4" ]
  then
    echo "inc_plots10.4 does not exist. Create directory now."
    mkdir inc_plots10.4
  fi
  mv full_*.pdf inc_plots10.4
  mv inclusive_compare_10pt4.pdf inc_plots10.4

elif [ $1 == 3 ]
then
  echo "Inclusive comparison plots for 10.6 GeV"
  root -l -b -q "pe_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"
  root -l -b -q "thetae_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"
  root -l -b -q "phie_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"

  root -l -b -q "xB_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"
  root -l -b -q "Q2_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"
  root -l -b -q "W2_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"
  root -l -b -q "omega_inc.C(\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_*.root\",\"/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root\")"

  gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=inclusive_compare_10pt6.pdf full_*

  if [ ! -d "inc_plots10.6" ]
  then
    echo "inc_plots10.6 does not exist. Create directory now."
    mkdir inc_plots10.6
  fi
  mv full_*.pdf inc_plots10.6
  mv inclusive_compare_10pt6.pdf inc_plots10.6
else
  echo "Parameter $1 not implemented. No inclusive comparisons plots created"
fi

# Electron quantities:
