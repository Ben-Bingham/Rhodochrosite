#version 330 core
out vec4 FragColor;

in vec2 textureCordinates;

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Sphere {
	vec3 origin;
	float radius;
	vec4 colour;
	int material; // DIFFUSE = 0, REFLECTION = 1, REFRACTION = 2
};

struct DirectionalLight {
	vec3 direction;
};

struct Hit {
	bool hitSomething;
	Sphere hitSphere;
	float distanceToHit;
};

// Uniforms
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

uniform Sphere spheres[32];
uniform int numberOfSpheres;

uniform DirectionalLight dirLights[8];
uniform int numberOfdirectionalLights;

uniform float aspectRatio;
uniform int pixelWidth;
uniform int pixelHeight;

uniform float time;

#define FLT_MAX 3.402823466e+38

// Functions
vec3 at(Ray ray, float t) {
	return ray.origin + ray.direction * t;
}

float PHI = 1.61803398874989484820459;  // Golden Ratio   

vec2 randomState;

float randomFloat(float seed) {
    randomState.x = fract(sin(dot(randomState.xy, vec2(12.9898, 78.233))) * 43758.5453);
    randomState.y = fract(sin(dot(randomState.xy, vec2(12.9898, 78.233))) * 43758.5453);;
    
    return randomState.x;
}

//float randomFloat(float seed) {
//	vec2 coord = vec2(textureCordinates.x * 1000, textureCordinates.y * 1000 * aspectRatio);
//    return fract(tan(distance(coord * PHI, coord) * tan(seed)) * coord.x);
//}

float randomRange(float minVal, float maxVal, float seed) { // [min, max[
	return minVal + (maxVal - minVal) * randomFloat(seed);
}

vec3 randomVec3(float seed) {
	return vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed));
}

vec3 randomVec3InRange(float minVal, float maxVal, float seed) {
	return vec3(randomRange(minVal, maxVal, seed), randomRange(minVal, maxVal, seed), randomRange(minVal, maxVal, seed));
}

float lengthSquared(vec3 vector) {
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

//vec3 randomVec3InUnitSphere(float seed) {
//	int i = 0;
//	while (true) {
//		i++;
//        vec3 vec = randomVec3InRange(-1,1, seed * i);
//        if (lengthSquared(vec) >= 1) continue;
//        return vec;
//	}
//}

const float PI = 3.14159265359;

vec3 randomVec3InUnitSphere(float seed) {
    // This function was taken from:
    //https://github.com/riccardoprosdocimi/real-time-ray-tracer/blob/master/shaders/frag.glsl
	vec3 randomVector = randomVec3(seed);
	float phi = 2.0 * PI * randomVector.x;
	float cosTheta = 2.0 * randomVector.y - 1.0;
	float u = randomVector.z;
    
	float theta = acos(cosTheta);
	float r = pow(u, 1.0 / 3.0);
    
	float x = r * sin(theta) * cos(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(theta);
    
	return vec3(x, y, z);
}

bool nearZero(vec3 vector) {
	float scaler = 1 * pow(10, -8);
	return (abs(vector.x) < scaler) && (abs(vector.y) < scaler) && (abs(vector.z) < scaler);
}

Hit hitSphere(Ray ray) {
	Hit sphereHit;
	sphereHit.hitSomething = false;
	sphereHit.distanceToHit = FLT_MAX;
	for (int i = 0; i < numberOfSpheres; i++) {

		vec3 origin = ray.origin - spheres[i].origin;

		// Discriminant calculations
		float a = dot(ray.direction, ray.direction);
		float b = 2.0 * dot(origin, ray.direction);
		float c = dot(origin, origin) - (spheres[i].radius * spheres[i].radius);

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0) {
			continue;
		}

		// Solving the quadratic formula
		float sqrtDiscriminant = sqrt(discriminant);
		float solution1 = (-b - sqrtDiscriminant) / 2.0 * a;
		float solution2 = (-b + sqrtDiscriminant) / 2.0 * a;
		float solution;
		if (solution1 < solution2) {
			solution = solution1;
		}
		else {
			solution = solution2;
		}
		if (solution < 0.0) {
			continue;
		}

		if (solution < sphereHit.distanceToHit) {
			sphereHit.distanceToHit = solution;
			sphereHit.hitSphere = spheres[i];
			sphereHit.hitSomething = true;
		}
	}
	return sphereHit;
}

Ray scatterRayDiffuse(Ray incidentRay, Hit hit, vec3 hitLocation, vec3 normal, float randSeed) {
	vec3 scatterDirection = normal + normalize(randomVec3InUnitSphere(randSeed));
	return Ray(hitLocation + (normal * 0.00001), normalize(scatterDirection));
}

vec3 backgroundColour = vec3(0.6, 0.7, 0.9);
int maxNumberOfBounces = 50;
int numberOfSamples = 4;
float distanceToImagePlane = 1.0;

void main() {
	randomState = textureCordinates.xy * time;

	//highp float randSeed = (time / time - time - time * time + time) / time;

	vec3 colour = vec3(0, 0, 0);
	for (int i = 0; i < numberOfSamples; i++) {
		float randX = randomRange(0, 1, 3.2042 + i);
		float randY = randomRange(0, 1, 2.453 + i);

		randX /= pixelWidth;
		randY /= pixelHeight;

		vec2 texCords;
		texCords.x = (textureCordinates.x + randX) * 2.0 - 1.0;
		texCords.y = ((textureCordinates.y + randY) * 2.0 - 1.0) * aspectRatio;

		vec3 cameraRight = cross(cameraDirection, vec3(0, -1, 0));
		vec3 cameraUp = cross(cameraDirection, cameraRight);

		vec3 imagePoint = ((-texCords.x) * cameraRight) + ((texCords.y) * cameraUp) + (cameraDirection * distanceToImagePlane) + cameraPosition;
		vec3 rayDirection = imagePoint - cameraPosition;

		Ray ray = Ray(cameraPosition, normalize(rayDirection));

		vec3 tempColour = vec3(0);
		float multiplier = 1.0;
		int j = 0;
		for (; j < maxNumberOfBounces; j++) {
			Hit hit = hitSphere(ray);

			if (hit.hitSomething == false) { // Miss
				tempColour += backgroundColour * multiplier;
				break;
			}
			
			vec3 origin = ray.origin - hit.hitSphere.origin;
			vec3 hitLocation = origin + ray.direction * hit.distanceToHit;
			vec3 normal = normalize(hitLocation);

			hitLocation += hit.hitSphere.origin;

			float lightIntensity = 0.0;
			for (int i = 0; i < numberOfdirectionalLights; i++) {
				lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
			}

			lightIntensity = clamp(lightIntensity, 0.0, 1.0);

			tempColour += hit.hitSphere.colour.xyz * multiplier * lightIntensity;
			multiplier *= 0.5;

			ray = scatterRayDiffuse(ray, hit, hitLocation, normal, 0.5423442);
		}
		colour += tempColour;
	}

	colour /= numberOfSamples;
	FragColor = vec4(colour, 1);
	return;
}