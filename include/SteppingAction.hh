#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;
class G4Step;

/// SteppingAction class
/// 记录每一步的详细信息：
/// - 前后位置 (x, y, z)
/// - 前后能量
/// - 能量沉积
/// - 步长
/// - 粒子类型
/// - 体积名称
/// - 动量方向

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    // 每一步都会调用这个函数
    virtual void UserSteppingAction(const G4Step* step);

private:
    EventAction* fEventAction;
};

#endif
