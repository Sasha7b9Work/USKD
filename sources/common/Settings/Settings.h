// 2023/06/22 13:59:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Settings
{
    int GetKoeffCurrent();          // Коэфццициент преобразования тока

    int GetNumberSteps();           // Число ступеней регулирования. Если -1, то регулирование не производится

    bool IsControllingRelays();     // Если true, то контролировать состояние реле и отключать фазу, если хотя бы одно реле на ней неисправно

    bool OnlyMeasure();             // Если true, то производится только измерение, без регулирования
};


extern Settings gset;
