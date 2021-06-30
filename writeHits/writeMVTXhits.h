// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef WRITEMVTXHITS_H
#define WRITEMVTXHITS_H

#include <fun4all/SubsysReco.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <mvtx/MvtxDefs.h>
#include <phool/getClass.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

class PHCompositeNode;
class SvtxTrack;
class SvtxTrackMap;
class TrkrClusterContainer;

class writeMVTXhits : public SubsysReco
{
 public:

  writeMVTXhits(const std::string &name = "writeMVTXhits");

  virtual ~writeMVTXhits();

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:

  unsigned int eventNum = 0;

  std::ofstream hitsFile;

  SvtxEvalStack *m_svtx_evalstack = nullptr;
  SvtxTrackEval *trackeval = nullptr;
  SvtxTruthEval *trutheval = nullptr;

  SvtxTrackMap *dst_trackmap = nullptr;
  SvtxVertexMap *dst_vertexmap = nullptr;
  
  TrkrClusterContainer *dst_clustermap = nullptr;
  TrkrCluster *cluster = nullptr;
};

#endif // WRITEMVTXHITS_H
