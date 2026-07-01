#include <BinF/Engine.hpp>
#include <BinF/Engine/Internal.hpp>



namespace BinF::Engine {
    void Init() {
        InitInput();
        InitRenderer();
    }

    void Update() {
        UpdateTime();
        UpdateInput();
    }

    void Exit() {
        ExitRenderer();
    }
}