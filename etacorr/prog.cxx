#include "rapcorr.h"

void plotRapidityCorrelations();
int addUrQMDEventsFromPath(TChain*);
double * fillRapidities(TrackInfo&, int, int&); 
void getInfo(int, float, float, float, TString&, float&, float&, float&,
		    	float&, float&, float&, float&, float&, float&);

int main(int argc, char **argv) {
	gRandom->SetSeed(123456);
	plotRapidityCorrelations();
	return 0;
}

void plotRapidityCorrelations() {

	float mass, charge, lifetime, eta, rapidity, phi, pTotal, pt, baryonNo; 
	TString name;

	Int_t ievt;
	Float_t parimp;
	Float_t gecm;
	Float_t evinfo[5];
	Int_t mpart;
	Int_t igid[11000];   
	Float_t gpx[11000];    
	Float_t gpy[11000];    
	Float_t gpz[11000];    
	
	TBranch *b_ievt;     
	TBranch *b_parimp;   
	TBranch *b_gecm;     
	TBranch *b_evinfo;   
	TBranch *b_mpart;    
	TBranch *b_igid;     
	TBranch *b_gpx;      
	TBranch *b_gpy;      
	TBranch *b_gpz;      

	TChain *chain = new TChain("h1", "events chain");
	int nb, nentries = addUrQMDEventsFromPath(chain);

	chain->SetMakeClass(1);
	chain->SetBranchAddress("ievt", &ievt, &b_ievt);
	chain->SetBranchAddress("parimp", &parimp, &b_parimp);
	chain->SetBranchAddress("gecm", &gecm, &b_gecm);
	chain->SetBranchAddress("evinfo", evinfo, &b_evinfo);
	chain->SetBranchAddress("mpart", &mpart, &b_mpart);
	chain->SetBranchAddress("igid", igid, &b_igid);
	chain->SetBranchAddress("gpx", gpx, &b_gpx);
	chain->SetBranchAddress("gpy", gpy, &b_gpy);
	chain->SetBranchAddress("gpz", gpz, &b_gpz);

	RapCorr *rapCorr = new RapCorr();
	rapCorr->setRunR2(true);
	rapCorr->setRunR3(false);
	rapCorr->book();
	
	const int N_EVENTS = 100000;
	TrackInfo trackInfo = {igid, gpx, gpy, gpz, name, mass, charge, 
	lifetime, eta, rapidity, phi, pTotal, pt, baryonNo};

	for(Long64_t iEvent = 0; iEvent < N_EVENTS; iEvent++) {

		if(iEvent % (N_EVENTS / 10) == 0) { 
			cout << "processing " << iEvent << " of " << N_EVENTS << endl; 
		}
               
		nb = chain->GetEntry(iEvent);
		int N_TRACKS = mpart;
	    int N_PROTON_TRACKS = 0;
	    double *rapidityArr = new double[N_TRACKS];
	    memset(rapidityArr, 0, N_TRACKS*sizeof(double)); 

		if(parimp > 3.2) {
			delete[] rapidityArr; rapidityArr = 0;
			continue; // allow only 0-5% central collisions...  
		} 

		rapidityArr = fillRapidities(trackInfo, N_TRACKS, N_PROTON_TRACKS);
		rapCorr->increment(rapidityArr, N_PROTON_TRACKS);
		delete[] rapidityArr; rapidityArr = 0;	
	}
	rapCorr->calculate();
	delete rapCorr; rapCorr = 0;
}

int addUrQMDEventsFromPath(TChain *chain) {
    TString path = TString("/nfs/rhi/UrQMD/events_2016/007/");
	TString	filenames = TString("urqmd_19_0005_*.root");
	TString input = path + filenames;
	cout << input.Data() << endl;
	chain->Add(input.Data());
	int neventtree = chain->GetEntries();
	cout << "N_events = " << neventtree << endl;
	return neventtree;
}

