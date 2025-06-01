#ifndef RIPPLE_FUNCTION_INCLUDE
#define RIPPLE_FUNCTION_INCLUDE

float SpawnRipple(float rippleWidth, float rippleSpeed, float timeSinceSpawn, float timeScale, float waveInterval, float delay, float lifetime, float2 uv, float2 spawnPos)
{
    // Correct ripple's shape base on screen aspect
    float aspect = _ScreenParams.x / _ScreenParams.y;
    float2 correctedUV = float2(uv.x * aspect, uv.y);
    
    float dist = length(spawnPos - correctedUV);

    // [waveInterval] is time between two wave, [delay] is time before the ripple start 
    // float t = fmod(max(0, timeNow), waveInterval);
    float t = max(0, timeSinceSpawn - delay) * timeScale;
    
    float waveFront = t * rippleSpeed;
    float dynamicWidth = (t <= lifetime) ? rippleWidth * sin((t * PI) / lifetime) : 0.0;
    
    float innerRadius = waveFront;
    float outerRadius = (t > 0) ? innerRadius + dynamicWidth : innerRadius;

    float isInRing = step(innerRadius, dist) * step(dist, outerRadius);
    
    return isInRing;
}

void Ripple_float(float rippleWidth, float rippleSpeed, float spawnTime, float timeNow, float timeScale, float waveInterval, float delay, float lifetime, float2 uv, out float ripple)
{
    float result = 0.0;
    float timeSinceSpawn = timeNow - spawnTime;
    
    if (spawnTime >= 0.0)
    { 
        {
            float2 pos = float2(0.0, 0.0);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.3, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.6, lifetime, uv, pos);
        }
        
        { 
            float2 pos = float2(-0.35, 0.15);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.3, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.6, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.9, lifetime, uv, pos);
        }
        
        { 
            float2 pos = float2(-0.4, -0.35);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.3, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.6, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.9, lifetime, uv, pos);
        }
        
        {
            float2 pos = float2(0.4, -0.35);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.5, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 0.8, lifetime, uv, pos);
            result += SpawnRipple(rippleWidth, rippleSpeed, timeSinceSpawn, timeScale, waveInterval, delay + 1.1, lifetime, uv, pos);
        }
        
        result = saturate(result);
    } 

    ripple = result;
}

#endif