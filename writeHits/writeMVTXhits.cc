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
           << "\n";

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int writeMVTXhits::process_event(PHCompositeNode *topNode)
{
  std::cout << "writeMVTXhits::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

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
  
  findNode = dynamic_cast<PHNode*>(nodeIter.findFirst("TRKR_CLUSTER"));
  if (findNode)
  {
    dst_clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  }
  else
  {
    std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__<< ": TRKR_CLUSTER does not exist" << std::endl;
  }

  for (const auto& [key_track, track] : *dst_trackmap)
  {
    for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
         iter != track->end_cluster_keys();
         ++iter)
    {
      TrkrDefs::cluskey clusKey = *iter;
      cluster = dst_clustermap->findCluster(clusKey);
      const unsigned int trkrId = TrkrDefs::getTrkrId(clusKey);

      const unsigned int trackNum = key_track;

      unsigned int layerId = 0, staveId = 0, chipId = 0;
      uint16_t row = 0, column = 0;
      float hitX = 0, hitY = 0, hitZ = 0;

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
        }
       
        hitsFile << std::setfill('0') << std::setw(6) << eventNum;
        hitsFile << ", " << std::setfill('0') << std::setw(3) << trackNum;
        hitsFile << ", " << std::setfill('0') << std::setw(2) << layerId;
        hitsFile << ", " << std::setfill('0') << std::setw(2) << staveId;
        hitsFile << ", " << std::setfill('0') << std::setw(2) << chipId;
        hitsFile << ", " << std::setfill('0') << std::setw(4) << row;
        hitsFile << ", " << std::setfill('0') << std::setw(4) << column;
        hitsFile << ", " << std::setprecision(6) << hitX;
        hitsFile << ", " << std::setprecision(6) << hitY;
        hitsFile << ", " << std::setprecision(6) << hitZ;
        hitsFile << "\n";
      }
    }
  }

  eventNum++;

  return Fun4AllReturnCodes::EVENT_OK;
}
//____________________________________________________________________________..
int writeMVTXhits::End(PHCompositeNode *topNode)
{
  std::cout << "writeMVTXhits::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  hitsFile.close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int writeMVTXhits::Reset(PHCompositeNode *topNode)
{
 std::cout << "writeMVTXhits::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void writeMVTXhits::Print(const std::string &what) const
{
  std::cout << "writeMVTXhits::Print(const std::string &what) const Printing info for " << what << std::endl;
}
