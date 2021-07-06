#include "writeMVTXhits.h"

#include <fun4all/Fun4AllReturnCodes.h>

//____________________________________________________________________________..
writeMVTXhits::writeMVTXhits(const std::string &name):
 SubsysReco(name)
{
  std::cout << "writeMVTXhits::writeMVTXhits(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
writeMVTXhits::~writeMVTXhits()
{
  std::cout << "writeMVTXhits::~writeMVTXhits() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int writeMVTXhits::Init(PHCompositeNode *topNode)
{
  std::cout << "writeMVTXhits::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  
  std::string hitsFileName = Name() + ".txt";

  hitsFile.open(hitsFileName.c_str());

  //Write hit file column names
  hitsFile << "event num."
           << ", track num."
           << ", layer ID"
           << ", stave ID"
           << ", chip ID"
           << ", row"
           << ", column"
           << ", global_x"
           << ", global_y"
           << ", global_z"
           << ", trackType"
           << ", vertex_x"
           << ", vertex_y"
           << ", vertex_z"
           << "\n";

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int writeMVTXhits::process_event(PHCompositeNode *topNode)
{

  /*
   * Check that the nodes we need were produced
   */   
  PHNodeIterator nodeIter(topNode);

  PHNode *findNode = dynamic_cast<PHNode*>(nodeIter.findFirst("SvtxTrackMap"));
  if (findNode)
  {
    dst_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if (Verbosity() >= VERBOSITY_A_LOT) std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__ << ": Number of tracks: " << dst_trackmap->size() << std::endl; 
  }
  else
  {
    std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< ": SvtxTrackMap does not exist" << std::endl;
  }
  
  findNode = dynamic_cast<PHNode*>(nodeIter.findFirst("SvtxVertexMap"));
  if (findNode)
  {
    dst_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
    if (Verbosity() >= VERBOSITY_A_LOT) std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__ << ": Number of vertices: " << dst_vertexmap->size() << std::endl; 
  }
  else
  {
    std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< ": SvtxVertexMap does not exist" << std::endl;
  }

  findNode = dynamic_cast<PHNode*>(nodeIter.findFirst("TRKR_CLUSTER"));
  if (findNode)
  {
    dst_clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  }
  else
  {
    std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< ": TRKR_CLUSTER does not exist" << std::endl;
  }

  //std::unique_ptr<TrkrHitSetContainer> hitsetcontainer;
TrkrHitSetContainer* hitsetcontainer = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");

  if (!m_svtx_evalstack)
  {
    m_svtx_evalstack = new SvtxEvalStack(topNode);
    clustereval = m_svtx_evalstack->get_cluster_eval();
    hiteval = m_svtx_evalstack->get_hit_eval();
    trackeval = m_svtx_evalstack->get_track_eval();
    trutheval = m_svtx_evalstack->get_truth_eval();
  }

  m_svtx_evalstack->next_event(topNode);

  float vtxX = 0, vtxY = 0, vtxZ = 0;
  unsigned int layerId = 0, staveId = 0, chipId = 0;
  uint16_t row = 0, column = 0;
  float hitX = 0, hitY = 0, hitZ = 0;
  unsigned int trackNum = 0;
  int trackType;

  std::ostringstream position;

  TrkrHitSetContainer::ConstRange hitset_range = hitsetcontainer->getHitSets(TrkrDefs::TrkrId::mvtxId);
  for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first;
       hitset_iter != hitset_range.second;
       ++hitset_iter)
  {
    TrkrHitSet::ConstRange hit_range = hitset_iter->second->getHits();

    for (TrkrHitSet::ConstIterator hit_iter = hit_range.first;
         hit_iter != hit_range.second;
         ++hit_iter)
    {
      hitKey = hit_iter->first;
      hit = hiteval->max_truth_hit_by_energy(hitKey);
      particle = hiteval->max_truth_particle_by_energy(hitKey);
      if (particle == nullptr) std::cout << "Particle is a null pointer!" << std::endl; 
      vtx = trutheval->get_vertex(particle);
      clusKey = clustereval->best_cluster_from(hit); 

      trackNum = particle->get_track_id();
      layerId = TrkrDefs::getLayer(clusKey);
      staveId = MvtxDefs::getStaveId(clusKey);
      chipId = MvtxDefs::getChipId(clusKey); 
      row = MvtxDefs::getRow(hitKey);
      column = MvtxDefs::getCol(hitKey);
      hitX = hit->get_x(0);
      hitY = hit->get_y(0);
      hitZ = hit->get_z(0);
      trackType = particle->get_primary_id();
      vtxX = vtx->get_x();
      vtxY = vtx->get_y();
      vtxZ = vtx->get_z();

      if (trackNum > 999) continue;

      if (Verbosity() >= VERBOSITY_SOME)
      {
        if (Verbosity() >= VERBOSITY_MORE) particle->identify();

        std::cout << "event_num: " << eventNum << std::endl;
        std::cout << "track_num: " << trackNum << std::endl;
        std::cout << "layer_ID: " << layerId << std::endl;
        std::cout << "stave_ID: " << staveId << std::endl;
        std::cout << "chip_ID: " << chipId << std::endl;
        std::cout << "row: " << row << std::endl;
        std::cout << "column: " << column << std::endl;
        std::cout << "hitX: " << hitX << std::endl;
        std::cout << "hitY: " << hitY << std::endl;
        std::cout << "hitZ: " << hitZ << std::endl;
        std::cout << "trackType: " << trackType << std::endl;
        std::cout << "vtxX: " << vtxX << std::endl;
        std::cout << "vtxY: " << vtxY << std::endl;
        std::cout << "vtxZ: " << vtxZ << std::endl;
      }
       
      hitsFile << std::setfill('0') << std::setw(6) << eventNum;
      hitsFile << ", " << std::setfill('0') << std::setw(3) << trackNum;
      hitsFile << ", " << std::setfill('0') << std::setw(2) << layerId;
      hitsFile << ", " << std::setfill('0') << std::setw(2) << staveId;
      hitsFile << ", " << std::setfill('0') << std::setw(2) << chipId;
      hitsFile << ", " << std::setfill('0') << std::setw(4) << row;
      hitsFile << ", " << std::setfill('0') << std::setw(4) << column;
      position << ", "  << std::setfill(' ') << std::setw(9) << hitX;
      hitsFile << position.str(); position.str(""); position.clear();
      position << ", "  << std::setfill(' ') << std::setw(9) << hitY;
      hitsFile << position.str(); position.str(""); position.clear();
      position << ", "  << std::setfill(' ') << std::setw(9) << hitZ;
      hitsFile << position.str(); position.str(""); position.clear();
      position << ", " << std::setfill(' ') << std::setw(3) << trackType;
      hitsFile << position.str(); position.str(""); position.clear();
      position << ", "  << std::setfill(' ') << std::setw(9) << vtxX;
      hitsFile << position.str(); position.str(""); position.clear();
      position << ", "  << std::setfill(' ') << std::setw(9) << vtxY;
      hitsFile << position.str(); position.str(""); position.clear();
      position << ", "  << std::setfill(' ') << std::setw(9) << vtxZ;
      hitsFile << position.str(); position.str(""); position.clear();
      hitsFile << "\n";
    }
  }
  eventNum++;

  return Fun4AllReturnCodes::EVENT_OK;
}
//____________________________________________________________________________..
int writeMVTXhits::End(PHCompositeNode *topNode)
{
  hitsFile.close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int writeMVTXhits::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void writeMVTXhits::Print(const std::string &what) const
{
  std::cout << __FILE__ << "::" << __func__ << ": No print is impelented" << std::endl;
}
