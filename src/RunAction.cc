#include "RunAction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "BSESensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <iomanip>
#include <iostream>

RunAction::RunAction()
 : G4UserRunAction()
{}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    G4cout << "=== Run " << run->GetRunID() << " started ===" << G4endl;

    // 创建输出目录
    system("mkdir -p ./OutPut");
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4cout << "=== Run " << run->GetRunID() << " finished ===" << G4endl;

    // 尝试获取敏感探测器实例
    auto sd = dynamic_cast<BSESensitiveDetector*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("BSESD"));

    if (sd) {
        sd->EndOfRun();
        G4cout << "✅ Depth statistics written to Energy_vs_Depth.csv" << G4endl;
    } else {
        G4cout << "⚠ No BSESensitiveDetector found — skipping depth summary." << G4endl;
    }
}

