/*
 * @Project: 
 * @Remark: 
 * @Author: 
 * @Institution: DUT
 * @The English name: Wyatt Foxsen
 * @E-mail: 
 * @Date: 2022-04-04 17:53:18
 */
  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
class RunAction;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction*);
   ~EventAction();

  public:
    virtual void   BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    void AddEdep(G4double edep) { fEdep += edep; }
    G4int GetEventID() {return EventID;}

  private:
    G4int EventID;
    RunAction* fRunAction= nullptr ;
    G4double   fEdep = 0.;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
