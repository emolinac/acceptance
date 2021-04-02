**ULTIMATE ACCEPTANCE**

This is a code that calculates acceptance in parallel form. It is very fast (~3 seconds per five dimensional factor). According to the number of bins you have use the version that suits you the most.

**Requirements:**

-ROOT

-Access to the CCTVAL cluster

-ROOT file that contain a TNtuple called "**limits_tuple**". With variables list **Q2_bin_min,Q2_bin_max,Nu_bin_min,Nu_bin_max,Zh_bin_min,Zh_bin_max,Pt2_bin_min,Pt2_bin_max,Phi_bin_min,Phi_bin_max,Q2_bin,Nu_bin,Zh_bin,Pt2_bin,Phi_bin**. Named based on the following format: **binning_target_NQ2NNUNZHNPT2NPHI.root** (N _X_ is the number of bins of the _X_ variable).

Each entry of the Tuple must contain the bin of the variable and the kinematical limits of that bin.

-Simulations that contains two tuples called **ntuple_accept** and **ntuple_thrown**. The name format of the simul must be **target1_simul.root**. Ex: D1_simul.root, C1_simul.root

**How to use:**

-Change addresses in the bash script called _parallel_ according to your locations.

-Execute the _parallel_ file according to the variables you want to parallelize. The codes ending in _Q2Nu_ parallelize those two variables. Then you will have NQ2 * NNU jobs sent. Same story for the _Q2NuZh_ version.
