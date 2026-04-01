#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4StepLimiterPhysics.hh"
#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"


int main(int argc, char** argv) {

    // 1. 创建 RunManager
    auto* runManager = new G4RunManager;

    // 2. 初始化几何
    runManager->SetUserInitialization(new DetectorConstruction());

    // 3. 使用 Shielding 物理列表（适合 keV-MeV 低能粒子）
    // 其他可选物理列表：
    //   - "Shielding": 最适合低能粒子 (keV-MeV)，辐射屏蔽专用
    //   - "QGSP_BERT_EMV": 增强电磁物理，适合 keV-GeV
    //   - "FTFP_BERT": 标准通用物理列表
    G4PhysListFactory factory;
    G4VModularPhysicsList* physicsList = nullptr;
    G4String physicsListName = "Shielding";  // 可以改为 "QGSP_BERT_EMV" 或 "FTFP_BERT"

    if (factory.IsReferencePhysList(physicsListName)) {
        physicsList = factory.GetReferencePhysList(physicsListName);
        // 添加 StepLimiter 过程以强制执行 UserLimits
        physicsList->RegisterPhysics(new G4StepLimiterPhysics());
        G4cout << "Using physics list: " << physicsListName << G4endl;
        G4cout << "StepLimiterPhysics registered to enforce UserLimits" << G4endl;
    } else {
        G4cerr << "Error: cannot find " << physicsListName << " physics list!" << G4endl;
        return 1;
    }
    runManager->SetUserInitialization(physicsList);


    // 注册 RunAction
    runManager->SetUserAction(new RunAction());

    // 4. 用户动作
    runManager->SetUserInitialization(new ActionInitialization());

    // 5. 初始化运行
    runManager->Initialize();

    // 6. 获取 UI 管理器
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // 7. 判断运行模式
    G4UIExecutive* uiExec = nullptr;
    if (argc == 1) { 
        // 没有命令行参数 -> 图形交互模式
        uiExec = new G4UIExecutive(argc, argv);

        // 初始化可视化
        auto* visManager = new G4VisExecutive;
        visManager->Initialize();

        // 载入默认可视化宏文件
        UImanager->ApplyCommand("/control/execute vis.mac");

        // 启动交互会话
        uiExec->SessionStart();

        // 清理
        delete visManager;
        delete uiExec;
    } else {
        // 带宏文件参数 -> 批处理模式
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    // 8. 清理资源
    delete runManager;

    return 0;
}

