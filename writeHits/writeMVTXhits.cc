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
           << ", staveID"
           << ", chipID"
           << ", row"
           << ", column"
           << ", global_x"
           << ", global_y"
           << ", global_z"
           << ", trackType"
           << ", vtxX"
           << ", vtxY"
           << ", vtxZ"
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
  }
  else
  {
    std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< ": SvtxTrackMap does not exist" << std::endl;
  }
  
  findNode = dynamic_cast<PHNode*>(nodeIter.findFirst("SvtxVertexMap"));
  if (findNode)
  {
    dst_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
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

  if (!m_svtx_evalstack)
  {
    m_svtx_evalstack = new SvtxEvalStack(topNode);
    trackeval = m_svtx_evalstack->get_track_eval();
    trutheval = m_svtx_evalstack->get_truth_eval();
  }

std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
  /*
   * Iterate over the tracks
   */
  for (const auto& [key_track, track] : *dst_trackmap)
  {

std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
    const unsigned int trackNum = key_track;
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
    const unsigned int vertexId = track->get_vertex_id();
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;

    /*
     * Find the assoiciated vertex
     */
    float vtxX = 0, vtxY = 0, vtxZ = 0;

std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
    for (const auto& [key_vertex, vertex] : *dst_vertexmap)
    {
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
      if (vertex->get_id() == track->get_vertex_id())
      {
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
        vtxX = vertex->get_x();
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
        vtxY = vertex->get_y();
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
        vtxZ = vertex->get_z();
      }
    }

    int trackType;
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
    PHG4Particle* truth_track = trackeval->max_truth_particle_by_nclusters(track);
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
    if (truth_track == nullptr)
{
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
 trackType = -1;
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
}
    else
{
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
trackType = trutheval->is_primary(truth_track);
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
}
    /*
     * Now iterate over the clusters to get the hit locations
     */ 
    for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
         iter != track->end_cluster_keys();
         ++iter)
    {
      TrkrDefs::cluskey clusKey = *iter;
      cluster = dst_clustermap->findCluster(clusKey);
      const unsigned int trkrId = TrkrDefs::getTrkrId(clusKey);

      unsigned int layerId = 0, staveId = 0, chipId = 0;
      uint16_t row = 0, column = 0;
      float hitX = 0, hitY = 0, hitZ = 0;
      float vtxX = 0, vtxY = 0, vtxZ = 0;


      if (trkrId == TrkrDefs::mvtxId)
      {
        layerId = TrkrDefs::getLayer(clusKey);
        staveId = MvtxDefs::getStaveId(clusKey);
        chipId = MvtxDefs::getChipId(clusKey); 
        row = MvtxDefs::getRow(clusKey);
        column = MvtxDefs::getCol(clusKey);
        hitX = cluster->getX();
        hitY = cluster->getY();
        hitZ = cluster->getZ();

        if (Verbosity() >= VERBOSITY_SOME)
        {
          if (Verbosity() >= VERBOSITY_MORE) track->identify();

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
        hitsFile << ", "  << std::setfill(' ') << std::setw(8) << std::setprecision(6) << hitX;
        hitsFile << ", "  << std::setfill(' ') << std::setw(8) << std::setprecision(6) << hitY;
        hitsFile << ", "  << std::setfill(' ') << std::setw(8) << std::setprecision(6) << hitZ;
        hitsFile << ", " << std::setfill(' ') << std::setw(3) << trackType;
        hitsFile << ", "  << std::setfill(' ') << std::setw(8) << std::setprecision(6) << vtxX;
        hitsFile << ", "  << std::setfill(' ') << std::setw(8) << std::setprecision(6) << vtxY;
        hitsFile << ", "  << std::setfill(' ') << std::setw(8) << std::setprecision(6) << vtxZ;
        hitsFile << "\n";
std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< std::endl;
      }
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
