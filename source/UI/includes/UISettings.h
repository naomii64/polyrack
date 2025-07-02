struct UISettingScalar
{
    float percentage=0.0f;
    float units=0.0f;

    float toUnits(float relativeTo){
        return (percentage*relativeTo)+(units);
    }
};