#ifndef BSESensitiveDetector_h
#define BSESensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include <map>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <set>
#include <string>

// 每个事件的特征数据结构
struct EventFeatures {
  G4String particleName;           // 粒子类型
  G4double incidentEnergy_keV;     // 入射能量
  G4double totalEdep_keV;          // 总能量沉积
  G4double maxDepthAl_um;          // 铝层最大深度
  G4double maxDepthSi_um;          // 硅层最大深度
  G4double trackLength_mm;         // 总径迹长度
  G4int    nSteps;                 // 总步数
  G4int    nSecondaries;           // 产生的二次粒子数
  G4double meanEdepPerStep_keV;    // 平均每步能量沉积
  G4double edepStdDev_keV;         // 能量沉积标准差
  std::set<G4String> secondaryTypes; // 二次粒子类型集合
};

class BSESensitiveDetector : public G4VSensitiveDetector {
public:
  BSESensitiveDetector(const G4String& name);
  ~BSESensitiveDetector() override;

  G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
  void   EndOfEvent(G4HCofThisEvent*) override;
  void   EndOfRun(); // RunAction 结束时调用

private:
  // ===============================
  // 原有统计：事件最大深度与入射能量
  // ===============================
  std::map<G4int, G4double> eventMaxDepthAlUm;  // 每个 event 铝层最大深度
  std::map<G4int, G4double> eventMaxDepthSiUm;  // 每个 event 硅层最大深度
  std::map<G4int, G4double> eventIncidentKeV;   // 每个 event 入射能量

  // 能量桶（静态，Run 层面）
  static std::map<G4double, std::vector<G4double>> energyBucketsAl;
  static std::map<G4double, std::vector<G4double>> energyBucketsSi;

  static G4double BucketizeEnergy(G4double keV) {
    return std::round(keV * 10.0) / 10.0; // 0.1 keV 分桶
  }

  // ========================================
  // CSV 文件输出
  // ========================================
  static std::ofstream s_eventFile;      // 事件级数据文件
  static std::ofstream s_stepFile;       // 步级数据文件
  static G4bool s_csvInitialized;
  static G4bool s_eventHeaderWritten;
  static G4bool s_stepHeaderWritten;

  static void InitCSVFiles();
  static void CloseCSVFiles();

  // ========================================
  // 内部数据存储
  // ========================================
  std::map<G4int, EventFeatures> eventFeaturesMap;  // 每个事件的特征
  std::map<G4int, std::vector<G4double>> eventEdepList; // 每个事件的能量沉积列表
  std::map<G4int, G4String> eventParticleType;      // 每个事件的粒子类型

  // 用于计算Bragg峰的深度-能量数据
  std::map<G4int, std::vector<G4double>> eventDepthList;  // 每个事件的深度列表
  std::map<G4int, std::vector<G4double>> eventEdepAtDepth; // 每个事件在各深度的能量沉积
};

#endif
