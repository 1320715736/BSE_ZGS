#include "SteppingAction.hh"
#include "EventAction.hh"
#include "BSESensitiveDetector.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // ========================================
    // 获取基本信息
    // ========================================

    // 获取粒子轨迹
    G4Track* track = step->GetTrack();

    // 获取前后步点
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();

    // ========================================
    // 提取详细的步级数据
    // ========================================

    // 1. 位置信息 (mm)
    G4ThreeVector prePos = preStepPoint->GetPosition();
    G4ThreeVector postPos = postStepPoint->GetPosition();

    G4double pre_x = prePos.x() / mm;
    G4double pre_y = prePos.y() / mm;
    G4double pre_z = prePos.z() / mm;

    G4double post_x = postPos.x() / mm;
    G4double post_y = postPos.y() / mm;
    G4double post_z = postPos.z() / mm;

    // 2. 能量信息 (keV)
    G4double preKineticEnergy = preStepPoint->GetKineticEnergy() / keV;
    G4double postKineticEnergy = postStepPoint->GetKineticEnergy() / keV;
    G4double energyDeposit = step->GetTotalEnergyDeposit() / keV;

    // 3. 步长信息 (mm)
    G4double stepLength = step->GetStepLength() / mm;

    // 4. 粒子信息
    G4String particleName = track->GetDefinition()->GetParticleName();
    G4int trackID = track->GetTrackID();
    G4int parentID = track->GetParentID();

    // 5. 体积信息
    G4VPhysicalVolume* preVolume = preStepPoint->GetPhysicalVolume();
    G4VPhysicalVolume* postVolume = postStepPoint->GetPhysicalVolume();

    G4String preVolumeName = preVolume ? preVolume->GetName() : "OutOfWorld";
    G4String postVolumeName = postVolume ? postVolume->GetName() : "OutOfWorld";

    // 6. 动量方向
    G4ThreeVector momentum = preStepPoint->GetMomentumDirection();
    G4double mom_x = momentum.x();
    G4double mom_y = momentum.y();
    G4double mom_z = momentum.z();

    // 7. 过程信息
    const G4VProcess* process = postStepPoint->GetProcessDefinedStep();
    G4String processName = process ? process->GetProcessName() : "None";

    // 8. 时间信息 (ns)
    G4double globalTime = preStepPoint->GetGlobalTime() / ns;
    G4double localTime = preStepPoint->GetLocalTime() / ns;

    // ========================================
    // 可选：打印调试信息（只打印前几步）
    // ========================================
    /*
    static G4int stepCount = 0;
    if (stepCount < 10) {
        G4cout << "Step " << stepCount
               << " | Particle: " << particleName
               << " | Pre: (" << pre_x << ", " << pre_y << ", " << pre_z << ")"
               << " | Post: (" << post_x << ", " << post_y << ", " << post_z << ")"
               << " | Edep: " << energyDeposit << " keV"
               << " | PreE: " << preKineticEnergy << " keV"
               << " | PostE: " << postKineticEnergy << " keV"
               << " | Process: " << processName
               << G4endl;
        stepCount++;
    }
    */

    // ========================================
    // 累积事件级的能量沉积（如果需要）
    // ========================================
    if (energyDeposit > 0.) {
        fEventAction->AddEdep(energyDeposit);
    }

    // ========================================
    // 注意：步级数据的详细记录已在 BSESensitiveDetector 中完成
    // 这里主要用于：
    // 1. 调试和验证
    // 2. 记录非敏感体积中的步（如果需要）
    // 3. 累积事件级的统计量
    // ========================================
}
