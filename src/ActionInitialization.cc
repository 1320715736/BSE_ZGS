#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization() : G4VUserActionInitialization() {}
ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const {
    // 设置主粒子发生器
    SetUserAction(new PrimaryGeneratorAction());

    // 设置 Run 动作
    RunAction* runAction = new RunAction();
    SetUserAction(runAction);

    // 设置 Event 动作
    EventAction* eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    // 设置 Stepping 动作 - 记录每一步的详细信息
    SetUserAction(new SteppingAction(eventAction));
}

