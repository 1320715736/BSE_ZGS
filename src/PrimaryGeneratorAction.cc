#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    fParticleSource = new G4GeneralParticleSource();

    // 默认粒子类型：电子
    // 可通过 .mac 文件修改为其他粒子：
    // - e- (电子)
    // - proton (质子)
    // - alpha (α粒子)
    // - gamma (γ射线)
    // - mu- (μ子)
    auto pdef = G4ParticleTable::GetParticleTable()->FindParticle("e-");
    fParticleSource->SetParticleDefinition(pdef);

    // 默认能量，可在 run.mac 修改
    fParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(100.0 * keV);

    // 粒子方向：垂直入射
    fParticleSource->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1));

    // === 修改为平面源 ===
    auto posDist = fParticleSource->GetCurrentSource()->GetPosDist();
    posDist->SetPosDisType("Plane");      // 平面分布
    posDist->SetPosDisShape("Square");    // 方形面
    posDist->SetCentreCoords(G4ThreeVector(0, 0, 0.3 * mm));  // 稍高于样品表面
    posDist->SetHalfX(0.15 * mm);          // 8 mm 宽
    posDist->SetHalfY(0.15* mm);          // 8 mm 高
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    // 生成事件
    fParticleSource->GeneratePrimaryVertex(anEvent);
}

G4String PrimaryGeneratorAction::GetCurrentParticleName() const {
    return fParticleSource->GetParticleDefinition()->GetParticleName();
}

