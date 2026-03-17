#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "G4StepLimiter.hh"
#include "BSESensitiveDetector.hh"

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(), fWorldLog(nullptr) {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    auto nist = G4NistManager::Instance();

    //-------------------------------
    // 1. 世界体 (World)
    //-------------------------------
    G4double worldSize = 1*mm;
    G4Material* vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    auto solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    fWorldLog = new G4LogicalVolume(solidWorld, vacuum, "World");
    auto physWorld = new G4PVPlacement(0, G4ThreeVector(), fWorldLog,
                                       "World", 0, false, 0);

    //-------------------------------
    // 2. 硅探测器体 (纯硅单层结构)
    //-------------------------------
    G4double detXY = 400 * um;
    G4double detThickness = 400 * um;

    // 使用高阻硅 (简化为纯硅)
    G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");

    // 创建几何体
    auto solidDetector = new G4Box("SiDetector", detXY/2, detXY/2, detThickness/2);
    auto logicDetector = new G4LogicalVolume(solidDetector, silicon, "SiDetector");

    // ========================================
    // 设置 UserLimits: 限制最小步长以增加step数据量
    // ========================================
    // 设置最大步长为 5 微米（增加适量的steps，避免内存溢出）
    G4double maxStepLength = 5.0 * um;  // 5 微米
    auto userLimits = new G4UserLimits(maxStepLength);
    logicDetector->SetUserLimits(userLimits);
    G4cout << "==> UserLimits set: Max step length = " << maxStepLength/um << " um" << G4endl;

    // 硅片放置在世界中心
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0),
                      logicDetector, "SiDetector", fWorldLog, false, 0);

    //-------------------------------
    // 3. 定义敏感探测器（整个硅体为探测区）
    //-------------------------------
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sd_BSE = new BSESensitiveDetector("BSESD");
    sdManager->AddNewDetector(sd_BSE);
    // 硅体为敏感体
    logicDetector->SetSensitiveDetector(sd_BSE);

    return physWorld;
}

