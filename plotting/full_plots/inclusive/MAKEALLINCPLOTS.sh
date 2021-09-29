#!/bin/bash

#$1 defines beam energy setting 1 = 10.2, 2 = 10.4, 3 = 10.6
#$2 choose if smeared values are used. 0 no smearing, other with smearing
if [ $1 == 1 ]
then
  echo "Inclusive comparison plots for 10.2 GeV"
  if [ ! -d "inc_plots10.2" ]
  then
    echo "inc_plots10.2 does not exist. Create directory now."
    mkdir inc_plots10.2
  fi
  cd inc_plots10.2
  datapath=/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/inclusive/final_inclusive_006*.root
  simpath=/volatile/clas12/users/segarrae/BAND/v3.1/10.2/final/final_inclusive_sim_10pt2.root

  root -l -b -q "../pe_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../thetae_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../phie_inc.C(\"$datapath\",\"$simpath\",$2)"

  root -l -b -q "../xB_inc.C\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../Q2_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../W2_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../omega_inc.C(\"$datapath\",\"$simpath\",$2)"

  gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=inclusive_compare_10pt2.pdf full_*

elif [ $1 == 2 ]
then
  echo "Inclusive comparison plots for 10.4 GeV"
  if [ ! -d "inc_plots10.4" ]
  then
    echo "inc_plots10.4 does not exist. Create directory now."
    mkdir inc_plots10.4
  fi
  cd inc_plots10.4
  datapath=/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/inclusive/final_inclusive_011*.root
  simpath=/volatile/clas12/users/segarrae/BAND/v3.1/10.4/final/final_inclusive_sim_10pt4.root

  root -l -b -q "../pe_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../thetae_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../phie_inc.C(\"$datapath\",\"$simpath\",$2)"

  root -l -b -q "../xB_inc.C\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../Q2_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../W2_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../omega_inc.C(\"$datapath\",\"$simpath\",$2)"

  gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=inclusive_compare_10pt4.pdf full_*

elif [ $1 == 3 ]
then
  echo "Inclusive comparison plots for 10.6 GeV"
  if [ ! -d "inc_plots10.6" ]
  then
    echo "inc_plots10.6 does not exist. Create directory now."
    mkdir inc_plots10.6
  fi
  cd inc_plots10.6
  datapath=/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/inclusive/final_inclusive_006*.root
  simpath=/volatile/clas12/users/segarrae/BAND/v3.1/10.6/final/final_inclusive_sim_10pt6.root

  root -l -b -q "../pe_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../thetae_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../phie_inc.C(\"$datapath\",\"$simpath\",$2)"

  root -l -b -q "../xB_inc.C\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../Q2_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../W2_inc.C(\"$datapath\",\"$simpath\",$2)"
  root -l -b -q "../omega_inc.C(\"$datapath\",\"$simpath\",$2)"

  gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=inclusive_compare_10pt6.pdf full_*

else
  echo "Parameter $1 not implemented. No inclusive comparisons plots created"
fi

# Electron quantities:
