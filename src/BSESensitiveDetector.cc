#include "BSESensitiveDetector.hh"
#include "G4Step.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include <sstream>
#include <sys/stat.h>

// =========================
// 静态成员定义
// =========================
std::map<G4double, std::vector<G4double>> BSESensitiveDetector::energyBucketsAl;
std::map<G4double, std::vector<G4double>> BSESensitiveDetector::energyBucketsSiC;

// CSV 文件流
std::ofstream BSESensitiveDetector::s_eventFile;
std::ofstream BSESensitiveDetector::s_stepFile;
G4bool BSESensitiveDetector::s_csvInitialized = false;
G4bool BSESensitiveDetector::s_eventHeaderWritten = false;
G4bool BSESensitiveDetector::s_stepHeaderWritten = false;

// =========================
// CSV 文件初始化
// =========================
void BSESensitiveDetector::InitCSVFiles() {
  if (s_csvInitialized) return;

  // 创建输出目录
  #ifdef _WIN32
    system("if not exist OutPut mkdir OutPut");
  #else
    mkdir("OutPut", 0755);
  #endif

  // 打开事件级数据文件
  s_eventFile.open("OutPut/event_data.csv", std::ios::out);
  if (!s_eventFile.is_open()) {
    G4cerr << "ERROR: Cannot create OutPut/event_data.csv" << G4endl;
    return;
  }

  // 写入事件级CSV表头
  s_eventFile << "EventID,ParticleName,IncidentEnergy_keV,TotalEdep_keV,MaxDepthSiC_um,"
              << "TrackLength_mm,NumSteps,NumSecondaries,MeanEdepPerStep_keV,EdepStdDev_keV,"
              << "ParticleStopped,FinalKineticEnergy_keV,EnergyEscapeFraction,"
              << "BraggPeakDepth_um,BraggPeakIntensity_keV,SecondaryTypes" << std::endl;

  s_eventHeaderWritten = true;

  // 打开步级数据文件
  s_stepFile.open("OutPut/step_data.csv", std::ios::out);
  if (!s_stepFile.is_open()) {
    G4cerr << "ERROR: Cannot create OutPut/step_data.csv" << G4endl;
    return;
  }

  // 写入步级CSV表头
  s_stepFile << "EventID,ParticleName,TrackID,ParentID,"
             << "PreX_mm,PreY_mm,PreZ_mm,PostX_mm,PostY_mm,PostZ_mm,"
             << "Depth_um,PreKineticEnergy_keV,PostKineticEnergy_keV,Edep_keV,"
             << "StepLength_mm,GlobalTime_ns,"
             << "MomentumDirX,MomentumDirY,MomentumDirZ,ProcessName" << std::endl;

  s_stepHeaderWritten = true;
  s_csvInitialized = true;

  G4cout << "==> CSV files initialized successfully" << G4endl;
  G4cout << "    - OutPut/event_data.csv" << G4endl;
  G4cout << "    - OutPut/step_data.csv" << G4endl;
}

// =========================
// 关闭 CSV 文件
// =========================
void BSESensitiveDetector::CloseCSVFiles() {
  if (s_eventFile.is_open()) {
    s_eventFile.close();
    G4cout << "==> event_data.csv saved" << G4endl;
  }
  if (s_stepFile.is_open()) {
    s_stepFile.close();
    G4cout << "==> step_data.csv saved" << G4endl;
  }
  s_csvInitialized = false;
}

// =========================
// 构造函数和析构函数
// =========================
BSESensitiveDetector::BSESensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) {
  InitCSVFiles();
}

BSESensitiveDetector::~BSESensitiveDetector() {
}

// =========================
// ProcessHits: 处理每一步
// =========================
G4bool BSESensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

  // 获取能量沉积
  G4double edep = step->GetTotalEnergyDeposit();
  G4double edep_keV = edep / keV;

  // 只记录有能量沉积的步骤
  if (edep_keV <= 0.) return true;

  // 获取步的基本信息
  G4Track* track = step->GetTrack();
  G4String particleName = track->GetDefinition()->GetParticleName();
  G4int trackID = track->GetTrackID();
  G4int parentID = track->GetParentID();

  // 获取前后位置
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4StepPoint* postStepPoint = step->GetPostStepPoint();

  G4ThreeVector prePos = preStepPoint->GetPosition();
  G4ThreeVector postPos = postStepPoint->GetPosition();

  G4double preX_mm = prePos.x() / mm;
  G4double preY_mm = prePos.y() / mm;
  G4double preZ_mm = prePos.z() / mm;
  G4double postX_mm = postPos.x() / mm;
  G4double postY_mm = postPos.y() / mm;
  G4double postZ_mm = postPos.z() / mm;

  // 计算深度
  G4double surfaceZ_mm = 0.2;
  G4double depth_um = (surfaceZ_mm - postZ_mm) * 1000.0;

  // 获取前后能量
  G4double preKE_keV = preStepPoint->GetKineticEnergy() / keV;
  G4double postKE_keV = postStepPoint->GetKineticEnergy() / keV;

  // 步长和时间
  G4double stepLength_mm = step->GetStepLength() / mm;
  G4double globalTime_ns = track->GetGlobalTime() / ns;

  // 动量方向
  G4ThreeVector momDir = preStepPoint->GetMomentumDirection();
  G4double momDirX = momDir.x();
  G4double momDirY = momDir.y();
  G4double momDirZ = momDir.z();

  // 物理过程
  const G4VProcess* process = postStepPoint->GetProcessDefinedStep();
  G4String processName = (process != nullptr) ? process->GetProcessName() : "None";

  // 体积名称
  G4String volumeName = preStepPoint->GetTouchableHandle()->GetVolume()->GetName();

  // 写入步级数据
  if (s_stepFile.is_open()) {
    s_stepFile << std::fixed << std::setprecision(6);
    s_stepFile << eventID << ","
               << particleName << ","
               << trackID << ","
               << parentID << ","
               << preX_mm << ","
               << preY_mm << ","
               << preZ_mm << ","
               << postX_mm << ","
               << postY_mm << ","
               << postZ_mm << ","
               << depth_um << ","
               << preKE_keV << ","
               << postKE_keV << ","
               << edep_keV << ","
               << stepLength_mm << ","
               << globalTime_ns << ","
               << momDirX << ","
               << momDirY << ","
               << momDirZ << ","
               << processName << std::endl;
  }

  // =============================
  // 事件级统计
  // =============================

  if (trackID == 1 && eventIncidentKeV.find(eventID) == eventIncidentKeV.end()) {
    eventIncidentKeV[eventID] = preKE_keV;
    eventParticleType[eventID] = particleName;
  }

  EventFeatures& features = eventFeaturesMap[eventID];

  if (features.particleName.empty()) {
    features.particleName = particleName;
    features.incidentEnergy_keV = preKE_keV;
  }

  features.totalEdep_keV += edep_keV;
  features.nSteps++;
  features.trackLength_mm += stepLength_mm;

  if (volumeName.contains("SiC")) {
    if (depth_um > eventMaxDepthSiCUm[eventID]) {
      eventMaxDepthSiCUm[eventID] = depth_um;
    }
    features.maxDepthSiC_um = eventMaxDepthSiCUm[eventID];
  }

  eventEdepList[eventID].push_back(edep_keV);
  eventDepthList[eventID].push_back(depth_um);
  eventEdepAtDepth[eventID].push_back(edep_keV);

  if (parentID > 0) {
    features.secondaryTypes.insert(particleName);
  }

  return true;
}

// =========================
// EndOfEvent
// =========================
void BSESensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
  G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

  if (eventFeaturesMap.find(eventID) == eventFeaturesMap.end()) {
    return;
  }

  EventFeatures& features = eventFeaturesMap[eventID];

  // 计算平均值
  if (features.nSteps > 0) {
    features.meanEdepPerStep_keV = features.totalEdep_keV / features.nSteps;
  }

  // 计算标准差
  if (eventEdepList[eventID].size() > 1) {
    G4double sum_sq = 0.0;
    G4double mean = features.meanEdepPerStep_keV;
    for (const auto& edep : eventEdepList[eventID]) {
      sum_sq += (edep - mean) * (edep - mean);
    }
    features.edepStdDev_keV = std::sqrt(sum_sq / eventEdepList[eventID].size());
  }

  features.nSecondaries = features.secondaryTypes.size();

  // 计算新增特征
  G4double incident_energy = eventIncidentKeV[eventID];
  G4double total_edep = features.totalEdep_keV;
  G4double final_energy = incident_energy - total_edep;
  G4double escape_fraction = (incident_energy > 0) ? (final_energy / incident_energy) : 0.0;

  G4bool particleStopped = (escape_fraction < 0.05);
  G4double finalKineticEnergy = final_energy;
  G4double energyEscapeFraction = escape_fraction;

  // 计算 Bragg 峰
  G4double braggPeakDepth = 0.0;
  G4double braggPeakIntensity = 0.0;

  std::vector<G4double>& depths = eventDepthList[eventID];
  std::vector<G4double>& edeps_at_depth = eventEdepAtDepth[eventID];

  if (!depths.empty()) {
    size_t max_idx = 0;
    G4double max_edep = 0.0;
    for (size_t i = 0; i < edeps_at_depth.size(); ++i) {
      if (edeps_at_depth[i] > max_edep) {
        max_edep = edeps_at_depth[i];
        max_idx = i;
      }
    }
    braggPeakDepth = depths[max_idx];
    braggPeakIntensity = max_edep;
  }

  // 写入事件数据
  if (s_eventFile.is_open()) {
    std::ostringstream secondaryTypesStr;
    for (auto it = features.secondaryTypes.begin(); it != features.secondaryTypes.end(); ++it) {
      if (it != features.secondaryTypes.begin()) {
        secondaryTypesStr << ";";
      }
      secondaryTypesStr << *it;
    }

    s_eventFile << std::fixed << std::setprecision(6);
    s_eventFile << eventID << ","
                << features.particleName << ","
                << features.incidentEnergy_keV << ","
                << features.totalEdep_keV << ","
                << features.maxDepthSiC_um << ","
                << features.trackLength_mm << ","
                << features.nSteps << ","
                << features.nSecondaries << ","
                << features.meanEdepPerStep_keV << ","
                << features.edepStdDev_keV << ","
                << (particleStopped ? "true" : "false") << ","
                << finalKineticEnergy << ","
                << energyEscapeFraction << ","
                << braggPeakDepth << ","
                << braggPeakIntensity << ","
                << secondaryTypesStr.str() << std::endl;
  }

  // 清理
  eventFeaturesMap.erase(eventID);
  eventEdepList.erase(eventID);
  eventIncidentKeV.erase(eventID);
  eventMaxDepthSiCUm.erase(eventID);
  eventMaxDepthAlUm.erase(eventID);
  eventParticleType.erase(eventID);
  eventDepthList.erase(eventID);
  eventEdepAtDepth.erase(eventID);
}

// =========================
// EndOfRun
// =========================
void BSESensitiveDetector::EndOfRun() {
  G4cout << "======================================" << G4endl;
  G4cout << "Closing CSV files..." << G4endl;
  G4cout << "======================================" << G4endl;

  CloseCSVFiles();

  G4cout << "======================================" << G4endl;
  G4cout << "CSV data saved to OutPut/ directory" << G4endl;
  G4cout << "======================================" << G4endl;
}
