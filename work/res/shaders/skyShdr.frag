#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

//need height of camera
//Need the height of the atmosphere
//need the height of the sun
//need the viewing direction

//The sun is 695700 Kms away from earth (could use 695 as an approximation)
//8Kms is how thick the atmoshpere is
//rayleigh scattering coefficients rgb = 5.8e^-6, 1.35e^-5, 3.31e^-5
//mie scattering coefficient is
//get the 'view' of the sun by using glLookAt()



//L(x,v,s) 
//x = radiance of light reaching camera
//v = direction of vector reaching x
//

uniform int iSteps = 16;
uniform int jSteps = 8;
uniform float PI = 3.141592;

precision highp float;

in vec3 vPosition;

uniform vec3 uSunPos;

//Ray Sphere intersection to test where ray hits from the sun
vec2 rsi(vec3 r0, vec3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5,-1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}


/*
    1. First calculate the amount of steps to calculate the Mie and Rayleigh scattering from where the 
    primary ray from the sun towards the viewer (camera) intersects the atmosphere 
    2. Then calculate the offset from the planet size to get the thickness of the atmosphere and how many steps through the
    atmosphere is visible towards the viewer
    3. Initialise a time counter and accumulators for the Mie and Rayleigh scattering and the depth accumulators for the ray
    4. We then calculate the Rayleigh phase function which has the form PR(μ)=3/16π*(1+μ^2) where μ (mu) is the cosine of 
    angle between the light ray and view ray
    5. We also calculate the Mie phase function which has the form PM(μ)=3/8π*((1−g^2)(1+μ^2)/((2+g^2)(1+g^2−2gμ)^(3/2))) where
    additionally to μ (mu) there is g which is the anisotropy of the particles or scattering direction
    6. We then go through our set number of sample points along the ray and accumulate the optical depth for the primary ray, 
    and for each of the set secondary rays we accumulate the amount of light at that point including attentuation
    7. Finally we multiply the intensity of the sun by the Rayleigh Phase function by the total amount of rayleigh light 
    accumulated by the scattering coeffecients calculated for the earth, and then add that to the intensity of the sun multiplied
    by the Mie Phase by amount of Mie light scattered by the Mie scattering coefficient
    8. This final value will be your final output color for a pixel.

*/






vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rEarth, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0,0,0);
    p.y = min(p.y, rsi(r0, r, rEarth).x);
    float iStepSize = (p.y - p.x) / float(iSteps);

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float RlhPhase = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float MiePhase = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rEarth;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odSteRlhPhase = exp(-iHeight / shRlh) * iStepSize;
        float odSteMiePhase = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odSteRlhPhase;
        iOdMie += odSteMiePhase;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {

            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rEarth;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odSteRlhPhase * attn;
        totalMie += odSteMiePhase * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (RlhPhase * kRlh * totalRlh + MiePhase * kMie * totalMie);
}

void main() {
    vec3 color = atmosphere(
        normalize(vPosition),           // normalized ray direction
        vec3(0,6372e3,0),               // ray origin
        uSunPos,                        // position of the sun
        22.0,                           // intensity of the sun
        6371e3,                         // radius of the planet in meters
        6471e3,                         // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
        8e3,                            // Rayleigh scale height
        1.2e3,                          // Mie scale height
        0.76                            // Mie preferred scattering direction
    );

    // Apply exposure.
    color = 1.0 - exp(-1.0 * color);

    FragColor = vec4(color, 1);
}