double * fillRapidities(TrackInfo &info, int N_TRACKS, int &N_PROTON_TRACKS) {
	double *rapidityArr = new double[N_TRACKS];
	int protonCount = 0; 
	const int PROTON = 14;

	for(int iTrack = 0; iTrack < N_TRACKS; iTrack++) {
		getInfo(info.geantID[iTrack], info.px[iTrack], info.py[iTrack], info.pz[iTrack], 
			info.name, info.mass, info.charge, info.lifetime, info.eta, info.rapidity, info.phi, info.pTotal, info.pt, info.baryonNo);
		if(info.geantID[iTrack] == PROTON && abs(info.rapidity) <= 1.0) { 
			rapidityArr[protonCount] = info.rapidity;
			protonCount++;
		}
	}
	N_PROTON_TRACKS = protonCount;
	return rapidityArr;
}

void getInfo(int geantID, float px, float py, float pz, 
        TString &name, float &mass, float &charge, float &lifetime,
        float &eta, float &rapidity, float &phi, 
        float &pTotal, float &pt, float &baryonNo) {

        mass = charge = lifetime = -9; 
        if(geantID == 1)      {name = TString("GAMMA");         mass = .0000E+00; charge =  0.; lifetime = .10000E+16; baryonNo =  0;}
        else if(geantID == 2) {name = TString("POSITRON");      mass = .5110E-03; charge =  1.; lifetime = .10000E+16; baryonNo =  0;}
        else if(geantID == 3) {name = TString("ELECTRON");      mass = .5110E-03; charge = -1.; lifetime = .10000E+16; baryonNo =  0;}
        else if(geantID == 4) {name = TString("NEUTRINO");      mass = .0000E+00; charge =  0.; lifetime = .10000E+16; baryonNo =  0;}
        else if(geantID == 5) {name = TString("MUON+");         mass = .1057E+00; charge =  1.; lifetime = .21970E-05; baryonNo =  0;}
        else if(geantID == 6) {name = TString("MUON-");         mass = .1057E+00; charge = -1.; lifetime = .21970E-05; baryonNo =  0;}
        else if(geantID == 7) {name = TString("PION0");         mass = .1350E+00; charge =  0.; lifetime = .84000E-16; baryonNo =  0;}
        else if(geantID == 8) {name = TString("PION+");         mass = .1396E+00; charge =  1.; lifetime = .26030E-07; baryonNo =  0;}
        else if(geantID == 9) {name = TString("PION-");         mass = .1396E+00; charge = -1.; lifetime = .26030E-07; baryonNo =  0;}
        else if(geantID == 10) {name = TString("KAON0LONG");    mass = .4977E+00; charge =  0.; lifetime = .51700E-07; baryonNo =  0;}
        else if(geantID == 11) {name = TString("KAON+");        mass = .4937E+00; charge =  1.; lifetime = .12370E-07; baryonNo =  0;}
        else if(geantID == 12) {name = TString("KAON-");        mass = .4937E+00; charge = -1.; lifetime = .12370E-07; baryonNo =  0;}
        else if(geantID == 13) {name = TString("NEUTRON");      mass = .9396E+00; charge =  0.; lifetime = .88700E+03; baryonNo =  1;}
        else if(geantID == 14) {name = TString("PROTON");       mass = .9383E+00; charge =  1.; lifetime = .10000E+16; baryonNo =  1;}
        else if(geantID == 15) {name = TString("ANTIPROTON");   mass = .9383E+00; charge = -1.; lifetime = .10000E+16; baryonNo = -1;}
        else if(geantID == 16) {name = TString("KAON 0 SHORT"); mass = .4977E+00; charge =  0.; lifetime = .89260E-10; baryonNo =  0;}
        else if(geantID == 17) {name = TString("ETA");          mass = .5475E+00; charge =  0.; lifetime = .54850E-18; baryonNo =  0;}
        else if(geantID == 18) {name = TString("LAMBDA");       mass = .1116E+01; charge =  0.; lifetime = .26320E-09; baryonNo =  1;}
        else if(geantID == 19) {name = TString("SIGMA+");       mass = .1189E+01; charge =  1.; lifetime = .79900E-10; baryonNo =  1;}
        else if(geantID == 20) {name = TString("SIGMA0");       mass = .1193E+01; charge =  0.; lifetime = .74000E-19; baryonNo =  1;}
        else if(geantID == 21) {name = TString("SIGMA-");       mass = .1197E+01; charge = -1.; lifetime = .14790E-09; baryonNo =  1;}
        else if(geantID == 22) {name = TString("XI0");          mass = .1315E+01; charge =  0.; lifetime = .29000E-09; baryonNo =  1;}
        else if(geantID == 23) {name = TString("XI-");          mass = .1321E+01; charge = -1.; lifetime = .16390E-09; baryonNo =  1;}
        else if(geantID == 24) {name = TString("OMEGA-");       mass = .1672E+01; charge = -1.; lifetime = .82200E-10; baryonNo =  1;}
        else if(geantID == 25) {name = TString("ANTINEUTRON");  mass = .9396E+00; charge =  0.; lifetime = .88700E+03; baryonNo = -1;}
        else if(geantID == 26) {name = TString("ANTILAMBDA");   mass = .1116E+01; charge =  0.; lifetime = .26320E-09; baryonNo = -1;}
        else if(geantID == 27) {name = TString("ANTISIGMA-");   mass = .1189E+01; charge = -1.; lifetime = .79900E-10; baryonNo = -1;}
        else if(geantID == 28) {name = TString("ANTISIGMA0");   mass = .1193E+01; charge =  0.; lifetime = .74000E-19; baryonNo = -1;}
        else if(geantID == 29) {name = TString("ANTISIGMA+");   mass = .1197E+01; charge =  1.; lifetime = .14790E-09; baryonNo = -1;}
        else if(geantID == 30) {name = TString("ANTIXI0");      mass = .1315E+01; charge =  0.; lifetime = .29000E-09; baryonNo = -1;}
        else if(geantID == 31) {name = TString("ANTIXI+");      mass = .1321E+01; charge =  1.; lifetime = .16390E-09; baryonNo = -1;}
        else if(geantID == 32) {name = TString("ANTIOMEGA+");   mass = .1672E+01; charge =  1.; lifetime = .82200E-10; baryonNo = -1;}
        else if(geantID == 45) {name = TString("DEUTERON");     mass = .1876E+01; charge =  1.; lifetime = .10000E+16; baryonNo =  2;}
        else if(geantID == 46) {name = TString("TRITON");       mass = .2809E+01; charge =  1.; lifetime = .10000E+16; baryonNo =  3;}
        else if(geantID == 47) {name = TString("ALPHA");        mass = .3727E+01; charge =  2.; lifetime = .10000E+16; baryonNo =  4;}
        else if(geantID == 48) {name = TString("GEANTINO");     mass = .0000E+00; charge =  0.; lifetime = .10000E+16; baryonNo =  0;}
        else if(geantID == 49) {name = TString("HE3");          mass = .2809E+01; charge =  2.; lifetime = .10000E+16; baryonNo =  3;}
        else if(geantID == 50) {name = TString("Cerenkov");     mass = .0000E+00; charge =  0.; lifetime = .10000E+16; baryonNo =  0;}
         
        if(mass < 0) { cout << "Unknown particle " << geantID << endl; }
        
        pt = sqrt(px * px + py * py);
        pTotal = sqrt(pt * pt + pz * pz);
        float E_Total = sqrt(mass * mass + pTotal * pTotal);
        rapidity = 0.5 * TMath::Log( (E_Total + pz) / (E_Total - pz) );

        if(pz != 0.0 && pTotal > 0.001) { 
             TVector3 pVect(px, py, pz);
             eta = pVect.PseudoRapidity(); 
        } 
        else {
             eta = 0;
        }
}