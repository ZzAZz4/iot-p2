#pragma once

struct AltitudeSensor {
    static void Init();
    static void Update();

    static float ReadAltitude();
};