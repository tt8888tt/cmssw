#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DQMOffline/Trigger/interface/HeavyIonUCC.h"

HeavyIonUCC::HeavyIonUCC(const edm::ParameterSet& ps)
{
	triggerResults_ = consumes<edm::TriggerResults>(ps.getParameter<edm::InputTag>("triggerResults"));
	theCaloMet = consumes<reco::CaloMETCollection>(ps.getParameter<edm::InputTag>("caloMet"));
	theSiPixelCluster = consumes<edmNew::DetSetVector<SiPixelCluster> >(ps.getParameter<edm::InputTag>("pixelCluster"));
	triggerPath_ = ps.getParameter<std::string>("triggerPath");

	nClusters = ps.getParameter<int>("nClusters");
	minClusters = ps.getParameter<int>("minClusters");
	maxClusters = ps.getParameter<int>("maxClusters");
	nEt = ps.getParameter<int>("nEt");
	minEt = ps.getParameter<double>("minEt");
	maxEt = ps.getParameter<double>("maxEt");
}

HeavyIonUCC::~HeavyIonUCC()
{

}


void HeavyIonUCC::bookHistograms(DQMStore::IBooker & ibooker_, edm::Run const &, edm::EventSetup const &)
{
	ibooker_.cd();;
	ibooker_.setCurrentFolder("HLT/HI/" + triggerPath_);

	h_SumEt = ibooker_.book1D("h_SumEt", "SumEt", nEt, minEt, maxEt);
	h_SiPixelClusters = ibooker_.book1D("h_SiPixelClusters", "h_SiPixelClusters", nClusters, minClusters, maxClusters);

	ibooker_.cd();
}

void HeavyIonUCC::analyze(edm::Event const& e, edm::EventSetup const& eSetup)
{

	edm::Handle<edm::TriggerResults> hltresults;
	e.getByToken(triggerResults_,hltresults);
	if(!hltresults.isValid())
	{
		edm::LogError ("HeavyIonUCC") << "invalid collection: TriggerResults" << "\n";
		return;
	}

	bool hasFired = false;
	const edm::TriggerNames& trigNames = e.triggerNames(*hltresults);
	unsigned int numTriggers = trigNames.size();
	for( unsigned int hltIndex=0; hltIndex<numTriggers; ++hltIndex ) {
		if (trigNames.triggerName(hltIndex).find(triggerPath_) != std::string::npos && hltresults->wasrun(hltIndex) && hltresults->accept(hltIndex)){
			hasFired = true;
		}
	}

	if (!hasFired) return;

	edm::Handle<edmNew::DetSetVector<SiPixelCluster> > cluster;
	e.getByToken(theSiPixelCluster, cluster);
	if ( cluster.isValid() ) {
		h_SiPixelClusters->Fill(cluster->dataSize());
	}

	edm::Handle<reco::CaloMETCollection> calomet;
	e.getByToken(theCaloMet, calomet);
	if ( calomet.isValid() ) {
		h_SumEt->Fill(calomet.sumEt());
	}

}

DEFINE_FWK_MODULE(HeavyIonUCC);
