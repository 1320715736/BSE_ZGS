#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "BSESensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.),
  EventID(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{
    // 重置事件能量累积
    fEdep = 0.;

    // 获取事件ID
    EventID = event->GetEventID();

    // 每100个事件打印一次进度
    if (EventID % 100 == 0) {
        G4cout << ">>> Event " << EventID << " started" << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
    // 事件结束时可以在这里做一些统计
    // 注意：详细的步级数据已经在 BSESensitiveDetector::ProcessHits() 中记录
    // 这里主要用于事件级的汇总

    // 可选：打印事件摘要
    /*
    if (fEdep > 0.) {
        G4cout << "Event " << EventID
               << " | Total Edep (from stepping): " << fEdep << " keV"
               << G4endl;
    }
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
